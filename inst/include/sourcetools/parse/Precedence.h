#ifndef SOURCETOOLS__PARSE__PRECEDENCE_H
#define SOURCETOOLS__PARSE__PRECEDENCE_H

#include <sourcetools/tokenization/tokenization.h>

namespace sourcetools {
namespace parser {
namespace precedence {

inline int binary(const tokens::Token& token)
{
  using namespace tokens;
  switch (token.type())
  {
  case OPERATOR_ASSIGN_LEFT_EQUALS:
    return 5;
  case OPERATOR_HELP:
    return 10;
  case OPERATOR_ASSIGN_LEFT_COLON:
    return 20;
  case OPERATOR_ASSIGN_LEFT:
  case OPERATOR_ASSIGN_LEFT_PARENT:
    return 30;
  case OPERATOR_ASSIGN_RIGHT:
  case OPERATOR_ASSIGN_RIGHT_PARENT:
    return 40;
  case OPERATOR_FORMULA:
    return 50;
  case OPERATOR_OR_SCALAR:
  case OPERATOR_OR_VECTOR:
    return 60;
  case OPERATOR_AND_SCALAR:
  case OPERATOR_AND_VECTOR:
    return 70;
  case OPERATOR_NEGATION:
    return 80;
  case OPERATOR_LESS:
  case OPERATOR_LESS_OR_EQUAL:
  case OPERATOR_GREATER:
  case OPERATOR_GREATER_OR_EQUAL:
  case OPERATOR_EQUAL:
  case OPERATOR_NOT_EQUAL:
    return 90;
  case OPERATOR_PLUS:
  case OPERATOR_MINUS:
    return 100;
  case OPERATOR_MULTIPLY:
  case OPERATOR_DIVIDE:
    return 110;
  case OPERATOR_USER:
    return 120;
  case OPERATOR_SEQUENCE:
    return 130;
  case OPERATOR_EXPONENTATION_STARS:
  case OPERATOR_HAT:
    return 150;
  case LPAREN:
  case LBRACKET:
  case LDBRACKET:
    return 170;
  case OPERATOR_DOLLAR:
  case OPERATOR_AT:
    return 180;
  case OPERATOR_NAMESPACE_EXPORTS:
  case OPERATOR_NAMESPACE_ALL:
    return 190;

  default:
    return 0;
  }
}

inline int unary(const tokens::Token& token)
{
  using namespace tokens;
  switch (token.type())
  {
  case OPERATOR_HELP:
    return 10;
  case OPERATOR_FORMULA:
    return 50;
  case OPERATOR_NEGATION:
    return 80;
  case OPERATOR_PLUS:
  case OPERATOR_MINUS:
    return 140;
  default:
    return 0;
  }
}

inline bool isRightAssociative(const tokens::Token& token)
{
  using namespace tokens;
  switch (token.type())
  {
  case OPERATOR_ASSIGN_LEFT:
  case OPERATOR_ASSIGN_LEFT_PARENT:
  case OPERATOR_ASSIGN_LEFT_EQUALS:
  case OPERATOR_EXPONENTATION_STARS:
  case OPERATOR_HAT:
  case LPAREN:
  case LBRACKET:
  case LDBRACKET:
    return true;
  default:
    return false;
  }
}

} // namespace precedence
} // namespace parser
} // namespace sourcetools

#endif /* SOURCETOOLS__PARSE__PRECEDENCE_H */
