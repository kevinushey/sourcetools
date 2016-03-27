#include <sourcetools.h>
using namespace sourcetools;
using namespace sourcetools::r;

extern "C" SEXP sourcetools_performs_nse(SEXP fnSEXP)
{
  if (TYPEOF(fnSEXP) == VECSXP || TYPEOF(fnSEXP) == EXPRSXP)
  {
    Protect protect;
    std::size_t n = Rf_length(fnSEXP);
    SEXP resultSEXP = protect(Rf_allocVector(LGLSXP, n));
    for (std::size_t i = 0; i < n; ++i)
    {
      SEXP elSEXP = VECTOR_ELT(fnSEXP, i);
      LOGICAL(resultSEXP)[i] = Rf_isFunction(elSEXP)
        ? nse::performsNonStandardEvaluation(elSEXP)
        : 0;
    }
    return resultSEXP;
  }

  bool result = Rf_isFunction(fnSEXP)
    ? nse::performsNonStandardEvaluation(fnSEXP)
    : false;

  return Rf_ScalarLogical(result);
}
