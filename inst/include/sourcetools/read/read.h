#ifndef SOURCETOOLS__READ__READ_H
#define SOURCETOOLS__READ__READ_H

#include <vector>
#include <string>

#include <sourcetools/read/MemoryMappedReader.h>

namespace sourcetools {

inline bool read(const std::string& absolutePath, std::string* pContent)
{
  return detail::MemoryMappedReader::read(absolutePath.c_str(), pContent);
}

inline bool read_lines(const std::string& absolutePath,
                       std::vector<std::string>* pLines)
{
  return detail::MemoryMappedReader::read_lines(absolutePath.c_str(), pLines);
}

}  // namespace sourcetools

#endif /* SOURCETOOLS__READ__READ_H */
