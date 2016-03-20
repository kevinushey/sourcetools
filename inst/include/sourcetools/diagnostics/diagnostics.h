#ifndef SOURCE_TOOLS_DIAGNOSTICS_DIAGNOSTICS_H
#define SOURCE_TOOLS_DIAGNOSTICS_DIAGNOSTICS_H

#include <vector>
#include <functional>

#include <sourcetools/parse/parse.h>

namespace sourcetools {
namespace diagnostics {

enum DiagnosticType
{
  DIAGNOSTIC_ERROR,
  DIAGNOSTIC_WARNING,
  DIAGNOSTIC_INFO,
  DIAGNOSTIC_STYLE
};

class Diagnostic
{
public:
  Diagnostic(DiagnosticType type,
             const std::string& message,
             const collections::Range& range)
    : type_(type), message_(message), range_(range)
  {
  }

  const std::string message() const { return message_; }
  DiagnosticType type() const { return type_; }
  collections::Range range() const { return range_; }
  collections::Position start() const { return range_.start(); }
  collections::Position end() const { return range_.end(); }

private:
  DiagnosticType type_;
  std::string message_;
  collections::Range range_;
};

class Diagnostics
{
  typedef collections::Range Range;

public:

  void add(DiagnosticType type, const std::string& message, const Range& range)
  {
    diagnostics_.push_back(Diagnostic(type, message, range));
  }

  void addError(const std::string& message, const Range& range)
  {
    add(DIAGNOSTIC_ERROR, message, range);
  }

  void addWarning(const std::string& message, const Range& range)
  {
    add(DIAGNOSTIC_WARNING, message, range);
  }

  void addInfo(const std::string& message, const Range& range)
  {
    add(DIAGNOSTIC_INFO, message, range);
  }

  operator const std::vector<Diagnostic>&() const { return diagnostics_; }

private:
  std::vector<Diagnostic> diagnostics_;
};

namespace detail {

class CheckerBase
{
public:
  typedef tokens::Token Token;
  typedef tokens::TokenType TokenType;
  typedef parser::Node Node;

  virtual void apply(const Node* pNode, Diagnostics* pDiagnostics) const = 0;
  virtual ~CheckerBase() {}
};

class ComparisonWithNullChecker : public detail::CheckerBase
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

    if (pLhs->token().contentsEqual("NULL") ||
        pRhs->token().contentsEqual("NULL"))
    {
      pDiagnostics->addWarning(
        "Use 'is.null()' to check if an object is NULL",
        pNode->range());
    }
  }
};

} // namespace detail

class DiagnosticsSet
{
  typedef std::vector<const detail::CheckerBase*> Checkers;

public:

  void add(detail::CheckerBase* pChecker)
  {
    checkers_.push_back(pChecker);
  }

  void run(const parser::Node* pNode)
  {
    for (Checkers::const_iterator it = checkers_.begin();
         it != checkers_.end();
         ++it)
    {
      (*it)->apply(pNode, &diagnostics_);
    }

    typedef parser::Node::Children Children;
    for (Children::const_iterator it = pNode->children().begin();
         it != pNode->children().end();
         ++it)
    {
      run(*it);
    }
  }

  void report()
  {
    const std::vector<Diagnostic>& diagnostics = diagnostics_;
    for (std::size_t i = 0; i < diagnostics.size(); ++i)
    {
      Diagnostic diagnostic = diagnostics[i];
      std::cerr << diagnostic.range() << ": "
                << diagnostic.message()
                << std::endl;
    }
  }

  ~DiagnosticsSet()
  {
    for (Checkers::const_iterator it = checkers_.begin();
         it != checkers_.end();
         ++it)
    {
      delete *it;
    }
  }

private:
  Checkers checkers_;
  Diagnostics diagnostics_;
};

} // namespace diagnostics
} // namespace sourcetools

#endif /* SOURCE_TOOLS_DIAGNOSTICS_DIAGNOSTICS_H */
