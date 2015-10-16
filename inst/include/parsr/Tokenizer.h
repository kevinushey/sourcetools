#ifndef PARSR_TOKENIZER_H
#define PARSR_TOKENIZER_H

#include <parsr/Macros.h>
#include <parsr/Utils.h>

#include <parsr/tokens/Token.h>

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
  Tokenizer() {}

private:
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
      consumeToken(cursor, tokens::ERR, distance + 1);
    }
  }

  void consumeOperator(TextCursor& cursor)
  {
    consumeUntil(cursor, '%', tokens::OPERATOR);
  }

  void consumeComment(TextCursor& cursor)
  {
    consumeUntil(cursor, '\n', tokens::COMMENT);
  }

  void consumeQuotedSymbol(TextCursor& cursor)
  {
    consumeUntil(cursor, '`', tokens::SYMBOL, true);
  }

  void consumeQString(TextCursor& cursor)
  {
    consumeUntil(cursor, '\'', tokens::STRING, true);
  }

  void consumeQQString(TextCursor& cursor)
  {
    consumeUntil(cursor, '"', tokens::STRING, true);
  }

  void consumeWhitespace(TextCursor& cursor)
  {
    std::size_t distance = 1;
    while (std::isspace(cursor.peek(distance)))
      ++distance;

    consumeToken(cursor, tokens::WHITESPACE, distance);
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
    return utils::isValidForStartOfRSymbol(cursor.peek());
  }

  void consumeNumber(TextCursor& cursor)
  {
    bool success = true;
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
      success = std::isdigit(cursor.peek(distance));
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
      success = std::isdigit(cursor.peek(distance));
      while (std::isdigit(cursor.peek(distance)))
        ++distance;

      // Consume '.' and following numbers. Note that this is
      // not really a valid number for R but it's better to tokenize
      // this is a single entity (and then report failure later)
      if (cursor.peek(distance) == '.') {
        success = false;
        ++distance;
        while (std::isdigit(cursor.peek(distance)))
          ++distance;
      }
    }

    // Consume a final 'L' for integer literals
    if (cursor.peek(distance) == 'L')
      ++distance;

    consumeToken(cursor, success ? tokens::NUMBER : tokens::ERR, distance);
  }

  void consumeSymbol(TextCursor& cursor)
  {
    std::size_t distance = 1;
    char ch = cursor.peek(distance);
    while (utils::isValidForRSymbol(ch)) {
      ++distance;
      ch = cursor.peek(distance);
    }

    const char* ptr = &*(cursor.begin() + cursor.offset());
    consumeToken(cursor, tokens::symbolType(ptr, distance), distance);
  }

public:

  const std::vector<Token>& tokenize(const std::string& code)
  {
    TextCursor cursor(code);

    while (cursor.isValid()) {
      char ch = cursor.peek();

      // Block-related tokens
      if (ch == '{')
        consumeToken(cursor, tokens::LBRACE, 1);
      else if (ch == '}')
        consumeToken(cursor, tokens::RBRACE, 1);
      else if (ch == '(')
        consumeToken(cursor, tokens::LPAREN, 1);
      else if (ch == ')')
        consumeToken(cursor, tokens::RPAREN, 1);
      else if (ch == '[') {
        if (cursor.peek(1) == '[') {
          consumeToken(cursor, tokens::LDBRACKET, 2);
          tokenStack_.push(tokens::LDBRACKET);
        } else {
          consumeToken(cursor, tokens::LBRACKET, 1);
          tokenStack_.push(tokens::LBRACKET);
        }
      } else if (ch == ']') {
        if (tokenStack_.empty()) {
          consumeToken(cursor, tokens::ERR, 1);
        } else if (tokenStack_.top() == tokens::LDBRACKET) {
          if (cursor.peek(1) == ']')
            consumeToken(cursor, tokens::RDBRACKET, 2);
          else
            consumeToken(cursor, tokens::ERR, 1);
          tokenStack_.pop();
        } else {
          consumeToken(cursor, tokens::RBRACKET, 1);
          tokenStack_.pop();
        }
      }

      // Operators
      else if (ch == '<')  // <<-, <=, <-, <
      {
        char next = cursor.peek(1);
        if (next == '-' || next == '=')
          consumeToken(cursor, tokens::OPERATOR, 2);
        else if (next == '<' && cursor.peek(2) == '-')
          consumeToken(cursor, tokens::OPERATOR, 3);
        else
          consumeToken(cursor, tokens::OPERATOR, 1);
      }

      else if (ch == '>')  // >=, >
        consumeToken(cursor, tokens::OPERATOR, 1 + (cursor.peek(1) == '='));
      else if (ch == '=')  // '==', '='
        consumeToken(cursor, tokens::OPERATOR, 1 + (cursor.peek(1) == '='));
      else if (ch == '|')  // '||', '|'
        consumeToken(cursor, tokens::OPERATOR, 1 + (cursor.peek(1) == '|'));
      else if (ch == '&')  // '&&', '&'
        consumeToken(cursor, tokens::OPERATOR, 1 + (cursor.peek(1) == '&'));
      else if (ch == '*')  // **, *
        consumeToken(cursor, tokens::OPERATOR, 1 + (cursor.peek(1) == '*'));
      else if (ch == ':')  // ':::', '::', ':=', ':'
      {
        if (cursor.peek(1) == ':')
          consumeToken(cursor, tokens::OPERATOR, 2 + (cursor.peek(2) == ':'));
        else
          consumeToken(cursor, tokens::OPERATOR, 1 + (cursor.peek(1) == '='));
      }
      else if (ch == '!')
      {
        if (cursor.peek(1) == '=')
          consumeToken(cursor, tokens::OPERATOR, 2);
        else
          consumeToken(cursor, tokens::OPERATOR_CAN_BE_UNARY, 1);
      }
      else if (ch == '-') // '->>', '->', '-'
      {
        if (cursor.peek(1) == '>')
          consumeToken(cursor, tokens::OPERATOR, 2 + (cursor.peek(2) == '>'));
        else
          consumeToken(cursor, tokens::OPERATOR_CAN_BE_UNARY, 1);
      }
      else if (ch == '+' || ch == '~' || ch == '?')
        consumeToken(cursor, tokens::OPERATOR_CAN_BE_UNARY, 1);
      else if (ch == '/' || ch == '@' || ch == '$' ||
               ch == '^' || ch == '?')
        consumeToken(cursor, tokens::OPERATOR, 1);

      // User operators
      else if (ch == '%')
        consumeOperator(cursor);

      // Punctuation-related tokens
      else if (ch == ',')
        consumeToken(cursor, tokens::COMMA, 1);
      else if (ch == ';')
        consumeToken(cursor, tokens::SEMI, 1);

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
        consumeToken(cursor, tokens::ERR, 1);
    }
    return tokens_;
  }

private:
  std::vector<Token> tokens_;
  std::stack<TokenType, std::vector<TokenType>> tokenStack_;
};

inline std::vector<tokens::Token> tokenize(const std::string& code)
{
  Tokenizer tokenizer;
  return tokenizer.tokenize(code);
}

}  // namespace parsr

#endif
