#ifndef PARSR_UTILS_H
#define PARSR_UTILS_H

namespace parsr {

inline bool isValidForStartOfRSymbol(char ch)
{
  return std::isalpha(ch) || ch == '.';
}

inline bool isValidForRSymbol(char ch)
{
  return std::isalnum(ch) || ch == '.' || ch == '_';
}

inline std::string escape(char ch)
{
  switch (ch) {
  case '\r':
    return "\\r";
  case '\n':
    return "\\n";
  case '\t':
    return "\\t";
  default:
    return std::string(1, ch);
  }
}

}  // namespace parsr

#endif  // PARSR_UTILS_H