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

} // namespace tokens

inline std::string toString(tokens::TokenType type)
{
  using namespace tokens;

  switch (type) {
  case TokenType::LPAREN:
    return "LPAREN";
  case TokenType::RPAREN:
    return "RPAREN";
  case TokenType::LBRACE:
    return "LBRACE";
  case TokenType::RBRACE:
    return "RBRACE";
  case TokenType::LBRACKET:
    return "LBRACKET";
  case TokenType::RBRACKET:
    return "RBRACKET";
  case TokenType::LDBRACKET:
    return "LDBRACKET";
  case TokenType::RDBRACKET:
    return "RDBRACKET";
  case TokenType::SEMI:
    return "SEMI";
  case TokenType::COMMA:
    return "COMMA";
  case TokenType::NUMBER:
    return "NUMBER";
  case TokenType::STRING:
    return "STRING";
  case TokenType::WHITESPACE:
    return "WHITESPACE";
  case TokenType::COMMENT:
    return "COMMENT";
  case TokenType::SYMBOL:
    return "SYMBOL";
  case TokenType::OPERATOR:
    return "OPERATOR";
  case TokenType::ERR:
    return "ERR";
  }
}

} // namespace parsr

#endif /* PARSR_TOKENS_TOKENS_H */
