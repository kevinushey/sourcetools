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
    std::size_t size;
    if (!conn.size(&size))
      return false;

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
