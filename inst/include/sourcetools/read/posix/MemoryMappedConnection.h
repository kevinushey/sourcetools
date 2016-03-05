#ifndef SOURCE_TOOLS_READ_POSIX_MEMORY_MAPPED_CONNECTION_H
#define SOURCE_TOOLS_READ_POSIX_MEMORY_MAPPED_CONNECTION_H

#include <sys/mman.h>
#include <fcntl.h>

namespace sourcetools {
namespace detail {

class MemoryMappedConnection
{
public:

  MemoryMappedConnection(int fd, std::size_t size)
    : size_(size)
  {
#ifdef MAP_POPULATE
    map_ = (char*) ::mmap(0, size, PROT_READ, MAP_SHARED | MAP_POPULATE, fd, 0);
#else
    map_ = (char*) ::mmap(0, size, PROT_READ, MAP_SHARED, fd, 0);
#endif
  }

  ~MemoryMappedConnection()
  {
    if (map_ != MAP_FAILED)
      ::munmap(map_, size_);
  }

  bool open()
  {
    return map_ != MAP_FAILED;
  }

  operator char*() const
  {
    return map_;
  }

private:
  char* map_;
  std::size_t size_;
};

} // namespace detail
} // namespace sourcetools

#endif /* SOURCE_TOOLS_READ_POSIX_MEMORY_MAPPED_CONNECTION_H */
