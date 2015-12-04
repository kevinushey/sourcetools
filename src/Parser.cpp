#include <sourcetools.h>

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

extern "C" SEXP sourcetools_parse_string(SEXP stringSEXP)
{
  const char* string = CHAR(STRING_ELT(stringSEXP, 0));
  auto root = sourcetools::parse(string);
  return R_NilValue;
}
