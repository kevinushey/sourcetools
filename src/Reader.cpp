#include <parsr.h>

extern "C" SEXP parsr_read(SEXP absolutePathSEXP)
{
  const char* absolutePath = CHAR(STRING_ELT(absolutePathSEXP, 0));

  std::string contents;
  bool result = parsr::read(absolutePath, &contents);
  if (!result)
  {
    Rf_warning("Failed to read file");
    return R_NilValue;
  }

  SEXP resultSEXP;
  PROTECT(resultSEXP = Rf_allocVector(STRSXP, 1));
  SET_STRING_ELT(resultSEXP, 0, Rf_mkChar(contents.c_str()));
  UNPROTECT(1);
  return resultSEXP;
}
