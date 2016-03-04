#ifndef SOURCE_TOOLS_READ_MEMORY_MAPPED_READER_H
#define SOURCE_TOOLS_READ_MEMORY_MAPPED_READER_H

#include <string>

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

  static bool read(const char* path, std::string* pContent)
  {
    // Open file connection
    FileConnection conn(path);
    if (!conn.open())
      return false;

    // Get size of file
    struct stat info;
    if (::fstat(conn, &info) == -1)
      return false;
    off_t size = info.st_size;

    // Return early for empty files
    if (size <= 0)
    {
      *pContent = std::string();
      return true;
    }

    // mmap the file
    MemoryMappedConnection map(conn, size);
    if (!map.open())
      return false;

    pContent->assign(map, size);
    return true;
  }

};

} // namespace detail
} // namespace sourcetools

#endif /* SOURCE_TOOLS_READ_MEMORY_MAPPED_READER_H */
