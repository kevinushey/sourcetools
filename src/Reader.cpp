#include <sourcetools/read/read.h>
#include <sourcetools/r/r.h>

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

extern "C" SEXP sourcetools_read(SEXP absolutePathSEXP)
{
  const char* absolutePath = CHAR(STRING_ELT(absolutePathSEXP, 0));

  std::string contents;
  bool result = sourcetools::read(absolutePath, &contents);
  if (!result)
  {
    Rf_warning("Failed to read file");
    return R_NilValue;
  }

  sourcetools::r::Protect protect;
  SEXP resultSEXP = protect(Rf_allocVector(STRSXP, 1));
  SET_STRING_ELT(resultSEXP, 0, Rf_mkCharLen(contents.c_str(), contents.size()));
  return resultSEXP;
}
