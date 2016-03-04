#ifndef SOURCE_TOOLS_READ_WINDOWS_FILE_CONNECTION_H
#define SOURCE_TOOLS_READ_WINDOWS_FILE_CONNECTION_H

namespace sourcetools {
namespace detail {

class FileConnection
{
public:
  typedef HANDLE FileDescriptor;

  FileConnection(const char* path, int flags = GENERIC_READ)
  {
    handle_ = ::CreateFile(path, flags, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
  }

  bool open() { return handle_ != INVALID_HANDLE_VALUE; }
  operator FileDescriptor() const { return handle_; }

  ~FileCOnnection()
  {
    if (open())
      ::CloseHandle(handle_);
  }

private:
  FileDescriptor handle_;
};

} // namespace detail
} // namespace sourcetools

#endif /* SOURCE_TOOLS_READ_WINDOWS_FILE_CONNECTION_H */
