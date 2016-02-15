#ifndef SOURCE_TOOLS_TOKENS_TOKENS_H
#define SOURCE_TOOLS_TOKENS_TOKENS_H

#include <cstring>

#include <vector>
#include <string>
#include <map>
#include <sstream>

#include <sourcetools/collection/Position.h>
#include <sourcetools/cursor/TextCursor.h>

namespace sourcetools {
namespace tokens {

typedef uint32_t TokenType;

inline namespace types {

// Simple, non-nestable types.
#define SOURCE_TOOLS_REGISTER_SIMPLE_TYPE(__NAME__, __TYPE__)         \
  static const TokenType __NAME__ = __TYPE__

SOURCE_TOOLS_REGISTER_SIMPLE_TYPE(ERR,        (1 << 31));
SOURCE_TOOLS_REGISTER_SIMPLE_TYPE(END,        (1 << 30));
SOURCE_TOOLS_REGISTER_SIMPLE_TYPE(EMPTY,      (1 << 29));
SOURCE_TOOLS_REGISTER_SIMPLE_TYPE(SEMI,       (1 << 28));
SOURCE_TOOLS_REGISTER_SIMPLE_TYPE(COMMA,      (1 << 27));
SOURCE_TOOLS_REGISTER_SIMPLE_TYPE(SYMBOL,     (1 << 26));
SOURCE_TOOLS_REGISTER_SIMPLE_TYPE(COMMENT,    (1 << 25));
SOURCE_TOOLS_REGISTER_SIMPLE_TYPE(WHITESPACE, (1 << 24));
SOURCE_TOOLS_REGISTER_SIMPLE_TYPE(STRING,     (1 << 23));
SOURCE_TOOLS_REGISTER_SIMPLE_TYPE(NUMBER,     (1 << 22));

/* Brackets */
#define SOURCE_TOOLS_BRACKET_BIT        (1 << 21)
#define SOURCE_TOOLS_BRACKET_RIGHT_BIT  (1 << 5)
#define SOURCE_TOOLS_BRACKET_LEFT_BIT   (1 << 4)
#define SOURCE_TOOLS_BRACKET_MASK       SOURCE_TOOLS_BRACKET_BIT
#define SOURCE_TOOLS_BRACKET_LEFT_MASK  (SOURCE_TOOLS_BRACKET_BIT | SOURCE_TOOLS_BRACKET_LEFT_BIT)
#define SOURCE_TOOLS_BRACKET_RIGHT_MASK (SOURCE_TOOLS_BRACKET_BIT | SOURCE_TOOLS_BRACKET_RIGHT_BIT)

#define SOURCE_TOOLS_REGISTER_BRACKET(__NAME__, __SIDE__, __INDEX__)  \
  static const TokenType __NAME__ =                            \
    SOURCE_TOOLS_BRACKET_BIT | __SIDE__ | __INDEX__

SOURCE_TOOLS_REGISTER_BRACKET(LPAREN,    SOURCE_TOOLS_BRACKET_LEFT_BIT, (1 << 0));
SOURCE_TOOLS_REGISTER_BRACKET(LBRACE,    SOURCE_TOOLS_BRACKET_LEFT_BIT, (1 << 1));
SOURCE_TOOLS_REGISTER_BRACKET(LBRACKET,  SOURCE_TOOLS_BRACKET_LEFT_BIT, (1 << 2));
SOURCE_TOOLS_REGISTER_BRACKET(LDBRACKET, SOURCE_TOOLS_BRACKET_LEFT_BIT, (1 << 3));

SOURCE_TOOLS_REGISTER_BRACKET(RPAREN,    SOURCE_TOOLS_BRACKET_RIGHT_BIT, (1 << 0));
SOURCE_TOOLS_REGISTER_BRACKET(RBRACE,    SOURCE_TOOLS_BRACKET_RIGHT_BIT, (1 << 1));
SOURCE_TOOLS_REGISTER_BRACKET(RBRACKET,  SOURCE_TOOLS_BRACKET_RIGHT_BIT, (1 << 2));
SOURCE_TOOLS_REGISTER_BRACKET(RDBRACKET, SOURCE_TOOLS_BRACKET_RIGHT_BIT, (1 << 3));

/* Operators */
#define SOURCE_TOOLS_OPERATOR_BIT         (1 << 20)
#define SOURCE_TOOLS_OPERATOR_UNARY_BIT   (1 << 6)
#define SOURCE_TOOLS_OPERATOR_MASK        (SOURCE_TOOLS_OPERATOR_BIT)
#define SOURCE_TOOLS_OPERATOR_UNARY_MASK  (SOURCE_TOOLS_OPERATOR_MASK | SOURCE_TOOLS_OPERATOR_UNARY_BIT)

#define SOURCE_TOOLS_REGISTER_OPERATOR(__NAME__, __STRING__, __MASKS__) \
                                                                        \
  static const TokenType OPERATOR_ ## __NAME__ =                        \
    SOURCE_TOOLS_OPERATOR_BIT | __MASKS__;                              \
                                                                        \
  static const char* const                                              \
    OPERATOR_ ## __NAME__ ## _STRING = __STRING__

#define SOURCE_TOOLS_REGISTER_UNARY_OPERATOR(__NAME__, __STRING__, __INDEX__)    \
  SOURCE_TOOLS_REGISTER_OPERATOR(__NAME__, __STRING__, SOURCE_TOOLS_OPERATOR_UNARY_BIT | __INDEX__)

// See ?"Syntax" for details on R's operators.
// Note: All operators registered work in a binary context, but only
// some will work as unary operators. (Occurring to the left of the token).
//
// In other words, -1 is parsed as `-`(1).
//
// Note that although brackets are operators we tokenize them separately,
// since we need to later check for their paired complement.
SOURCE_TOOLS_REGISTER_UNARY_OPERATOR(PLUS,          "+",    0);
SOURCE_TOOLS_REGISTER_UNARY_OPERATOR(MINUS,         "-",    1);
SOURCE_TOOLS_REGISTER_UNARY_OPERATOR(HELP,          "?",    2);
SOURCE_TOOLS_REGISTER_UNARY_OPERATOR(NEGATION,      "!",    3);
SOURCE_TOOLS_REGISTER_UNARY_OPERATOR(FORMULA,       "~",    4);

SOURCE_TOOLS_REGISTER_OPERATOR(NAMESPACE_EXPORTS,   "::",   5);
SOURCE_TOOLS_REGISTER_OPERATOR(NAMESPACE_ALL,       ":::",  6);
SOURCE_TOOLS_REGISTER_OPERATOR(DOLLAR,              "$",    7);
SOURCE_TOOLS_REGISTER_OPERATOR(AT,                  "@",    8);
SOURCE_TOOLS_REGISTER_OPERATOR(HAT,                 "^",    9);
SOURCE_TOOLS_REGISTER_OPERATOR(EXPONENTATION_STARS, "**",  10);
SOURCE_TOOLS_REGISTER_OPERATOR(SEQUENCE,            ":",   11);
SOURCE_TOOLS_REGISTER_OPERATOR(MULTIPLY,            "*",   12);
SOURCE_TOOLS_REGISTER_OPERATOR(DIVIDE,              "/",   13);
SOURCE_TOOLS_REGISTER_OPERATOR(LESS,                "<",   14);
SOURCE_TOOLS_REGISTER_OPERATOR(LESS_OR_EQUAL,       "<=",  15);
SOURCE_TOOLS_REGISTER_OPERATOR(GREATER,             ">",   16);
SOURCE_TOOLS_REGISTER_OPERATOR(GREATER_OR_EQUAL,    ">=",  17);
SOURCE_TOOLS_REGISTER_OPERATOR(EQUAL,               "==",  18);
SOURCE_TOOLS_REGISTER_OPERATOR(NOT_EQUAL,           "!=",  19);
SOURCE_TOOLS_REGISTER_OPERATOR(AND_VECTOR,          "&",   20);
SOURCE_TOOLS_REGISTER_OPERATOR(AND_SCALAR,          "&&",  21);
SOURCE_TOOLS_REGISTER_OPERATOR(OR_VECTOR,           "|",   22);
SOURCE_TOOLS_REGISTER_OPERATOR(OR_SCALAR,           "||",  23);
SOURCE_TOOLS_REGISTER_OPERATOR(ASSIGN_LEFT,         "<-",  24);
SOURCE_TOOLS_REGISTER_OPERATOR(ASSIGN_LEFT_PARENT,  "<<-", 25);
SOURCE_TOOLS_REGISTER_OPERATOR(ASSIGN_RIGHT,        "->",  26);
SOURCE_TOOLS_REGISTER_OPERATOR(ASSIGN_RIGHT_PARENT, "->>", 27);
SOURCE_TOOLS_REGISTER_OPERATOR(ASSIGN_LEFT_EQUALS,  "=",   28);
SOURCE_TOOLS_REGISTER_OPERATOR(ASSIGN_LEFT_COLON,   ":=",  29);
SOURCE_TOOLS_REGISTER_OPERATOR(USER,                "%%",  30);

/* Keywords and symbols */
#define SOURCE_TOOLS_KEYWORD_BIT               (1 << 19)
#define SOURCE_TOOLS_KEYWORD_CONTROL_FLOW_BIT  (1 << 7)
#define SOURCE_TOOLS_KEYWORD_MASK              SOURCE_TOOLS_KEYWORD_BIT
#define SOURCE_TOOLS_KEYWORD_CONTROL_FLOW_MASK (SOURCE_TOOLS_KEYWORD_MASK | SOURCE_TOOLS_KEYWORD_CONTROL_FLOW_BIT)

#define SOURCE_TOOLS_REGISTER_KEYWORD(__NAME__, __MASKS__)            \
  static const TokenType KEYWORD_ ## __NAME__ =                \
    __MASKS__ | SOURCE_TOOLS_KEYWORD_MASK

#define SOURCE_TOOLS_REGISTER_CONTROL_FLOW_KEYWORD(__NAME__, __MASKS__) \
  SOURCE_TOOLS_REGISTER_KEYWORD(__NAME__, __MASKS__ | SOURCE_TOOLS_KEYWORD_CONTROL_FLOW_MASK)

// See '?Reserved' for a list of reversed R symbols.
SOURCE_TOOLS_REGISTER_CONTROL_FLOW_KEYWORD(IF,       1);
SOURCE_TOOLS_REGISTER_CONTROL_FLOW_KEYWORD(FOR,      2);
SOURCE_TOOLS_REGISTER_CONTROL_FLOW_KEYWORD(WHILE,    3);
SOURCE_TOOLS_REGISTER_CONTROL_FLOW_KEYWORD(REPEAT,   4);
SOURCE_TOOLS_REGISTER_CONTROL_FLOW_KEYWORD(FUNCTION, 5);

SOURCE_TOOLS_REGISTER_KEYWORD(ELSE,                  6);
SOURCE_TOOLS_REGISTER_KEYWORD(IN,                    7);
SOURCE_TOOLS_REGISTER_KEYWORD(NEXT,                  8);
SOURCE_TOOLS_REGISTER_KEYWORD(BREAK,                 9);
SOURCE_TOOLS_REGISTER_KEYWORD(TRUE,                 10);
SOURCE_TOOLS_REGISTER_KEYWORD(FALSE,                11);
SOURCE_TOOLS_REGISTER_KEYWORD(NULL,                 12);
SOURCE_TOOLS_REGISTER_KEYWORD(Inf,                  13);
SOURCE_TOOLS_REGISTER_KEYWORD(NaN,                  14);
SOURCE_TOOLS_REGISTER_KEYWORD(NA,                   15);
SOURCE_TOOLS_REGISTER_KEYWORD(NA_integer_,          16);
SOURCE_TOOLS_REGISTER_KEYWORD(NA_real_,             17);
SOURCE_TOOLS_REGISTER_KEYWORD(NA_complex_,          18);
SOURCE_TOOLS_REGISTER_KEYWORD(NA_character_,        19);

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

  Token()
    : begin_(empty().begin()),
      end_(empty().end()),
      type_(ERR)
  {
  }

  explicit Token(TokenType type)
    : begin_(empty().begin()),
      end_(empty().end()),
      type_(type)
  {
  }

  Token(const Position& position)
    : begin_(empty().begin()),
      end_(empty().end()),
      position_(position),
      type_(ERR)
  {
  }

  Token(const TextCursor& cursor, TokenType type, std::size_t length)
      : begin_(cursor.begin() + cursor.offset()),
        end_(cursor.begin() + cursor.offset() + length),
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
  bool isType(TokenType type) const { return type_ == type; }

private:
  std::string::const_iterator begin_;
  std::string::const_iterator end_;

  Position position_;
  TokenType type_;

  static const std::string& empty()
  {
    static std::string instance;
    return instance;
  }
};

inline namespace utils {

inline bool isBracket(const Token& token)
{
  return SOURCE_TOOLS_CHECK_MASK(token.type(), SOURCE_TOOLS_BRACKET_MASK);
}

inline bool isLeftBracket(const Token& token)
{
  return SOURCE_TOOLS_CHECK_MASK(token.type(), SOURCE_TOOLS_BRACKET_LEFT_MASK);
}

inline bool isRightBracket(const Token& token)
{
  return SOURCE_TOOLS_CHECK_MASK(token.type(), SOURCE_TOOLS_BRACKET_RIGHT_MASK);
}

inline bool isComplement(TokenType lhs, TokenType rhs)
{
  static const TokenType mask =
    SOURCE_TOOLS_BRACKET_BIT | SOURCE_TOOLS_BRACKET_LEFT_BIT | SOURCE_TOOLS_BRACKET_RIGHT_BIT;

  if (SOURCE_TOOLS_CHECK_MASK((lhs | rhs), mask))
    return SOURCE_TOOLS_LOWER_BITS(lhs, 4) == SOURCE_TOOLS_LOWER_BITS(rhs, 4);

  return false;
}

inline TokenType complement(TokenType type)
{
  static const TokenType mask =
    SOURCE_TOOLS_BRACKET_LEFT_BIT | SOURCE_TOOLS_BRACKET_RIGHT_BIT;
  return type ^ mask;
}

inline bool isKeyword(const Token& token)
{
  return SOURCE_TOOLS_CHECK_MASK(token.type(), SOURCE_TOOLS_KEYWORD_MASK);
}

inline bool isControlFlowKeyword(const Token& token)
{
  return SOURCE_TOOLS_CHECK_MASK(token.type(), SOURCE_TOOLS_KEYWORD_CONTROL_FLOW_MASK);
}

inline bool isOperator(const Token& token)
{
  return SOURCE_TOOLS_CHECK_MASK(token.type(), SOURCE_TOOLS_OPERATOR_MASK);
}

inline bool isUnaryOperator(const Token& token)
{
  return SOURCE_TOOLS_CHECK_MASK(token.type(), SOURCE_TOOLS_OPERATOR_UNARY_MASK);
}

inline bool isNonUnaryOperator(const Token& token)
{
  return isOperator(token) && !isUnaryOperator(token);
}

inline bool isWhitespace(const Token& token)
{
  return token.type() == WHITESPACE;
}

inline bool isComment(const Token& token)
{
  return token.type() == COMMENT;
}

inline bool isSymbol(const Token& token)
{
  return token.type() == SYMBOL;
}

inline bool isEnd(const Token& token)
{
  return token.type() == END;
}

inline bool isString(const Token& token)
{
  return token.type() == STRING;
}

inline bool isSymbolic(const Token& token)
{
  static const TokenType mask = SYMBOL | NUMBER | STRING;
  return (token.type() & mask) != 0;
}

inline bool isNumeric(const Token& token)
{
  return (token.type() & NUMBER) != 0;
}

inline bool isCallOperator(const Token& token)
{
  return token.type() == LPAREN ||
         token.type() == LBRACKET ||
         token.type() == LDBRACKET;
}

} // namespace utils
} // namespace tokens

inline std::string toString(tokens::TokenType type)
{
  using namespace tokens;

       if (type == ERR)        return "err";
  else if (type == END)        return "end";
  else if (type == EMPTY)      return "empty";
  else if (type == SEMI)       return "semi";
  else if (type == COMMA)      return "comma";
  else if (type == SYMBOL)     return "symbol";
  else if (type == COMMENT)    return "comment";
  else if (type == WHITESPACE) return "whitespace";
  else if (type == STRING)     return "string";
  else if (type == NUMBER)     return "number";

  else if (SOURCE_TOOLS_CHECK_MASK(type, SOURCE_TOOLS_BRACKET_MASK))
    return "bracket";
  else if (SOURCE_TOOLS_CHECK_MASK(type, SOURCE_TOOLS_KEYWORD_MASK))
    return "keyword";
  else if (SOURCE_TOOLS_CHECK_MASK(type, SOURCE_TOOLS_OPERATOR_MASK))
    return "operator";

  return "unknown";
}

inline std::string toString(const tokens::Token& token)
{
  std::string contents;
  if (token.isType(tokens::END))
    contents = "<END>";
  else if (token.isType(tokens::EMPTY))
    contents = "<empty>";
  else
    contents = token.contents();

  char buff[1024];
  ::snprintf(buff,
             1024 - 1,
             "[%4lu:%4lu]: %s\n",
             (unsigned long) token.row(),
             (unsigned long) token.column(),
             contents.c_str());
  return buff;
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

} // namespace sourcetools

#endif /* SOURCE_TOOLS_TOKENS_TOKENS_H */
