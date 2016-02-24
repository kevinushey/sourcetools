#ifndef SOURCE_TOOLS_TOKENIZATION_TOKEN_H
#define SOURCE_TOOLS_TOKENIZATION_TOKEN_H

#include <cstring>

#include <vector>
#include <string>
#include <map>
#include <sstream>

#include <sourcetools/tokenization/Registration.h>
#include <sourcetools/collection/Position.h>
#include <sourcetools/cursor/TextCursor.h>

namespace sourcetools {
namespace tokens {

class Token
{
private:
  typedef cursors::TextCursor TextCursor;
  typedef collections::Position Position;

public:

  Token()
    : begin_(nullptr),
      end_(nullptr),
      type_(ERR)
  {
  }

  explicit Token(TokenType type)
    : begin_(nullptr),
      end_(nullptr),
      type_(type)
  {
  }

  Token(const Position& position)
    : begin_(nullptr),
      end_(nullptr),
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

  const char* begin() const { return begin_; }
  const char* end() const { return end_; }
  std::size_t size() const { return end_ - begin_; }

  std::string contents() const { return std::string(begin_, end_); }

  const Position& position() const { return position_; }
  std::size_t row() const { return position_.row; }
  std::size_t column() const { return position_.column; }

  TokenType type() const { return type_; }
  bool isType(TokenType type) const { return type_ == type; }

private:
  const char* begin_;
  const char* end_;

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

#endif /* SOURCE_TOOLS_TOKENIZATION_TOKEN_H */
