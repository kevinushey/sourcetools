#ifndef SOURCETOOLS_CURSOR_TEXT_CURSOR_H
#define SOURCETOOLS_CURSOR_TEXT_CURSOR_H

#include <string>

#include <sourcetools/core/macros.h>
#include <sourcetools/collection/Position.h>

namespace sourcetools {
namespace cursors {

class TextCursor
{
public:

  TextCursor(const char* text, index_type n)
      : text_(text),
        n_(n),
        offset_(0),
        position_(0, 0)
  {
  }

  char peek(index_type offset = 0)
  {
    index_type index = offset_ + offset;
    if (UNLIKELY(index >= n_))
      return '\0';
    return text_[index];
  }

  void advance(index_type times = 1)
  {
    for (index_type i = 0; i < times; ++i) {
      if (peek() == '\n') {
        ++position_.row;
        position_.column = 0;
      } else {
        ++position_.column;
      }
      ++offset_;
    }
  }

  operator const char*() const { return text_ + offset_; }

  index_type offset() const { return offset_; }

  const collections::Position& position() const { return position_; }
  index_type row() const { return position_.row; }
  index_type column() const { return position_.column; }

  const char* begin() const { return text_; }
  const char* end() const { return text_ + n_; }

private:
  const char* text_;
  index_type n_;
  index_type offset_;
  collections::Position position_;
};

} // namespace cursors
} // namespace sourcetools

#endif /* SOURCETOOLS_CURSOR_TEXT_CURSOR_H */
