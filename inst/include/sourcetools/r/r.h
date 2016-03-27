#ifndef SOURCE_TOOLS_R_R_H
#define SOURCE_TOOLS_R_R_H

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

#include <set>
#include <vector>
#include <string>

#include <sourcetools/core/util.h>

namespace sourcetools {
namespace r {

class Protect : noncopyable
{
public:
  SEXP add(SEXP objectSEXP) { PROTECT(objectSEXP); ++n_; return objectSEXP; }
  Protect() : n_(0) {}
  ~Protect() { UNPROTECT(n_); }

private:
  std::size_t n_;
};

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

inline SEXP eval(const std::string& fn, SEXP envSEXP = NULL)
{
  Protect protect;
  if (envSEXP == NULL)
  {
    SEXP strSEXP = protect.add(Rf_mkString("sourcetools"));
    envSEXP = R_FindNamespace(strSEXP);
  }

  SEXP callSEXP = protect.add(Rf_lang1(Rf_install(fn.c_str())));
  SEXP resultSEXP = protect.add(Rf_eval(callSEXP, envSEXP));
  return resultSEXP;
}

inline std::set<std::string> objectsOnSearchPath()
{
  std::set<std::string> results;
  Protect protect;

  SEXP objectsSEXP;
  protect.add(objectsSEXP = eval("objectsOnSearchPath"));

  for (R_xlen_t i = 0; i < Rf_length(objectsSEXP); ++i)
  {
    SEXP strSEXP = VECTOR_ELT(objectsSEXP, i);
    for (R_xlen_t j = 0; j < Rf_length(strSEXP); ++j)
    {
      SEXP charSEXP = STRING_ELT(strSEXP, j);
      std::string element(CHAR(charSEXP), Rf_length(charSEXP));
      results.insert(element);
    }
  }

  return results;
}

namespace util {

inline void setNames(SEXP dataSEXP, const char** names, std::size_t n)
{
  RObjectFactory factory;
  SEXP namesSEXP = factory.create(STRSXP, n);
  for (std::size_t i = 0; i < n; ++i)
    SET_STRING_ELT(namesSEXP, i, Rf_mkChar(names[i]));
  Rf_setAttrib(dataSEXP, R_NamesSymbol, namesSEXP);
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
