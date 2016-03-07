#ifndef SOURCE_TOOLS_UTILS_H
#define SOURCE_TOOLS_UTILS_H

#include <string>
#include <memory>
#include <cctype>

namespace sourcetools {
namespace detail {

class noncopyable
{
protected:
  noncopyable() {}
  ~noncopyable() {}

private:
  noncopyable(const noncopyable&);
  noncopyable& operator=(const noncopyable&);
};

} // namespace detail
typedef detail::noncopyable noncopyable;

namespace utils {

inline bool isWhitespace(char ch)
{
  return
    ch == ' ' ||
    ch == '\f' ||
    ch == '\r' ||
    ch == '\n' ||
    ch == '\t' ||
    ch == '\v';
}

inline bool isDigit(char ch)
{
  return
    (ch >= '0' && ch <= '9');
}

inline bool isAlphabetic(char ch)
{
  return
    (ch >= 'a' && ch <= 'z') ||
    (ch >= 'A' && ch <= 'Z');
}

inline bool isAlphaNumeric(char ch)
{
  return
    (ch >= 'a' && ch <= 'z') ||
    (ch >= 'A' && ch <= 'Z') ||
    (ch >= '0' && ch <= '9');
}

inline bool isValidForStartOfRSymbol(char ch)
{
  return
    isAlphabetic(ch) ||
    ch == '.' ||
    ch < 0;
}

inline bool isValidForRSymbol(char ch)
{
  return
    isAlphaNumeric(ch) ||
    ch == '.' ||
    ch == '_' ||
    ch < 0;
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

} // namespace utils
} // namespace sourcetools

#endif  // SOURCE_TOOLS_UTILS_H
