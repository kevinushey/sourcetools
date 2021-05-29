#ifndef SOURCETOOLS_READ_MEMORY_MAPPED_READER_H
#define SOURCETOOLS_READ_MEMORY_MAPPED_READER_H

#include <vector>
#include <string>
#include <algorithm>

#include <sourcetools/core/macros.h>

#include <sourcetools/r/RHeaders.h>
#include <sourcetools/r/RUtils.h>

#ifndef _WIN32
# include <sourcetools/read/posix/FileConnection.h>
# include <sourcetools/read/posix/MemoryMappedConnection.h>
#else
# include <sourcetools/read/windows/FileConnection.h>
# include <sourcetools/read/windows/MemoryMappedConnection.h>
#endif

namespace sourcetools {
namespace detail {

class MemoryMappedReader
{
public:

  class VectorReader
  {
  public:

    explicit VectorReader(std::vector<std::string>* pData)
      : pData_(pData)
    {
    }

    template <typename T>
    void operator()(const T& lhs, const T& rhs)
    {
      pData_->push_back(std::string(lhs, rhs));
    }

  private:
    std::vector<std::string>* pData_;
  };

  static bool read(const char* path, std::string* pContent)
  {
    // Open file connection
    FileConnection conn(path);
    if (!conn.open())
      return false;

    // Get size of file
    index_type size;
    if (!conn.size(&size))
      return false;

    // Early return for empty files
    if (UNLIKELY(size == 0))
      return true;

    // mmap the file
    MemoryMappedConnection map(conn, size);
    if (!map.open())
      return false;

    pContent->assign(map, size);
    return true;
  }

  template <typename F>
  static bool read_lines(const char* path, F f)
  {
    FileConnection conn(path);
    if (!conn.open())
      return false;

    // Get size of file
    index_type size;
    if (!conn.size(&size))
      return false;

    // Early return for empty files
    if (UNLIKELY(size == 0))
      return true;

    // mmap the file
    MemoryMappedConnection map(conn, size);
    if (!map.open())
      return false;

    // special case: just a '\n'
    bool endsWithNewline =
      map[size - 1] == '\n' ||
      map[size - 1] == '\r';
    
    if (size == 1 && endsWithNewline)
      return true;

    // Search for newlines
    const char* lower = map;
    const char* end = map + size;
    
    for (const char* it = lower; it != end; it++)
    {
      // check for newline
      char ch = *it;
      bool isNewline = ch == '\r' || ch == '\n';
      if (!isNewline)
        continue;
      
      // found a newline; call functor
      f(lower, it);
      
      // update iterator, handling '\r\n' specially
      if (it[0] == '\r' &&
          it[1] == '\n')
      {
        it += 1;
      }
      
      // update lower iterator
      lower = it + 1;
      
    }

    // If this file ended with a newline, we're done
    if (endsWithNewline)
      return true;

    // Otherwise, consume one more string, then we're done
    f(lower, end);
    return true;
  }

  static bool read_lines(const char* path, std::vector<std::string>* pContent)
  {
    VectorReader reader(pContent);
    return read_lines(path, reader);
  }

};

} // namespace detail
} // namespace sourcetools

#endif /* SOURCETOOLS_READ_MEMORY_MAPPED_READER_H */
