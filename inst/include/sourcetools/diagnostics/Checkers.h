#ifndef SOURCE_TOOLS_DIAGNOSTICS_CHECKERS_H
#define SOURCE_TOOLS_DIAGNOSTICS_CHECKERS_H

#include <sourcetools/parse/Node.h>
#include <sourcetools/diagnostics/Diagnostic.h>

namespace sourcetools {
namespace diagnostics {
namespace checkers {

class CheckerBase
{
public:
  typedef tokens::Token Token;
  typedef tokens::TokenType TokenType;
  typedef parser::Node Node;

  virtual void apply(const Node* pNode, Diagnostics* pDiagnostics) const = 0;
  virtual ~CheckerBase() {}
};

class ComparisonWithNullChecker : public CheckerBase
{
public:
  void apply(const Node* pNode, Diagnostics* pDiagnostics) const
  {
    const Token& token = pNode->token();
    bool isEquals =
      token.isType(tokens::OPERATOR_EQUAL) ||
      token.isType(tokens::OPERATOR_NOT_EQUAL);

    if (!isEquals)
      return;

    if (pNode->children().size() != 2)
      return;

    Node* pLhs = pNode->children()[0];
    Node* pRhs = pNode->children()[1];

    if (pLhs->token().isType(tokens::KEYWORD_NULL) ||
        pRhs->token().isType(tokens::KEYWORD_NULL))
    {
      pDiagnostics->addWarning(
        "Use 'is.null()' to check if an object is NULL",
        pNode->range());
    }
  }
};

/**
 * Warn about code of the form:
 *
 *    if (x = 1) { ... }
 *
 * The user likely intended to write 'if (x == 1)'.
 */
class AssignmentInIfChecker : public CheckerBase
{
public:
  void apply(const Node* pNode, Diagnostics* pDiagnostics) const
  {
    if (!pNode->token().isType(tokens::KEYWORD_IF))
      return;

    if (pNode->children().size() < 1)
      return;

    Node* pCondition = pNode->children()[0];
    if (!pCondition->token().isType(tokens::OPERATOR_ASSIGN_LEFT_EQUALS))
      return;

    pDiagnostics->addWarning(
      "Using '=' for assignment in 'if' condition",
      pCondition->range());

  }
};

} // namespace checkers
} // namespace diagnostics
} // namespace sourcetools

#endif /* SOURCE_TOOLS_DIAGNOSTICS_CHECKERS_H */
