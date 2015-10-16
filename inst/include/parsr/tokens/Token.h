#ifndef PARSR_TOKENS_TOKENS_H
#define PARSR_TOKENS_TOKENS_H

#include <vector>
#include <string>
#include <cstring>

#include <parsr/collections/Position.h>
#include <parsr/cursors/TextCursor.h>

namespace parsr {
namespace tokens {

inline namespace types {

typedef unsigned char TokenType;

inline bool hasType(TokenType self, TokenType other)
{
  return (self & other) != 0;
}

/* Non-nestable types */
static const TokenType ERR         = 0;
static const TokenType SEMI        = 1;
static const TokenType COMMA       = 2;
static const TokenType SYMBOL      = 3;
static const TokenType COMMENT     = 4;
static const TokenType WHITESPACE  = 5;
static const TokenType STRING      = 6;
static const TokenType NUMBER      = 7;

/* Nestable types */
static const TokenType BRACKET   = 1 << 4;
static const TokenType LPAREN    = BRACKET | 1;
static const TokenType LBRACE    = BRACKET | 2;
static const TokenType LBRACKET  = BRACKET | 3;
static const TokenType LDBRACKET = BRACKET | 4;
static const TokenType RDBRACKET = BRACKET | 5;
static const TokenType RPAREN    = BRACKET | 6;
static const TokenType RBRACE    = BRACKET | 7;
static const TokenType RBRACKET  = BRACKET | 8;

static const TokenType OPERATOR               = 1 << 5;
static const TokenType OPERATOR_CAN_BE_UNARY  = OPERATOR | 1;

static const TokenType KEYWORD          = 1 << 6;
static const TokenType KEYWORD_FOR      = KEYWORD | 1;
static const TokenType KEYWORD_IN       = KEYWORD | 2;
static const TokenType KEYWORD_WHILE    = KEYWORD | 3;
static const TokenType KEYWORD_REPEAT   = KEYWORD | 4;
static const TokenType KEYWORD_BREAK    = KEYWORD | 5;
static const TokenType KEYWORD_NEXT     = KEYWORD | 6;
static const TokenType KEYWORD_IF       = KEYWORD | 7;
static const TokenType KEYWORD_ELSE     = KEYWORD | 8;
static const TokenType KEYWORD_FUNCTION = KEYWORD | 9;

inline TokenType symbolType(const char* string, std::size_t n)
{
  // TODO: Is this insanity really an optimization or am I just silly?
  if (n == 2) {
    if (!std::memcmp(string, "in", n)) return KEYWORD_IN;
    if (!std::memcmp(string, "if", n)) return KEYWORD_IF;
  } else if (n == 3) {
    if (!std::memcmp(string, "for", n)) return KEYWORD_FOR;
  } else if (n == 4) {
    if (!std::memcmp(string, "else", n)) return KEYWORD_ELSE;
    if (!std::memcmp(string, "next", n)) return KEYWORD_NEXT;
  } else if (n == 5) {
    if (!std::memcmp(string, "while", n)) return KEYWORD_WHILE;
    if (!std::memcmp(string, "break", n)) return KEYWORD_BREAK;
  } else if (n == 6) {
    if (!std::memcmp(string, "repeat", n)) return KEYWORD_REPEAT;
  } else if (n == 8) {
    if (!std::memcmp(string, "function", n)) return KEYWORD_FUNCTION;
  }

  return SYMBOL;
}

inline TokenType symbolType(const std::string& symbol)
{
  return symbolType(symbol.c_str(), symbol.length());
}

} // namespace types

class Token
{
private:
  typedef cursors::TextCursor TextCursor;
  typedef collections::Position Position;

public:

  explicit Token(TokenType type)
    : begin_(),
      end_(),
      position_(0, 0),
      type_(type)
  {
  }

  Token(const Position& position)
    : begin_(),
      end_(),
      position_(position),
      type_(ERR)
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

  const Position& position() const { return position_; }
  std::size_t row() const { return position_.row; }
  std::size_t column() const { return position_.column; }

  TokenType type() const { return type_; }

private:
  std::string::const_iterator begin_;
  std::string::const_iterator end_;

  Position position_;
  TokenType type_;
};

namespace utils {

inline bool isLeftBracket(const Token& token)
{
  const auto& type = token.type();
  return hasType(type, BRACKET) && (
      type == LBRACE ||
      type == LPAREN ||
      type == LBRACKET ||
      type == LDBRACKET
    );
}

inline bool isRightBracket(const Token& token)
{
  const auto& type = token.type();
  return hasType(type, BRACKET) && (
      type == RBRACE ||
      type == RPAREN ||
      type == RBRACKET ||
      type == RDBRACKET
    );
}

inline bool isSymbolic(const Token& token)
{
  const auto& type = token.type();
  return
    (type & SYMBOL) ||
    (type & STRING) ||
    (type & NUMBER);
}

inline bool isComplement(TokenType lhs, TokenType rhs)
{
  if ((BRACKET & lhs) == 0 || (BRACKET & rhs) == 0) return false;

  else if (lhs == LPAREN)    return rhs == RPAREN;
  else if (lhs == LBRACE)    return rhs == RBRACE;
  else if (lhs == LBRACKET)  return rhs == RBRACKET;
  else if (lhs == LDBRACKET) return rhs == RDBRACKET;

  else if (lhs == RPAREN)    return rhs == LPAREN;
  else if (lhs == RBRACE)    return rhs == LBRACE;
  else if (lhs == RBRACKET)  return rhs == LBRACKET;
  else if (lhs == RDBRACKET) return rhs == LDBRACKET;

  return ERR;
}

inline TokenType complement(TokenType type)
{
  if ((OPERATOR & type) == 0) return ERR;

  else if (type == LPAREN)    return RPAREN;
  else if (type == LBRACE)    return RBRACE;
  else if (type == LBRACKET)  return RBRACKET;
  else if (type == LDBRACKET) return RDBRACKET;

  else if (type == RPAREN)    return LPAREN;
  else if (type == RBRACE)    return LBRACE;
  else if (type == RBRACKET)  return LBRACKET;
  else if (type == RDBRACKET) return LDBRACKET;

  return ERR;
}

inline bool isKeyword(const Token& token)
{
  return (token.type() & KEYWORD) != 0;
}

inline bool isWhitespace(const Token& token)
{
  return token.type() == WHITESPACE;
}

inline bool isOperator(const Token& token)
{
  return (token.type() & OPERATOR) != 0;
}

inline bool isValidAsUnaryOp(const Token& token)
{
  return token.type() == OPERATOR_CAN_BE_UNARY;
}

} // namespace utils

} // namespace tokens

inline std::string toString(tokens::TokenType type)
{
  using namespace tokens;

  if (type == LPAREN)
    return "(";
  else if (type == RPAREN)
    return ")";
  else if (type == LBRACE)
    return "{";
  else if (type == RBRACE)
    return "}";
  else if (type == LBRACKET)
    return "[";
  else if (type == RBRACKET)
    return "]";
  else if (type == LDBRACKET)
    return "[[";
  else if (type == RDBRACKET)
    return "]]";
  else if (type == SEMI)
    return ";";
  else if (type == COMMA)
    return ",";
  else if (type == NUMBER)
    return "<number>";
  else if (type == WHITESPACE)
    return "<whitespace>";
  else if (type == COMMENT)
    return "<comment>";
  else if (type == OPERATOR_CAN_BE_UNARY)
    return "<operator:unary>";
  else if (type == OPERATOR)
    return "<operator>";
  else if (type == KEYWORD_BREAK)
    return "<keyword:break>";
  else if (type == KEYWORD_ELSE)
    return "<keyword:else>";
  else if (type == KEYWORD_FOR)
    return "<keyword:for>";
  else if (type == KEYWORD_FUNCTION)
    return "<keyword:function>";
  else if (type == KEYWORD_IF)
    return "<keyword:if>";
  else if (type == KEYWORD_IN)
    return "<keyword:in>";
  else if (type == KEYWORD_NEXT)
    return "<keyword:next>";
  else if (type == KEYWORD_REPEAT)
    return "<keyword:repeat>";
  else if (type == KEYWORD_WHILE)
    return "<keyword:while>";
  else if (type == SYMBOL)
    return "<symbol>";
  else if (type == STRING)
    return "<string>";
  else if (type == ERR)
    return "<err>";

  return "<unknown>";
}

} // namespace parsr

#endif /* PARSR_TOKENS_TOKENS_H */
