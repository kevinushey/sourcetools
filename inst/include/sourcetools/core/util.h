#ifndef SOURCE_TOOLS_UTILS_H
#define SOURCE_TOOLS_UTILS_H

#include <string>
#include <memory>
#include <cctype>

namespace sourcetools {

template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

namespace detail {

class noncopyable
{
protected:
  noncopyable() = default;
  ~noncopyable() = default;
  noncopyable(const noncopyable&) = delete;
  noncopyable& operator=(const noncopyable&) = delete;
};

} // namespace detail
typedef detail::noncopyable noncopyable;

namespace utils {

inline bool isValidForStartOfRSymbol(char ch)
{
  return
    std::isalpha(ch) ||
    ch == '.' ||
    (unsigned char) ch >= 128;
}

inline bool isValidForRSymbol(char ch)
{
  return
    std::isalnum(ch) ||
    ch == '.' ||
    ch == '_' ||
    (unsigned char) ch >= 128;
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
