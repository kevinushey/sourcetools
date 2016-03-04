#ifndef SOURCE_TOOLS_READ_POSIX_FILE_CONNECTION_H
#define SOURCE_TOOLS_READ_POSIX_FILE_CONNECTION_H

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace sourcetools {
namespace detail {

class FileConnection
{
public:

  typedef int FileDescriptor;

  FileConnection(const char* path, int flags = O_RDONLY)
  {
    fd_ = ::open(path, flags);
  }

  ~FileConnection()
  {
    ::close(fd_);
  }

  bool open() { return fd_ != -1; }
  bool size(std::size_t* pSize)
  {
    struct stat info;
    if (!::fstate(fd_, &info))
      return false;

    *pSize = info.st_size;
    return true;
  }
  operator FileDescriptor() const { return fd_; }

private:
  FileDescriptor fd_;
};


} // namespace detail
} // namespace sourcetools

#endif /* SOURCE_TOOLS_READ_POSIX_FILE_CONNECTION_H */
