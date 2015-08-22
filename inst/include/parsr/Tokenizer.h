#ifndef PARSR_TOKENIZER_H
#define PARSR_TOKENIZER_H

#include <parsr/Macros.h>
#include <parsr/Utils.h>

#include <parsr/tokens/Tokens.h>

#include <parsr/cursors/TextCursor.h>

#include <vector>
#include <stack>
#include <sstream>

namespace parsr {

class Tokenizer
{
private:
  typedef tokens::Token Token;
  typedef cursors::TextCursor TextCursor;
  typedef tokens::TokenType TokenType;

public:
  explicit Tokenizer(const std::string& contents)
  {
    TextCursor cursor(contents);
    tokenize(cursor);
  }

  const std::vector<Token>& tokens() const { return tokens_; }

  void consumeToken(TextCursor& cursor, TokenType type, std::size_t sz)
  {
    tokens_.push_back(Token(cursor, type, sz));
    cursor.moveForward(sz);
  }

  void consumeUntil(TextCursor& cursor, char ch, TokenType type,
                    bool skipEscaped = false)
  {
    TextCursor lookahead = cursor;

    bool success = false;
    std::size_t distance = 0;

    while (lookahead.isValid()) {
      lookahead.moveForward();
      ++distance;

      if (skipEscaped && lookahead.peek() == '\\') {
        lookahead.moveForward();
        ++distance;
        continue;
      }

      if (lookahead.peek() == ch) {
        success = true;
        break;
      }
    }

    if (success) {
      consumeToken(cursor, type, distance + 1);
    } else {
      consumeToken(cursor, TokenType::ERR, distance + 1);
    }
  }

  void consumeOperator(TextCursor& cursor)
  {
    consumeUntil(cursor, '%', TokenType::OPERATOR);
  }

  void consumeComment(TextCursor& cursor)
  {
    consumeUntil(cursor, '\n', TokenType::COMMENT);
  }

  void consumeQuotedSymbol(TextCursor& cursor)
  {
    consumeUntil(cursor, '`', TokenType::SYMBOL, true);
  }

  void consumeQString(TextCursor& cursor)
  {
    consumeUntil(cursor, '\'', TokenType::STRING, true);
  }

  void consumeQQString(TextCursor& cursor)
  {
    consumeUntil(cursor, '"', TokenType::STRING, true);
  }

  void consumeWhitespace(TextCursor& cursor)
  {
    std::size_t distance = 1;
    while (std::isspace(cursor.peek(distance)))
      ++distance;

    consumeToken(cursor, TokenType::WHITESPACE, distance);
  }

  // NOTE: Don't tokenize '-' as part of number; instead
  // it's parsed as a unary operator.
  bool isStartOfNumber(TextCursor& cursor)
  {
    char ch = cursor.peek();
    if (std::isdigit(ch))
      return true;
    if (ch == '.')
      return std::isdigit(cursor.peek(1));
    return false;
  }

  bool isStartOfSymbol(TextCursor& cursor)
  {
    return isValidForStartOfRSymbol(cursor.peek());
  }

  void consumeNumber(TextCursor& cursor)
  {
    std::size_t distance = 0;

    // NOTE: A leading `-` is not consumed as part of
    // the number.

    // Consume digits
    while (std::isdigit(cursor.peek(distance)))
      ++distance;

    // Consume a dot for decimals
    // Note: '.5' is a valid specification for a number
    if (cursor.peek(distance) == '.') {
      ++distance;
      while (std::isdigit(cursor.peek(distance)))
        ++distance;
    }

    // Consume 'e', 'E' for exponential notation
    if (cursor.peek(distance) == 'e' || cursor.peek(distance) == 'E') {
      ++distance;

      // Consume a '-' for a negative number
      if (cursor.peek(distance) == '-')
        ++distance;

      // Parse another set of numbers following the E
      while (std::isdigit(cursor.peek(distance)))
        ++distance;

      // Consume a decimal + numbers
      if (cursor.peek(distance) == '.') {
        ++distance;
        while (std::isdigit(cursor.peek(distance)))
          ++distance;
      }
    }

    // Consume a final 'L' for integer literals
    if (cursor.peek(distance) == 'L')
      ++distance;

    consumeToken(cursor, TokenType::NUMBER, distance);
  }

  void consumeSymbol(TextCursor& cursor)
  {
    std::size_t distance = 1;
    char ch = cursor.peek(distance);
    while (isValidForRSymbol(ch)) {
      ++distance;
      ch = cursor.peek(distance);
    }
    consumeToken(cursor, TokenType::SYMBOL, distance);
  }

  void tokenize(TextCursor& cursor)
  {
    while (cursor.isValid()) {
      char ch = cursor.peek();

      // Block-related tokens
      if (ch == '{')
        consumeToken(cursor, TokenType::LBRACE, 1);
      else if (ch == '}')
        consumeToken(cursor, TokenType::RBRACE, 1);
      else if (ch == '(')
        consumeToken(cursor, TokenType::LPAREN, 1);
      else if (ch == ')')
        consumeToken(cursor, TokenType::RPAREN, 1);
      else if (ch == '[') {
        if (cursor.peek(1) == '[') {
          consumeToken(cursor, TokenType::LDBRACKET, 2);
          tokenStack_.push(TokenType::LDBRACKET);
        } else {
          consumeToken(cursor, TokenType::LBRACKET, 1);
          tokenStack_.push(TokenType::LBRACKET);
        }
      } else if (ch == ']') {
        if (tokenStack_.empty()) {
          consumeToken(cursor, TokenType::ERR, 1);
        } else if (tokenStack_.top() == TokenType::LDBRACKET) {
          if (cursor.peek(1) == ']')
            consumeToken(cursor, TokenType::RDBRACKET, 2);
          else
            consumeToken(cursor, TokenType::ERR, 1);
        } else {
          consumeToken(cursor, TokenType::RBRACKET, 1);
        }
      }

      // Operators
      else if (ch == '<')  // <<-, <=, <-, <
      {
        char next = cursor.peek(1);
        if (next == '-' || next == '=')
          consumeToken(cursor, TokenType::OPERATOR, 2);
        else if (next == '<' && cursor.peek(2) == '-')
          consumeToken(cursor, TokenType::OPERATOR, 3);
        else
          consumeToken(cursor, TokenType::OPERATOR, 1);
      }

      else if (ch == '>')  // >=, >
        consumeToken(cursor, TokenType::OPERATOR, 1 + (cursor.peek(1) == '='));
      else if (ch == '=')  // '=', '=='
        consumeToken(cursor, TokenType::OPERATOR, 1 + (cursor.peek(1) == '='));
      else if (ch == '|')  // '||', '|'
        consumeToken(cursor, TokenType::OPERATOR, 1 + (cursor.peek(1) == '|'));
      else if (ch == '&')  // '&&', '&'
        consumeToken(cursor, TokenType::OPERATOR, 1 + (cursor.peek(1) == '&'));
      else if (ch == '*')  // **, *
        consumeToken(cursor, TokenType::OPERATOR, 1 + (cursor.peek(1) == '*'));
      else if (ch == ':')  // ':::', '::', ':=', ':'
      {
        if (cursor.peek(1) == ':')
          consumeToken(cursor, TokenType::OPERATOR, 2 + (cursor.peek(2) == ':'));
        else
          consumeToken(cursor, TokenType::OPERATOR, 1 + (cursor.peek(1) == '='));
      }
      else if (ch == '!')
        consumeToken(cursor, TokenType::OPERATOR, 1 + (cursor.peek(1) == '='));
      else if (ch == '-') // '->>', '->', '-'
      {
        if (cursor.peek(1) == '>')
          consumeToken(cursor, TokenType::OPERATOR, 2 + (cursor.peek(2) == '>'));
        else
          consumeToken(cursor, TokenType::OPERATOR, 1);
      }
      else if (ch == '+' || ch == '/' || ch == '~' ||
               ch == '@' || ch == '$' || ch == '^' ||
               ch == '?')
        consumeToken(cursor, TokenType::OPERATOR, 1);

      // User operators
      else if (ch == '%')
        consumeOperator(cursor);

      // Punctuation-related tokens
      else if (ch == ',')
        consumeToken(cursor, TokenType::COMMA, 1);
      else if (ch == ';')
        consumeToken(cursor, TokenType::SEMI, 1);

      // Whitespace
      else if (std::isspace(ch))
        consumeWhitespace(cursor);

      // Strings and symbols
      else if (ch == '\'')
        consumeQString(cursor);
      else if (ch == '"')
        consumeQQString(cursor);
      else if (ch == '`')
        consumeQuotedSymbol(cursor);

      // Comments
      else if (ch == '#')
        consumeComment(cursor);

      // Number
      else if (isStartOfNumber(cursor))
        consumeNumber(cursor);

      // Symbol
      else if (isStartOfSymbol(cursor))
        consumeSymbol(cursor);

      // Nothing matched -- error
      else
        consumeToken(cursor, TokenType::ERR, 1);
    }
  }

private:
  std::vector<Token> tokens_;
  std::stack<TokenType, std::vector<TokenType>> tokenStack_;
};

}  // namespace parsr

#endif
