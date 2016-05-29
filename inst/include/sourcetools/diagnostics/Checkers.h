#ifndef SOURCETOOLS__DIAGNOSTICS__CHECKERS_H
#define SOURCETOOLS__DIAGNOSTICS__CHECKERS_H

#include <vector>
#include <set>
#include <string>

#include <sourcetools/r/r.h>
#include <sourcetools/parse/ParseNode.h>
#include <sourcetools/diagnostics/Diagnostic.h>

namespace sourcetools {
namespace diagnostics {
namespace checkers {

class CheckerBase
{
public:
  typedef tokens::Token Token;
  typedef tokens::TokenType TokenType;
  typedef parser::ParseNode ParseNode;

  virtual void apply(const ParseNode* pNode, Diagnostics* pDiagnostics, std::size_t depth) = 0;
  virtual ~CheckerBase() {}
};

/**
 * Warn about code of the form:
 *
 *    x == NULL
 *
 * The user likely intended to check if a value was NULL,
 * and in such a case should use `is.null()` instead.
 */
class ComparisonWithNullChecker : public CheckerBase
{
public:
  void apply(const ParseNode* pNode, Diagnostics* pDiagnostics, std::size_t depth)
  {
    const Token& token = pNode->token();
    bool isEquals =
      token.isType(tokens::OPERATOR_EQUAL) ||
      token.isType(tokens::OPERATOR_NOT_EQUAL);

    if (!isEquals)
      return;

    if (pNode->children().size() != 2)
      return;

    ParseNode* pLhs = pNode->children()[0];
    ParseNode* pRhs = pNode->children()[1];

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
  void apply(const ParseNode* pNode, Diagnostics* pDiagnostics, std::size_t depth)
  {
    if (!pNode->token().isType(tokens::KEYWORD_IF))
      return;

    if (pNode->children().size() < 1)
      return;

    ParseNode* pCondition = pNode->children()[0];
    if (!pCondition->token().isType(tokens::OPERATOR_ASSIGN_LEFT_EQUALS))
      return;

    pDiagnostics->addWarning(
      "Using '=' for assignment in 'if' condition",
      pCondition->range());

  }
};

/**
 * Warn about vectorized '&' or '|' used in
 * 'if' statements. The scalar forms, '&&' and '||',
 * are likely preferred.
 */
class ScalarOpsInIfChecker : public CheckerBase
{
public:
  void apply(const ParseNode* pNode, Diagnostics* pDiagnostics, std::size_t depth)
  {
    if (!pNode->token().isType(tokens::KEYWORD_IF))
      return;

    if (pNode->children().size() < 1)
      return;

    ParseNode* pCondition = pNode->children()[0];
    const Token& token = pCondition->token();
    if (token.isType(tokens::OPERATOR_AND_VECTOR))
    {
      pDiagnostics->addInfo(
        "Prefer '&&' to '&' in 'if' statement condition",
        pCondition->range());
    }
    else if (token.isType(tokens::OPERATOR_OR_VECTOR))
    {
      pDiagnostics->addInfo(
        "Prefer '||' to '|' in 'if' statement condition",
        pCondition->range());
    }
  }
};

/**
 * Warn about unused computations, e.g.
 *
 *    foo <- function(x) {
 *       x < 1
 *       print(x)
 *    }
 *
 * For example, in the above code, it's possible that the user
 * intended to assign 1 to x, or use that result elsewhere.
 *
 * Don't warn if the expression shows up as the last statement
 * within a parent function's body.
 */
class UnusedResultChecker : public CheckerBase
{
public:
  void apply(const ParseNode* pNode, Diagnostics* pDiagnostics, std::size_t depth)
  {
    if (pNode->parent() == NULL)
      return;

    const Token& parentToken = pNode->parent()->token();
    bool isTopLevelContext =
      parentToken.isType(tokens::ROOT) ||
      parentToken.isType(tokens::LBRACE);

    if (!isTopLevelContext)
      return;

    if (parentToken.isType(tokens::LBRACE))
    {
      const std::vector<ParseNode*>& siblings = pNode->parent()->children();
      if (pNode == siblings[siblings.size() - 1])
        return;
    }

    const Token& token = pNode->token();
    if (!tokens::isOperator(token))
      return;

    if (tokens::isAssignmentOperator(token))
      return;


    pDiagnostics->addInfo(
      "result of computation is not used",
      pNode->range());
  }
};

class NoSymbolInScopeChecker : public CheckerBase
{
public:

  NoSymbolInScopeChecker()
  {
    stack_.push_back(Context(0));
    objects_ = r::objectsOnSearchPath();
  }

  void apply(const ParseNode* pNode, Diagnostics* pDiagnostics, std::size_t depth)
  {
    using namespace tokens;
    const Token& token = pNode->token();

    // If we've left the last active scope, pop.
    if (depth < current().depth())
      pop();

    // Assignments update the current scope.
    if (token.isType(OPERATOR_ASSIGN_LEFT) ||
        token.isType(OPERATOR_ASSIGN_LEFT_EQUALS))
    {
      const ParseNode* pChild = pNode->children()[0];
      const Token& symbol = pChild->token();
      if (symbol.isType(SYMBOL) || symbol.isType(STRING))
        add(symbol);
    }

    // Check if a symbol has a definition in scope.
    if (token.isType(SYMBOL))
      check(token, pDiagnostics);

    // If we encounter a function definition, create a new scope
    // and make the function argument names present in that scope.
    if (token.isType(KEYWORD_FUNCTION))
      push(pNode, depth);
  }

private:

  class Context
  {
  public:
    explicit Context(std::size_t depth)
      : depth_(depth)
    {
    }

    void add(const Token& token)
    {
      values_.insert(token.contents());
    }

    bool contains(const std::string& contents) const
    {
      return values_.count(contents);
    }

    std::size_t depth() const
    {
      return depth_;
    }

  private:
    std::set<std::string> values_;
    std::size_t depth_;
  };

  Context& current()
  {
    return stack_[stack_.size() - 1];
  }

  void push(const ParseNode* pNode, std::size_t depth)
  {
    stack_.push_back(Context(depth));

    ParseNode* pFormals = pNode->children()[0];
    const std::vector<ParseNode*>& children = pFormals->children();
    for (std::vector<ParseNode*>::const_iterator it = children.begin();
         it != children.end();
         ++it)
    {
      const Token& token = (*it)->token();
      if (token.isType(tokens::SYMBOL))
        add(token);
      else if (token.isType(tokens::OPERATOR_ASSIGN_LEFT_EQUALS))
      {
        const Token& lhs = (*it)->children()[0]->token();
        if (lhs.isType(tokens::SYMBOL))
          add(lhs);
      }
    }
  }

  void pop()
  {
    stack_.pop_back();
  }

  void add(const Token& token)
  {
    current().add(token);
  }

  void check(const Token& token, Diagnostics* pDiagnostics)
  {
    if (!token.isType(tokens::SYMBOL))
      return;

    std::string contents = token.contents();
    for (std::vector<Context>::const_iterator it = stack_.begin();
         it != stack_.end();
         ++it)
    {
      if (it->contains(contents))
      {
        return;
      }
    }

    if (objects_.count(token.contents()))
      return;

    collections::Range range(token.position(), token.position() + token.size());
    pDiagnostics->addWarning(
        "use of undefined symbol '" + token.contents() + "'",
        range);
  }

  std::vector<Context> stack_;
  std::set<std::string> objects_;

};

} // namespace checkers
} // namespace diagnostics
} // namespace sourcetools

#endif /* SOURCETOOLS__DIAGNOSTICS__CHECKERS_H */
