#ifndef PARSR_TOKENS_TOKENS_H
#define PARSR_TOKENS_TOKENS_H

#include <cstring>

#include <vector>
#include <string>
#include <map>
#include <sstream>

#include <parsr/collections/Position.h>
#include <parsr/cursors/TextCursor.h>

namespace parsr {
namespace tokens {

typedef uint32_t TokenType;

inline namespace types {

// Simple, non-nestable types.
#define PARSR_REGISTER_SIMPLE_TYPE(__NAME__, __TYPE__)         \
  static const TokenType __NAME__ = __TYPE__

PARSR_REGISTER_SIMPLE_TYPE(ERR,        (1 <<  8));
PARSR_REGISTER_SIMPLE_TYPE(SEMI,       (1 <<  9));
PARSR_REGISTER_SIMPLE_TYPE(COMMA,      (1 << 10));
PARSR_REGISTER_SIMPLE_TYPE(SYMBOL,     (1 << 11));
PARSR_REGISTER_SIMPLE_TYPE(COMMENT,    (1 << 12));
PARSR_REGISTER_SIMPLE_TYPE(WHITESPACE, (1 << 13));
PARSR_REGISTER_SIMPLE_TYPE(STRING,     (1 << 14));
PARSR_REGISTER_SIMPLE_TYPE(NUMBER,     (1 << 15));

/* Brackets */
#define PARSR_BRACKET_BIT        (1 << 16)
#define PARSR_BRACKET_RIGHT_BIT  (1 << 3)
#define PARSR_BRACKET_LEFT_BIT   (1 << 2)
#define PARSR_BRACKET_MASK       PARSR_BRACKET_BIT
#define PARSR_BRACKET_LEFT_MASK  (PARSR_BRACKET_BIT | PARSR_BRACKET_LEFT_BIT)
#define PARSR_BRACKET_RIGHT_MASK (PARSR_BRACKET_BIT | PARSR_BRACKET_RIGHT_BIT)

#define PARSR_REGISTER_BRACKET(__NAME__, __SIDE__, __INDEX__)  \
  static const TokenType __NAME__ =                            \
    PARSR_BRACKET_BIT | __SIDE__ | __INDEX__

PARSR_REGISTER_BRACKET(LPAREN,    PARSR_BRACKET_LEFT_BIT, 0);  // 00010100
PARSR_REGISTER_BRACKET(LBRACE,    PARSR_BRACKET_LEFT_BIT, 1);  // 00010101
PARSR_REGISTER_BRACKET(LBRACKET,  PARSR_BRACKET_LEFT_BIT, 2);  // 00010110
PARSR_REGISTER_BRACKET(LDBRACKET, PARSR_BRACKET_LEFT_BIT, 3);  // 00010111

PARSR_REGISTER_BRACKET(RPAREN,    PARSR_BRACKET_RIGHT_BIT, 0); // 00011000
PARSR_REGISTER_BRACKET(RBRACE,    PARSR_BRACKET_RIGHT_BIT, 1); // 00011001
PARSR_REGISTER_BRACKET(RBRACKET,  PARSR_BRACKET_RIGHT_BIT, 2); // 00011010
PARSR_REGISTER_BRACKET(RDBRACKET, PARSR_BRACKET_RIGHT_BIT, 3); // 00011011

/* Operators */
#define PARSR_OPERATOR_BIT        (1 << 17)
#define PARSR_OPERATOR_UNARY_BIT  (1 << 5)
#define PARSR_OPERATOR_MASK       PARSR_OPERATOR_BIT
#define PARSR_OPERATOR_UNARY_MASK (PARSR_OPERATOR_MASK | PARSR_OPERATOR_UNARY_BIT)

#define PARSR_REGISTER_OPERATOR(__NAME__, __STRING__, __MASKS__) \
                                                                 \
  static const TokenType OPERATOR_ ## __NAME__ =                 \
    PARSR_OPERATOR_BIT | __MASKS__;                              \
                                                                 \
  static const char* const                                       \
    OPERATOR_ ## __NAME__ ## _STRING = __STRING__

#define PARSR_REGISTER_UNARY_OPERATOR(__NAME__, __STRING__, __INDEX__)    \
  PARSR_REGISTER_OPERATOR(__NAME__, __STRING__, PARSR_OPERATOR_UNARY_BIT | __INDEX__)

// See ?"Syntax" for details on R's operators.
// Note: All operators registered work in a binary context, but only
// some will work as unary operators. (Occurring to the left of the token).
//
// In other words, -1 is parsed as `-`(1).
//
// Note that although brackets are operators we tokenize them separately,
// since we need to later check for their paired complement.
PARSR_REGISTER_UNARY_OPERATOR(PLUS,          "+",    0);
PARSR_REGISTER_UNARY_OPERATOR(MINUS,         "-",    1);
PARSR_REGISTER_UNARY_OPERATOR(HELP,          "?",    2);
PARSR_REGISTER_UNARY_OPERATOR(NEGATION,      "!",    3);
PARSR_REGISTER_UNARY_OPERATOR(FORMULA,       "~",    4);

PARSR_REGISTER_OPERATOR(NAMESPACE_EXPORTS,   "::",   5);
PARSR_REGISTER_OPERATOR(NAMESPACE_ALL,       ":::",  6);
PARSR_REGISTER_OPERATOR(DOLLAR,              "$",    7);
PARSR_REGISTER_OPERATOR(AT,                  "@",    8);
PARSR_REGISTER_OPERATOR(HAT,                 "^",    9);
PARSR_REGISTER_OPERATOR(EXPONENTATION_STARS, "**",  10);
PARSR_REGISTER_OPERATOR(SEQUENCE,            ":",   11);
PARSR_REGISTER_OPERATOR(MULTIPLY,            "*",   12);
PARSR_REGISTER_OPERATOR(DIVIDE,              "/",   13);
PARSR_REGISTER_OPERATOR(LESS,                "<",   14);
PARSR_REGISTER_OPERATOR(LESS_OR_EQUAL,       "<=",  15);
PARSR_REGISTER_OPERATOR(GREATER,             ">",   16);
PARSR_REGISTER_OPERATOR(GREATER_OR_EQUAL,    ">=",  17);
PARSR_REGISTER_OPERATOR(EQUAL,               "==",  18);
PARSR_REGISTER_OPERATOR(NOT_EQUAL,           "!=",  19);
PARSR_REGISTER_OPERATOR(AND_VECTOR,          "&",   20);
PARSR_REGISTER_OPERATOR(AND_SCALAR,          "&&",  21);
PARSR_REGISTER_OPERATOR(OR_VECTOR,           "|",   22);
PARSR_REGISTER_OPERATOR(OR_SCALAR,           "||",  23);
PARSR_REGISTER_OPERATOR(ASSIGN_LEFT,         "<-",  24);
PARSR_REGISTER_OPERATOR(ASSIGN_LEFT_PARENT,  "<<-", 25);
PARSR_REGISTER_OPERATOR(ASSIGN_RIGHT,        "->",  26);
PARSR_REGISTER_OPERATOR(ASSIGN_RIGHT_PARENT, "->>", 27);
PARSR_REGISTER_OPERATOR(ASSIGN_LEFT_EQUALS,  "=",   28);
PARSR_REGISTER_OPERATOR(ASSIGN_LEFT_COLON,   ":=",  29);
PARSR_REGISTER_OPERATOR(USER,                "%%",  30);

/* Keywords and symbols */
#define PARSR_KEYWORD_BIT               (1 << 18)
#define PARSR_KEYWORD_CONTROL_FLOW_BIT  (1 << 5)
#define PARSR_KEYWORD_MASK              PARSR_KEYWORD_BIT
#define PARSR_KEYWORD_CONTROL_FLOW_MASK (PARSR_KEYWORD_MASK | PARSR_KEYWORD_CONTROL_FLOW_BIT)

#define PARSR_REGISTER_KEYWORD(__NAME__, __MASKS__)            \
  static const TokenType KEYWORD_ ## __NAME__ =                \
    __MASKS__ | PARSR_KEYWORD_MASK

#define PARSR_REGISTER_CONTROL_FLOW_KEYWORD(__NAME__, __MASKS__) \
  PARSR_REGISTER_KEYWORD(__NAME__, __MASKS__ | PARSR_KEYWORD_CONTROL_FLOW_MASK)

// See '?Reserved' for a list of reversed R symbols.
PARSR_REGISTER_CONTROL_FLOW_KEYWORD(IF,       1);
PARSR_REGISTER_CONTROL_FLOW_KEYWORD(ELSE,     2);
PARSR_REGISTER_CONTROL_FLOW_KEYWORD(REPEAT,   3);
PARSR_REGISTER_CONTROL_FLOW_KEYWORD(WHILE,    4);
PARSR_REGISTER_CONTROL_FLOW_KEYWORD(FUNCTION, 5);
PARSR_REGISTER_CONTROL_FLOW_KEYWORD(FOR,      6);
PARSR_REGISTER_CONTROL_FLOW_KEYWORD(IN,       7);
PARSR_REGISTER_CONTROL_FLOW_KEYWORD(NEXT,     8);
PARSR_REGISTER_CONTROL_FLOW_KEYWORD(BREAK,    9);

PARSR_REGISTER_KEYWORD(TRUE,                 10);
PARSR_REGISTER_KEYWORD(FALSE,                11);
PARSR_REGISTER_KEYWORD(NULL,                 12);
PARSR_REGISTER_KEYWORD(Inf,                  13);
PARSR_REGISTER_KEYWORD(NaN,                  14);
PARSR_REGISTER_KEYWORD(NA,                   15);
PARSR_REGISTER_KEYWORD(NA_integer_,          16);
PARSR_REGISTER_KEYWORD(NA_real_,             17);
PARSR_REGISTER_KEYWORD(NA_complex_,          18);
PARSR_REGISTER_KEYWORD(NA_character_,        19);

inline TokenType symbolType(const char* string, std::size_t n)
{
  // TODO: Is this insanity really an optimization or am I just silly?
  if (n < 2 || n > 13) {
    return SYMBOL;
  } else if (n == 2) {
    if (!std::memcmp(string, "in", n)) return KEYWORD_IN;
    if (!std::memcmp(string, "if", n)) return KEYWORD_IF;
    if (!std::memcmp(string, "NA", n)) return KEYWORD_NA;
  } else if (n == 3) {
    if (!std::memcmp(string, "for", n)) return KEYWORD_FOR;
    if (!std::memcmp(string, "Inf", n)) return KEYWORD_Inf;
    if (!std::memcmp(string, "NaN", n)) return KEYWORD_NaN;
  } else if (n == 4) {
    if (!std::memcmp(string, "else", n)) return KEYWORD_ELSE;
    if (!std::memcmp(string, "next", n)) return KEYWORD_NEXT;
    if (!std::memcmp(string, "TRUE", n)) return KEYWORD_TRUE;
    if (!std::memcmp(string, "NULL", n)) return KEYWORD_NULL;
  } else if (n == 5) {
    if (!std::memcmp(string, "while", n)) return KEYWORD_WHILE;
    if (!std::memcmp(string, "break", n)) return KEYWORD_BREAK;
    if (!std::memcmp(string, "FALSE", n)) return KEYWORD_FALSE;
  } else if (n == 6) {
    if (!std::memcmp(string, "repeat", n)) return KEYWORD_REPEAT;
  } else if (n == 8) {
    if (!std::memcmp(string, "function", n)) return KEYWORD_FUNCTION;
    if (!std::memcmp(string, "NA_real_", n)) return KEYWORD_NA_real_;
  } else if (n == 11) {
    if (!std::memcmp(string, "NA_integer_", n)) return KEYWORD_NA_integer_;
    if (!std::memcmp(string, "NA_complex_", n)) return KEYWORD_NA_complex_;
  } else if (n == 13) {
    if (!std::memcmp(string, "NA_character_", n)) return KEYWORD_NA_character_;
  }

  return SYMBOL;
}

inline TokenType symbolType(const std::string& symbol)
{
  return symbolType(symbol.c_str(), symbol.length());
}

} // end namespace types

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
  bool isType(TokenType type) const { return PARSR_CHECK_MASK(type_, type); }

private:
  std::string::const_iterator begin_;
  std::string::const_iterator end_;

  Position position_;
  TokenType type_;
};

inline namespace utils {

inline bool isBracket(const Token& token)
{
  return PARSR_CHECK_MASK(token.type(), PARSR_BRACKET_MASK);
}

inline bool isLeftBracket(const Token& token)
{
  return PARSR_CHECK_MASK(token.type(), PARSR_BRACKET_LEFT_MASK);
}

inline bool isRightBracket(const Token& token)
{
  return PARSR_CHECK_MASK(token.type(), PARSR_BRACKET_RIGHT_MASK);
}

inline bool isComplement(TokenType lhs, TokenType rhs)
{
  static const TokenType mask =
    PARSR_BRACKET_BIT | PARSR_BRACKET_LEFT_BIT | PARSR_BRACKET_RIGHT_BIT;

  if (PARSR_CHECK_MASK((lhs | rhs), mask))
    return PARSR_LOWER_BITS(lhs, 2) == PARSR_LOWER_BITS(rhs, 2);

  return false;
}

inline TokenType complement(TokenType type)
{
  static const TokenType mask =
    PARSR_BRACKET_LEFT_BIT | PARSR_BRACKET_RIGHT_BIT;
  return type ^ mask;
}

inline bool isKeyword(const Token& token)
{
  return PARSR_CHECK_MASK(token.type(), PARSR_KEYWORD_MASK);
}

inline bool isControlFlowKeyword(const Token& token)
{
  return PARSR_CHECK_MASK(token.type(), PARSR_KEYWORD_CONTROL_FLOW_MASK);
}

inline bool isOperator(const Token& token)
{
  return PARSR_CHECK_MASK(token.type(), PARSR_OPERATOR_MASK);
}

inline bool isUnaryOperator(const Token& token)
{
  return PARSR_CHECK_MASK(token.type(), PARSR_OPERATOR_UNARY_MASK);
}

inline bool isWhitespace(const Token& token)
{
  return token.type() == WHITESPACE;
}

} // namespace utils
} // namespace tokens

inline std::string toString(tokens::TokenType type)
{
  using namespace tokens;

       if (type == ERR)        return "<err>";
  else if (type == SEMI)       return "<semi>";
  else if (type == COMMA)      return "<comma>";
  else if (type == SYMBOL)     return "<symbol>";
  else if (type == COMMENT)    return "<comment>";
  else if (type == WHITESPACE) return "<whitespace>";
  else if (type == STRING)     return "<string>";
  else if (type == NUMBER)     return "<number>";

  else if (PARSR_CHECK_MASK(type, PARSR_BRACKET_MASK))
    return "<bracket>";
  else if (PARSR_CHECK_MASK(type, PARSR_KEYWORD_MASK))
    return "<keyword>";
  else if (PARSR_CHECK_MASK(type, PARSR_OPERATOR_MASK))
    return "<operator>";

  return "<unknown>";
}

inline std::string toString(const tokens::Token& token)
{
  std::stringstream ss;
  ss << "[" << token.row() << ":" << token.column() << "]: " << token.contents();
  return ss.str();
}

inline std::ostream& operator<<(std::ostream& os, const tokens::Token& token)
{
  return os << toString(token);
}

inline std::ostream& operator<<(std::ostream& os, const std::vector<tokens::Token>& tokens)
{
  for (auto& token : tokens)
    os << token << std::endl;
  return os;
}

} // namespace parsr

#endif /* PARSR_TOKENS_TOKENS_H */
