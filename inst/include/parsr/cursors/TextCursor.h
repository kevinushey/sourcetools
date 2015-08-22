#ifndef PARSR_CURSOR_TEXTCURSOR_H
#define PARSR_CURSOR_TEXTCURSOR_H

#include <string>

namespace parsr {
namespace cursors {

class TextCursor
{
public:
  TextCursor(const std::string& contents)
      : contents_(contents), offset_(0), row_(0), column_(0),
        size_(contents.size())
  {
  }

private:
  TextCursor(const std::string& contents, std::size_t offset, std::size_t row,
             std::size_t column, std::size_t size)
      : contents_(contents), offset_(offset), row_(row), column_(column),
        size_(size)
  {
  }

public:
  TextCursor clone()
  {
    return TextCursor(contents_, offset_, row_, column_, size_);
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
    while (times != 0) {
      if (peek() == '\n') {
        ++row_;
        column_ = 0;
      } else {
        ++column_;
      }

      --times;
      ++offset_;
    }
  }

  bool isValid() { return offset_ < size_; }

  std::size_t offset() const { return offset_; }
  std::size_t row() const { return row_; }
  std::size_t column() const { return column_; }

  std::string::const_iterator begin() const { return contents_.begin(); }
  std::string::const_iterator end() const { return contents_.end(); }

private:
  const std::string& contents_;
  std::size_t offset_;
  std::size_t row_;
  std::size_t column_;
  std::size_t size_;
};

} // namespace cursors
} // namespace parsr

#endif /* PARSR_CURSOR_TEXTCURSOR_H */
