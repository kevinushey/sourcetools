#ifndef SOURCE_TOOLS_R_CALL_RECURSER_H
#define SOURCE_TOOLS_R_CALL_RECURSER_H

#include <vector>

#include <sourcetools/core/core.h>
#include <sourcetools/r/RHeaders.h>

namespace sourcetools {
namespace r {

class CallRecurser : noncopyable
{
public:

  class Operation
  {
  public:
    virtual void apply(SEXP dataSEXP) = 0;
    virtual ~Operation() {}
  };

  explicit CallRecurser(SEXP dataSEXP)
    : dataSEXP_(dataSEXP)
  {
  }

  void add(Operation* pOperation)
  {
    operations_.push_back(pOperation);
  }

  void run()
  {
    runImpl(dataSEXP_);
  }

  void runImpl(SEXP dataSEXP)
  {
    for (std::vector<Operation*>::iterator it = operations_.begin();
         it != operations_.end();
         ++it)
    {
      (*it)->apply(dataSEXP);
    }

    if (TYPEOF(dataSEXP) == LANGSXP)
    {
      while (dataSEXP != R_NilValue)
      {
        runImpl(CAR(dataSEXP));
        dataSEXP = CDR(dataSEXP);
      }
    }
  }

private:
  SEXP dataSEXP_;
  std::vector<Operation*> operations_;
};

} // namespace r
} // namespace sourcetools

#endif /* SOURCE_TOOLS_R_CALL_RECURSER_H */
