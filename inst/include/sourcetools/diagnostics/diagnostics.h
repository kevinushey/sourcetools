#ifndef SOURCE_TOOLS_DIAGNOSTICS_DIAGNOSTICS_H
#define SOURCE_TOOLS_DIAGNOSTICS_DIAGNOSTICS_H

#include <functional>

#include <sourcetools/parse/parse.h>

namespace sourcetools {
namespace diagnostics {

namespace detail {

class DiagnosticBase
{
public:
  typedef tokens::Token Token;
  typedef tokens::TokenType TokenType;
  typedef parser::Node Node;

  virtual void apply(const Node* pNode) const = 0;
  virtual ~DiagnosticBase() {}
};

class ComparisonWithNullDiagnostic : public detail::DiagnosticBase
{
public:
  void apply(const Node* pNode) const
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

    if (pLhs->token().contentsEqual("NULL") ||
        pRhs->token().contentsEqual("NULL"))
    {
      std::cerr << "Invalid NULL comparison\n";
    }
  }
};

} // namespace detail

class DiagnosticsSet
{
public:

  void add(detail::DiagnosticBase* pDiagnostic)
  {
    diagnostics_.push_back(pDiagnostic);
  }

  void run(const parser::Node* pNode)
  {
    for (Diagnostics::const_iterator it = diagnostics_.begin();
         it != diagnostics_.end();
         ++it)
    {
      (*it)->apply(pNode);
    }

    typedef parser::Node::Children Children;
    for (Children::const_iterator it = pNode->children().begin();
         it != pNode->children().end();
         ++it)
    {
      run(*it);
    }
  }

  ~DiagnosticsSet()
  {
    for (Diagnostics::const_iterator it = diagnostics_.begin();
         it != diagnostics_.end();
         ++it)
    {
      delete *it;
    }
  }

private:
  typedef std::vector<const detail::DiagnosticBase*> Diagnostics;
  Diagnostics diagnostics_;
};

} // namespace diagnostics
} // namespace sourcetools

#endif /* SOURCE_TOOLS_DIAGNOSTICS_DIAGNOSTICS_H */
