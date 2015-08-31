#ifndef PARSR_VALIDATORS_SYNTAX_VALIDATOR_H
#define PARSR_VALIDATORS_SYNTAX_VALIDATOR_H

#include <sstream>
#include <vector>

#include <parsr/collections/Position.h>
#include <parsr/tokens/Token.h>
#include <parsr/cursors/TokenCursor.h>

namespace parsr {
namespace validators {

class SyntaxError {
private:
  typedef collections::Position Position;

public:

  explicit SyntaxError(const Position& position,
                       const std::string& message)
    : position_(position),
      message_(message)
  {}

  std::string report()
  {
    std::ostringstream os;
    os << "[" << position_.row << ":" << position_.column << "]: "
       << message_;

    return os.str();
  }

  std::size_t row() const { return position_.row; }
  std::size_t column() const { return position_.column; }
  const Position& position() const { return position_; }
  const std::string& message() const { return message_; }

private:
  Position position_;
  std::string message_;
};

class SyntaxValidator {

private:

  typedef tokens::Token Token;
  typedef cursors::TokenCursor Cursor;
  typedef tokens::TokenType TokenType;

  void unexpectedToken(const Token& token,
                       const std::string& expected = std::string())
  {
    std::string message = "unexpected token '" + token.contents() + "'";
    if (!expected.empty())
      message += " (expected '" + expected + "')";

    errors_.push_back(SyntaxError(token.position(), message));
  }

public:

  explicit SyntaxValidator(const std::vector<Token>& tokens)
  {
    using namespace tokens::utils;

    Cursor cursor(tokens);
    std::vector<TokenType> stack;
    stack.push_back(TokenType::LBRACE);

    do
    {
      const Token& curr = cursor.currentToken();
      const Token& next = cursor.nextSignificantToken();

      bool sameRow = curr.row() == next.row();
      TokenType bracketType = stack[stack.size() - 1];

      TokenType currType = curr.type();
      TokenType nextType = next.type();

      // Update brace state
      if (isLeftBracket(curr)) {
        stack.push_back(currType);
      } else if (isRightBracket(curr)) {
        std::size_t size = stack.size();
        auto last = stack[size - 1];
        if (size == 1) {
          unexpectedToken(cursor);
        } else {
          if (!isComplement(currType, last))
            unexpectedToken(cursor, toString(complement(last)));
          stack.pop_back();
        }
      }

      /* Check for syntax errors */

      if (sameRow && isSymbolic(curr) && isSymbolic(next)) {
        errors_.push_back(SyntaxError(
          cursor.position(),
          "unexpected token '" + next.contents() + "'"
        ));
      }

      else if (sameRow && isSymbolic(curr) && nextType == TokenType::LBRACE) {
        unexpectedToken(next);
      }

      else if (currType == TokenType::COMMA && isRightBracket(next)) {
        errors_.push_back(SyntaxError(
          cursor.position(),
          "unexpected closing bracket '" + next.contents() + "'"
        ));
      }

      else if (currType == TokenType::OPERATOR && nextType == TokenType::OPERATOR) {
        errors_.push_back(SyntaxError(
          cursor.position(),
          "unexpected operator '" + next.contents() + "'"
        ));
      }

      else if (currType == TokenType::SEMI) {
        if (bracketType != TokenType::LBRACE)
          unexpectedToken(curr, ",");
      }

      else if (currType == TokenType::COMMA) {
        if (bracketType == TokenType::LBRACE)
          unexpectedToken(curr, ";");
      }

    } while (cursor.moveToNextToken());

  }

  const std::vector<SyntaxError>& errors() const { return errors_; }

private:

  std::vector<SyntaxError> errors_;

};

} // namespace validators
} // namespace parsr

#endif /* PARSR_VALIDATORS_SYNTAX_VALIDATOR_H */
