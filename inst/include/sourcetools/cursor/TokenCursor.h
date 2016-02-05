#ifndef SOURCE_TOOLS_CURSORS_TOKEN_CURSOR_H
#define SOURCE_TOOLS_CURSORS_TOKEN_CURSOR_H

#include <algorithm>

#include <sourcetools/collection/Position.h>
#include <sourcetools/tokenization/Token.h>

namespace sourcetools {
namespace cursors {

class TokenCursor {

private:
  typedef collections::Position Position;
  typedef tokens::Token Token;

public:

  TokenCursor(const std::vector<Token>& tokens)
    : tokens_(tokens),
      offset_(0),
      n_(tokens.size()),
      noSuchToken_(tokens::ERR)
  {}

  bool moveToNextToken()
  {
    if (UNLIKELY(offset_ >= n_ - 1))
      return false;

    ++offset_;
    return true;
  }

  bool moveToNextSignificantToken()
  {
    if (!moveToNextToken())
      return false;

    if (!fwdOverWhitespaceAndComments())
      return false;

    return true;
  }

  bool moveToPreviousToken()
  {
    if (UNLIKELY(offset_ == 0))
      return false;

    --offset_;
    return true;
  }

  bool moveToPreviousSignificantToken()
  {
    if (!moveToPreviousToken())
      return false;

    if (!bwdOverWhitespace())
      return false;

    return true;
  }

  operator const Token&()
  {
    return tokens_[offset_];
  }

  const Token& peekFwd(std::size_t offset = 1)
  {
    std::size_t index = offset_ + offset;
    if (UNLIKELY(index >= n_))
      return noSuchToken_;

    return tokens_[index];
  }

  const Token& peekBwd(std::size_t offset = 1)
  {
    if (UNLIKELY(offset > offset_))
      return noSuchToken_;

    std::size_t index = offset_ - offset;
    return tokens_[index];
  }

  const Token& currentToken() const
  {
    return tokens_[offset_];
  }

  bool fwdOverWhitespace()
  {
    while (isType(tokens::WHITESPACE))
      if (!moveToNextToken())
        return false;
    return true;
  }

  bool bwdOverWhitespace()
  {
    while (isType(tokens::WHITESPACE))
      if (!moveToPreviousToken())
        return false;
      return true;
  }

  bool fwdOverComments()
  {
    while (isType(tokens::COMMENT))
      if (!moveToNextToken())
        return false;
    return true;
  }

  bool bwdOverComments()
  {
    while (isType(tokens::COMMENT))
      if (!moveToPreviousToken())
        return false;
    return true;
  }


  bool fwdOverWhitespaceAndComments()
  {
    while (isType(tokens::COMMENT) || isType(tokens::WHITESPACE))
      if (!moveToNextToken())
        return false;
    return true;
  }

  bool bwdOverWhitespaceAndComments()
  {
    while (isType(tokens::COMMENT) || isType(tokens::WHITESPACE))
      if (!moveToPreviousToken())
        return false;
    return true;
  }


  const Token& nextSignificantToken()
  {
    TokenCursor clone(*this);

    if (!clone.moveToNextToken())
      return noSuchToken_;

    if (!clone.fwdOverWhitespaceAndComments())
      return noSuchToken_;

    return clone;
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
      Token(position),
      [](const Token& lhs, const Token& rhs)
      {
        return lhs.position() < rhs.position();
      }
    );
  }

  friend std::ostream& operator<<(std::ostream& os, const TokenCursor& cursor)
  {
    return os << toString(cursor.currentToken());
  }

  tokens::TokenType type() const { return currentToken().type(); }
  bool isType(tokens::TokenType type) const { return currentToken().isType(type); }
  collections::Position position() const { return currentToken().position(); }
  std::size_t offset() const { return offset_; }
  std::size_t row() const { return currentToken().row(); }
  std::size_t column() const { return currentToken().column(); }


private:

  const std::vector<Token>& tokens_;
  std::size_t offset_;
  std::size_t n_;
  Token noSuchToken_;

};

} // namespace cursors

inline std::string toString(const cursors::TokenCursor& cursor)
{
  return toString(cursor.currentToken());
}

} // namespace sourcetools

#endif /* SOURCE_TOOLS_CURSORS_TOKEN_CURSOR_H */
