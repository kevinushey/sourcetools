#ifndef SOURCE_TOOLS_VALIDATORS_SYNTAX_VALIDATOR_H
#define SOURCE_TOOLS_VALIDATORS_SYNTAX_VALIDATOR_H

#include <sstream>
#include <vector>

#include <sourcetools/collection/Position.h>
#include <sourcetools/tokenization/Token.h>
#include <sourcetools/cursor/TokenCursor.h>

namespace sourcetools {
namespace validators {

class SyntaxError {
private:
  typedef collections::Position Position;
  typedef cursors::TokenCursor TokenCursor;
  typedef tokens::Token Token;

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
  typedef cursors::TokenCursor TokenCursor;
  typedef tokens::TokenType TokenType;

  void unexpectedToken(const Token& token, const std::string& expected = std::string())
  {
    std::string message = "unexpected token '" + token.contents() + "'";
    if (!expected.empty())
      message += " (expected '" + expected + "')";

    errors_.push_back(SyntaxError(token.position(), message));
  }

  void updateBracketStack(const Token& token, std::vector<TokenType>* pStack)
  {
    using namespace tokens::utils;

    // Update brace state
    if (isLeftBracket(token)) {
      pStack->push_back(token.type());
    } else if (isRightBracket(token)) {
      std::size_t size = pStack->size();
      auto last = pStack->at(size - 1);
      if (size == 1) {
        unexpectedToken(token);
      } else {
        if (!isComplement(token.type(), last))
          unexpectedToken(token, toString(complement(last)));
        pStack->pop_back();
      }
    }
  }

public:

  explicit SyntaxValidator(const std::vector<Token>& tokens)
  {
    if (tokens.empty())
      return;

    TokenCursor cursor(tokens);
    std::vector<TokenType> stack;
    stack.push_back(tokens::ERR);

    const Token* pThisToken = &(cursor.currentToken());
    const Token* pPrevToken = pThisToken;

    while (cursor.moveToNextSignificantToken()) {

      pPrevToken = pThisToken;
      pThisToken = &(cursor.currentToken());

      updateBracketStack(cursor.currentToken(), &stack);
      executeValidators(*pPrevToken, *pThisToken);

    }
  }

  const std::vector<SyntaxError>& errors() const { return errors_; }

private:

  void executeValidators(const tokens::Token& prevToken,
                         const tokens::Token& thisToken)
  {
    using namespace tokens::utils;

    if (isOperator(prevToken)) {

      // Operator followed non-unary operator.
      if (isNonUnaryOperator(thisToken))
        unexpectedToken(thisToken);

      // Operator (other than =) followed by any kind of right bracket.
      // We need to allow e.g. 'parse(text = )'.
      if (isRightBracket(thisToken) && !prevToken.isType(tokens::OPERATOR_ASSIGN_LEFT_EQUALS))
        unexpectedToken(thisToken);

      // Operator followed by '[' or '[['.
      if (thisToken.isType(tokens::LBRACKET) ||
          thisToken.isType(tokens::LDBRACKET))
        unexpectedToken(thisToken);
    }

    else if (isSymbolic(prevToken)) {

      // Two symbols on the same line.
      if (isSymbolic(thisToken) && prevToken.row() == thisToken.row())
        unexpectedToken(thisToken);
    }

  }

  std::vector<SyntaxError> errors_;

};

} // namespace validators
} // namespace sourcetools

#endif /* SOURCE_TOOLS_VALIDATORS_SYNTAX_VALIDATOR_H */
