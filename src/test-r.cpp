#include "tests.h"
#include <sourcetools.h>

using namespace sourcetools;

class StringRecorder : public r::CallRecurser::Operation
{
public:

  virtual void apply(SEXP dataSEXP)
  {
    if (TYPEOF(dataSEXP) == STRSXP)
      strings_.insert(CHAR(STRING_ELT(dataSEXP, 0)));
  }

  const std::set<std::string>& strings() const
  {
    return strings_;
  }

private:
  std::set<std::string> strings_;
};

context("CallRecurser")
{
  test_that("The R call recurser works")
  {
    SEXP fnSEXP   = Rf_findFun(Rf_install("all.equal"), R_BaseNamespace);
    SEXP bodySEXP = r::util::functionBody(fnSEXP);

    scoped_ptr<StringRecorder> recorder(new StringRecorder);

    r::CallRecurser recurser(bodySEXP);
    recurser.add(recorder);
    recurser.run();

    const std::set<std::string>& discoveries = recorder->strings();

    expect_true(discoveries.size() == 1);
    expect_true(discoveries.count("all.equal"));
  }

  test_that("Functions which perform non-standard evaluation are detected")
  {
    SEXP fnSEXP;
    fnSEXP = Rf_findFun(Rf_install("library"), R_BaseNamespace);
    expect_true(r::nse::performsNonStandardEvaluation(fnSEXP));

    fnSEXP = Rf_findFun(Rf_install(".gtn"), R_BaseNamespace);
    expect_false(r::nse::performsNonStandardEvaluation(fnSEXP));
  }
}
