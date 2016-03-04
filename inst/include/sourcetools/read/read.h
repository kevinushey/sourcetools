#ifndef SOURCE_TOOLS_READ_READ_H
#define SOURCE_TOOLS_READ_READ_H

#include <sourcetools/read/MemoryMappedReader.h>

namespace sourcetools {

inline bool read(const std::string& absolutePath, std::string* pContent)
{
  return detail::MemoryMappedReader::read(absolutePath.c_str(), pContent);
}

}  // namespace sourcetools

#endif /* SOURCE_TOOLS_READ_READ_H */
