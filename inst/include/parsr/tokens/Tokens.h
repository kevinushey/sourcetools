#ifndef PARSR_TOKENS_TOKENS_H
#define PARSR_TOKENS_TOKENS_H

#include <vector>
#include <string>

#include <parsr/cursors/TextCursor.h>

namespace parsr {
namespace tokens {

template <typename T> class Stack {

public:
  Stack() : offset_(-1) { stack_.reserve(16); }

  T& peek() { return stack_[offset_]; }

  void push(const T& item)
  {
    stack_.push_back(item);
    ++offset_;
  }

  void pop()
  {
    stack_.pop_back();
    --offset_;
  }

  std::size_t size() { return stack_.size(); }

  bool empty() { return offset_ == -1; }

private:
  std::vector<T> stack_;
  int offset_;
};

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
  Token(const TextCursor& cursor, TokenType type, std::size_t tokenSize)
      : begin_(cursor.begin() + cursor.offset()),
        end_(cursor.begin() + cursor.offset() + tokenSize), row_(cursor.row()),
        column_(cursor.column()), type_(type)
  {
  }

  std::string content() const { return std::string(begin_, end_); }
  std::size_t row() const { return row_; }
  std::size_t column() const { return column_; }
  TokenType type() const { return type_; }

private:
  std::string::const_iterator begin_;
  std::string::const_iterator end_;

  std::size_t row_;
  std::size_t column_;

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
