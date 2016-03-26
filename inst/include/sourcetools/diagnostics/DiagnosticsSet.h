#ifndef SOURCE_TOOLS_DIAGNOSTICS_DIAGNOSTICS_SET_H
#define SOURCE_TOOLS_DIAGNOSTICS_DIAGNOSTICS_SET_H

#include <sourcetools/diagnostics/Diagnostic.h>
#include <sourcetools/diagnostics/Checkers.h>

namespace sourcetools {
namespace diagnostics {

class DiagnosticsSet
{
  typedef std::vector<const checkers::CheckerBase*> Checkers;
  typedef checkers::CheckerBase CheckerBase;

public:

  void add(CheckerBase* pChecker)
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

inline DiagnosticsSet* createDefaultDiagnosticsSet()
{
  DiagnosticsSet* pSet = new DiagnosticsSet();
  pSet->add(new checkers::AssignmentInIfChecker);
  pSet->add(new checkers::ComparisonWithNullChecker);
  pSet->add(new checkers::ScalarOpsInIfChecker);
  pSet->add(new checkers::UnusedResultChecker);
  return pSet;
}

} // namespace diagnostics
} // namespace sourcetools

#endif /* SOURCE_TOOLS_DIAGNOSTICS_DIAGNOSTICS_SET_H */

