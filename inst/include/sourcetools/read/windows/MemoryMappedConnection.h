#ifndef SOURCE_TOOLS_READ_WINDOWS_MEMORY_MAPPED_CONNECTION_H
#define SOURCE_TOOLS_READ_WINDOWS_MEMORY_MAPPED_CONNECTION_H

#undef Realloc
#undef Free
#include <windows.h>

namespace sourcetools {
namespace detail {

class MemoryMappedConnection
{
public:

  MemoryMappedConnection(HANDLE handle, std::size_t size)
    : map_(NULL), size_(size)
  {
    handle_ = ::CreateFileMapping(handle, NULL, PAGE_READONLY, 0, 0, NULL);
    if (handle_ == NULL)
      return;

    map_ = (char*) ::MapViewOfFile(handle_, FILE_MAP_READ, 0, 0, size);
  }

  bool open() { return map_ != NULL; }
  operator char*() const { return map_; }

  ~MemoryMappedConnection()
  {
    if (handle_ != INVALID_HANDLE_VALUE)
      ::CloseHandle(handle_);
  }

private:
  char* map_;
  std::size_t size_;

  HANDLE handle_;
};

} // namespace detail
} // namespace sourcetools

#endif /* SOURCE_TOOLS_READ_WINDOWS_MEMORY_MAPPED_CONNECTION_H */
