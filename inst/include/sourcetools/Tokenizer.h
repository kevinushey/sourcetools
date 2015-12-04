#ifndef SOURCE_TOOLS_TOKENIZER_H
#define SOURCE_TOOLS_TOKENIZER_H

#include <sourcetools/Macros.h>
#include <sourcetools/Utils.h>

#include <sourcetools/tokens/Token.h>

#include <sourcetools/cursors/TextCursor.h>

#include <vector>
#include <stack>
#include <sstream>

namespace sourcetools {

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

  void consumeUserOperator(TextCursor& cursor)
  {
    consumeUntil(cursor, '%', tokens::OPERATOR_USER);
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

  // NOTE: Don't tokenize '-' or '+' as part of number; instead
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

  bool isHexDigit(char c)
  {
    return
      (c >= '0' && c <= '9') ||
      (c >= 'A' && c <= 'F') ||
      (c >= 'a' && c <= 'f');
  }

  bool consumeHexadecimalNumber(TextCursor& cursor)
  {
    std::size_t distance = 0;

    // Detect the leading '0'.
    if (cursor.peek(distance) != '0')
      return false;
    ++distance;

    // Detect a 'x' or 'X'.
    if (!(cursor.peek(distance) == 'x' || cursor.peek(distance) == 'X'))
      return false;
    ++distance;

    // Check and consume all non-whitespace characters.
    // The number is valid if the characters are valid
    // hexadecimal characters (0-9, a-f, A-F). The number
    // can also end with an 'i' (for an imaginary number)
    // or with an 'L' for an integer.
    if (!isHexDigit(cursor.peek(distance)))
    {
      consumeToken(cursor, tokens::ERR, distance);
      return false;
    }

    bool success = true;
    char peek = cursor.peek(distance);
    while (!std::isspace(peek) && peek != '\0') {

      // If we encounter an 'i' or an 'L', check to
      // see if this ends the identifier.
      if (peek == 'i' || peek == 'L')
      {
        char next = cursor.peek(distance + 1);
        if (std::isspace(next) || next == '\0')
        {
          ++distance;
          break;
        }
      }

      if (!isHexDigit(peek))
        success = false;

      ++distance;
      peek = cursor.peek(distance);
    }

    consumeToken(cursor, success ? tokens::NUMBER : tokens::ERR, distance);
    return true;
  }

  void consumeNumber(TextCursor& cursor)
  {
    bool success = true;
    std::size_t distance = 0;

    // NOTE: A leading '-' or '+' is not consumed as part of
    // the number.

    // Try parsing this as a hexadecimal number first (e.g. '0xabc').
    if (consumeHexadecimalNumber(cursor))
      return;

    // Consume digits
    while (std::isdigit(cursor.peek(distance)))
      ++distance;

    // Consume a dot for decimals
    // Note: '.5' is a valid specification for a number
    // So is '100.'; ie, with a trailing decimal.
    if (cursor.peek(distance) == '.') {
      ++distance;
      while (std::isdigit(cursor.peek(distance)))
        ++distance;
    }

    // Consume 'e', 'E' for exponential notation
    if (cursor.peek(distance) == 'e' || cursor.peek(distance) == 'E') {
      ++distance;

      // Consume a '-' or a '+' for a negative number
      if (cursor.peek(distance) == '-' || cursor.peek(distance) == '+')
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
        if (next == '-') // <-
          consumeToken(cursor, tokens::OPERATOR_ASSIGN_LEFT, 2);
        else if (next == '=') // <=
          consumeToken(cursor, tokens::OPERATOR_LESS_OR_EQUAL, 2);
        else if (next == '<' && cursor.peek(2) == '-')
          consumeToken(cursor, tokens::OPERATOR_ASSIGN_LEFT_PARENT, 3);
        else
          consumeToken(cursor, tokens::OPERATOR_LESS, 1);
      }

      else if (ch == '>')  // >=, >
      {
        if (cursor.peek(1) == '=')
          consumeToken(cursor, tokens::OPERATOR_GREATER_OR_EQUAL, 2);
        else
          consumeToken(cursor, tokens::OPERATOR_GREATER, 1);
      }
      else if (ch == '=')  // '==', '='
      {
        if (cursor.peek(1) == '=')
          consumeToken(cursor, tokens::OPERATOR_EQUAL, 2);
        else
          consumeToken(cursor, tokens::OPERATOR_ASSIGN_LEFT_EQUALS, 1);
      }
      else if (ch == '|')  // '||', '|'
      {
        if (cursor.peek(1) == '|')
          consumeToken(cursor, tokens::OPERATOR_OR_SCALAR, 2);
        else
          consumeToken(cursor, tokens::OPERATOR_OR_VECTOR, 1);
      }
      else if (ch == '&')  // '&&', '&'
      {
        if (cursor.peek(1) == '&')
          consumeToken(cursor, tokens::OPERATOR_AND_SCALAR, 2);
        else
          consumeToken(cursor, tokens::OPERATOR_AND_VECTOR, 1);
      }
      else if (ch == '*')  // **, *
      {
        if (cursor.peek(1) == '*')
          consumeToken(cursor, tokens::OPERATOR_EXPONENTATION_STARS, 2);
        else
          consumeToken(cursor, tokens::OPERATOR_MULTIPLY, 1);
      }
      else if (ch == ':')  // ':::', '::', ':=', ':'
      {
        if (cursor.peek(1) == ':')
        {
          if (cursor.peek(2) == ':')
            consumeToken(cursor, tokens::OPERATOR_NAMESPACE_ALL, 3);
          else
            consumeToken(cursor, tokens::OPERATOR_NAMESPACE_EXPORTS, 2);
        }
        else if (cursor.peek(1) == '=')
          consumeToken(cursor, tokens::OPERATOR_ASSIGN_LEFT_COLON, 2);
        else
          consumeToken(cursor, tokens::OPERATOR_SEQUENCE, 1);
      }
      else if (ch == '!')
      {
        if (cursor.peek(1) == '=')
          consumeToken(cursor, tokens::OPERATOR_NOT_EQUAL, 2);
        else
          consumeToken(cursor, tokens::OPERATOR_NEGATION, 1);
      }
      else if (ch == '-') // '->>', '->', '-'
      {
        if (cursor.peek(1) == '>')
        {
          if (cursor.peek(2) == '>')
            consumeToken(cursor, tokens::OPERATOR_ASSIGN_RIGHT_PARENT, 3);
          else
            consumeToken(cursor, tokens::OPERATOR_ASSIGN_RIGHT, 2);
        }
        else
          consumeToken(cursor, tokens::OPERATOR_MINUS, 1);
      }
      else if (ch == '+')
        consumeToken(cursor, tokens::OPERATOR_PLUS, 1);
      else if (ch == '~')
        consumeToken(cursor, tokens::OPERATOR_FORMULA, 1);
      else if (ch == '?')
        consumeToken(cursor, tokens::OPERATOR_HELP, 1);
      else if (ch == '/')
        consumeToken(cursor, tokens::OPERATOR_DIVIDE, 1);
      else if (ch == '@')
        consumeToken(cursor, tokens::OPERATOR_AT, 1);
      else if (ch == '$')
        consumeToken(cursor, tokens::OPERATOR_DOLLAR, 1);
      else if (ch == '^')
        consumeToken(cursor, tokens::OPERATOR_HAT, 1);

      // User operators
      else if (ch == '%')
        consumeUserOperator(cursor);

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

}  // namespace sourcetools

#endif
