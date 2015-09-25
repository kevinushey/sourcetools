#ifndef PARSR_TOKENS_TOKENS_H
#define PARSR_TOKENS_TOKENS_H

#include <vector>
#include <string>

#include <parsr/collections/Position.h>
#include <parsr/cursors/TextCursor.h>

namespace parsr {
namespace tokens {

enum class TokenType : char
{
  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,
  LBRACKET,
  RBRACKET,
  LDBRACKET,
  RDBRACKET,
  SEMI,
  COMMA,
  NUMBER,
  STRING,
  WHITESPACE,
  COMMENT,
  SYMBOL,
  OPERATOR,
  ERR
};

class Token
{
private:
  typedef cursors::TextCursor TextCursor;

public:

  explicit Token(TokenType type)
    : begin_(),
      end_(),
      position_(0, 0),
      type_(type)
  {
  }

  Token(const collections::Position& position)
    : begin_(),
      end_(),
      position_(position),
      type_(TokenType::ERR)
  {}

  Token(const TextCursor& cursor, TokenType type, std::size_t tokenSize)
      : begin_(cursor.begin() + cursor.offset()),
        end_(cursor.begin() + cursor.offset() + tokenSize),
        position_(cursor.position()),
        type_(type)
  {
  }

  std::string::const_iterator begin() const { return begin_; }
  std::string::const_iterator end() const { return end_; }
  std::string contents() const { return std::string(begin_, end_); }

  const collections::Position& position() const { return position_; }
  std::size_t row() const { return position_.row; }
  std::size_t column() const { return position_.column; }

  TokenType type() const { return type_; }

private:
  std::string::const_iterator begin_;
  std::string::const_iterator end_;

  collections::Position position_;
  TokenType type_;
};

namespace utils {

inline bool isLeftBracket(const Token& token)
{
  switch (token.type()) {
  case TokenType::LBRACE:
  case TokenType::LPAREN:
  case TokenType::LBRACKET:
  case TokenType::LDBRACKET:
    return true;
  default:
    return false;
  }
}

inline bool isRightBracket(const Token& token)
{
  switch (token.type()) {
  case TokenType::RBRACE:
  case TokenType::RPAREN:
  case TokenType::RBRACKET:
  case TokenType::RDBRACKET:
    return true;
  default:
    return false;
  }
}

inline bool isSymbolic(const Token& token)
{
  switch (token.type()) {
  case TokenType::SYMBOL:
  case TokenType::STRING:
  case TokenType::NUMBER:
    return true;
  default:
    return false;
  }
}

inline bool isComplement(TokenType lhs, TokenType rhs)
{
  switch (lhs)
  {

  case TokenType::LPAREN:    return rhs == TokenType::RPAREN;
  case TokenType::LBRACE:    return rhs == TokenType::RBRACE;
  case TokenType::LBRACKET:  return rhs == TokenType::RBRACKET;
  case TokenType::LDBRACKET: return rhs == TokenType::RDBRACKET;

  case TokenType::RPAREN:    return rhs == TokenType::LPAREN;
  case TokenType::RBRACE:    return rhs == TokenType::LBRACE;
  case TokenType::RBRACKET:  return rhs == TokenType::LBRACKET;
  case TokenType::RDBRACKET: return rhs == TokenType::LDBRACKET;

  default:
    return false;
  }
}

inline TokenType complement(TokenType type)
{
  switch (type)
  {

  case TokenType::LPAREN:    return TokenType::RPAREN;
  case TokenType::LBRACE:    return TokenType::RBRACE;
  case TokenType::LBRACKET:  return TokenType::RBRACKET;
  case TokenType::LDBRACKET: return TokenType::RDBRACKET;

  case TokenType::RPAREN:    return TokenType::LPAREN;
  case TokenType::RBRACE:    return TokenType::LBRACE;
  case TokenType::RBRACKET:  return TokenType::LBRACKET;
  case TokenType::RDBRACKET: return TokenType::LDBRACKET;

  default:
    return TokenType::ERR;
  }
}

inline bool isUnaryOp(const Token& token)
{
  if (token.type() != TokenType::OPERATOR)
    return false;

  const std::string& contents = token.contents();
  return
    contents == "-" ||
    contents == "+" ||
    contents == "~" ||
    contents == "?" ||
    contents == "!";
}

} // namespace utils

} // namespace tokens

inline std::string toString(tokens::TokenType type)
{
  using namespace tokens;

  switch (type) {
  case TokenType::LPAREN:
    return "(";
  case TokenType::RPAREN:
    return ")";
  case TokenType::LBRACE:
    return "{";
  case TokenType::RBRACE:
    return "}";
  case TokenType::LBRACKET:
    return "[";
  case TokenType::RBRACKET:
    return "]";
  case TokenType::LDBRACKET:
    return "[[";
  case TokenType::RDBRACKET:
    return "]]";
  case TokenType::SEMI:
    return ";";
  case TokenType::COMMA:
    return ",";
  case TokenType::NUMBER:
    return "<number>";
  case TokenType::STRING:
    return "<string>";
  case TokenType::WHITESPACE:
    return "<whitespace>";
  case TokenType::COMMENT:
    return "<comment>";
  case TokenType::SYMBOL:
    return "<symbol>";
  case TokenType::OPERATOR:
    return "<operator>";
  case TokenType::ERR:
    return "<err>";
  }
}

} // namespace parsr

#endif /* PARSR_TOKENS_TOKENS_H */
