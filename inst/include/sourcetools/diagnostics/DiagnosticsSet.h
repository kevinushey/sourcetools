#ifndef SOURCETOOLS_DIAGNOSTICS_DIAGNOSTICS_SET_H
#define SOURCETOOLS_DIAGNOSTICS_DIAGNOSTICS_SET_H

#include <sourcetools/diagnostics/Diagnostic.h>
#include <sourcetools/diagnostics/Checkers.h>

namespace sourcetools {
namespace diagnostics {

class DiagnosticsSet
{
  typedef std::vector<checkers::CheckerBase*> Checkers;
  typedef checkers::CheckerBase CheckerBase;
  typedef parser::ParseNode ParseNode;

public:

  void add(CheckerBase* pChecker)
  {
    checkers_.push_back(pChecker);
  }

  const std::vector<Diagnostic>& run(const ParseNode* pNode)
  {
    runImpl(pNode);
    return diagnostics_;
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
  void runImpl(const ParseNode* pNode, std::size_t depth = 0)
  {
    for (Checkers::iterator it = checkers_.begin();
         it != checkers_.end();
         ++it)
    {
      (*it)->apply(pNode, &diagnostics_, depth);
    }

    for (std::vector<ParseNode*>::const_iterator it = pNode->children().begin();
         it != pNode->children().end();
         ++it)
    {
      runImpl(*it, depth + 1);
    }
  }


private:
  Checkers checkers_;
  Diagnostics diagnostics_;
};

inline DiagnosticsSet* createDefaultDiagnosticsSet()
{
  DiagnosticsSet* pSet = new DiagnosticsSet();
  pSet->add(new checkers::AssignmentInIfChecker);
  pSet->add(new checkers::ComparisonWithNullChecker);
  pSet->add(new checkers::ScalarOpsInIfChecker);
  pSet->add(new checkers::UnusedResultChecker);
  pSet->add(new checkers::NoSymbolInScopeChecker);
  return pSet;
}

} // namespace diagnostics
} // namespace sourcetools

#endif /* SOURCETOOLS_DIAGNOSTICS_DIAGNOSTICS_SET_H */
