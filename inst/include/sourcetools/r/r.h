#ifndef SOURCE_TOOLS_R_R_H
#define SOURCE_TOOLS_R_R_H

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

#include <vector>
#include <string>
#include <sourcetools/core/util.h>

namespace sourcetools {
namespace r {

class RObjectFactory : noncopyable
{
public:

  RObjectFactory()
    : n_(0)
  {
  }

  template <typename T, typename F>
  SEXP create(SEXPTYPE type, const std::vector<T>& vector, F f)
  {
    n_ += 1;
    std::size_t n = vector.size();
    SEXP resultSEXP = PROTECT(Rf_allocVector(type, n));
    for (std::size_t i = 0; i < n; ++i)
      f(resultSEXP, i, vector[i]);
    return resultSEXP;
  }

  SEXP create(SEXPTYPE type, std::size_t n)
  {
    n_ += 1;
    return PROTECT(Rf_allocVector(type, n));
  }

  ~RObjectFactory()
  {
    UNPROTECT(n_);
  }

private:
  std::size_t n_;
};

namespace util {

inline void setNames(SEXP dataSEXP, const char** names, std::size_t n)
{
  RObjectFactory factory;
  SEXP namesSEXP = factory.create(STRSXP, n);
  for (std::size_t i = 0; i < n; ++i)
    SET_STRING_ELT(namesSEXP, i, Rf_mkChar(names[i]));
}

inline void listToDataFrame(SEXP listSEXP, int n)
{
  SEXP classSEXP = PROTECT(Rf_mkString("data.frame"));
  Rf_setAttrib(listSEXP, R_ClassSymbol, classSEXP);

  SEXP rownamesSEXP = PROTECT(Rf_allocVector(INTSXP, 2));
  INTEGER(rownamesSEXP)[0] = NA_INTEGER;
  INTEGER(rownamesSEXP)[1] = -n;
  Rf_setAttrib(listSEXP, R_RowNamesSymbol, rownamesSEXP);

  UNPROTECT(2);
}

} // namespace util
} // namespace r
} // namespace sourcetools

#endif /* SOURCE_TOOLS_R_R_H */
