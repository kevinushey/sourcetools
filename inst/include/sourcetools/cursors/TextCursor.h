#ifndef SOURCE_TOOLS_CURSOR_TEXTCURSOR_H
#define SOURCE_TOOLS_CURSOR_TEXTCURSOR_H

#include <string>

#include <sourcetools/Macros.h>
#include <sourcetools/collections/Position.h>

namespace sourcetools {
namespace cursors {

class TextCursor
{
public:

  TextCursor(const std::string& contents)
      : contents_(contents),
        offset_(0),
        position_(0, 0),
        size_(contents.size())
  {
  }

  char peek(std::size_t offset = 0)
  {
    std::size_t index = offset_ + offset;
    if (index >= size_)
      return '\0';
    return contents_[index];
  }

  void moveForward(std::size_t times = 1)
  {
    for (std::size_t i = 0; i < times; ++i) {
      if (peek() == '\n') {
        ++position_.row;
        position_.column = 0;
      } else {
        ++position_.column;
      }
      ++offset_;
    }
  }

  bool isValid() { return LIKELY(offset_ < size_); }

  std::size_t offset() const { return offset_; }

  const collections::Position& position() const { return position_; }
  std::size_t row() const { return position_.row; }
  std::size_t column() const { return position_.column; }

  std::string::const_iterator begin() const { return contents_.begin(); }
  std::string::const_iterator end() const { return contents_.end(); }

private:
  const std::string& contents_;
  std::size_t offset_;
  collections::Position position_;
  std::size_t size_;
};

} // namespace cursors
} // namespace sourcetools

#endif /* SOURCE_TOOLS_CURSOR_TEXTCURSOR_H */
