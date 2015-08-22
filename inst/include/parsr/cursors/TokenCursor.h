#ifndef PARSR_CURSORS_TOKEN_CURSOR_H
#define PARSR_CURSORS_TOKEN_CURSOR_H

#include <algorithm>

#include <parsr/tokens/Token.h>

namespace parsr {
namespace cursors {

template <typename TokenType>
class TokenCursor {

public:

  TokenCursor(const std::vector<TokenType>& tokens)
    : token_(tokens),
      offset_(0),
      n_(tokens.size())
  {}

  TokenCursor(const TokenCursor& other)
    : tokens_(other.tokens),
      offset_(other.offset_),
      n_(other.n_)
  {}

  bool moveToNextToken()
  {
    if (UNLIKELY(offset_ >= n_))
      return false;

    ++offset_;
    return true;
  }

  bool moveToPreviousToken()
  {
    if (UNLIKELY(offset_ == 0))
      return false;

    --offset_;
    return true;
  }

  operator const TokenType& const()
  {
    return tokens_[offset_];
  }

  const TokenType& peekFwd(std::size_t offset = 1)
  {
    std::size_t index = offset_ + offset;
    if (UNLIKELY(index >= n_))
      return noSuchToken_;

    return tokens_[index];
  }

  const TokenType& peekBwd(std::size_t offset = 1)
  {
    if (UNLIKELY(offset > offset_))
      return noSuchToken_;

    std::size_t index = offset_ - offset;
    return tokens_[index];
  }

  const TokenType& currentToken()
  {
    return tokens_[offset_];
  }

  bool moveToPosition(std::size_t row, std::size_t column)
  {
    return moveToPosition(Position(row, column));
  }

  bool moveToPosition(const Position& position)
  {
    return std::binary_search(
      tokens_.begin(),
      tokens_.end(),
      position
    );
  }

private:

  operator const Position&() const {
    return tokens_[offset_].position();
  }

  const std::vector<TokenType>& tokens_;
  std::size_t offset_;

};

} // namespace cursors
} // namespace parsr

#endif /* PARSR_CURSORS_TOKEN_CURSOR_H */
