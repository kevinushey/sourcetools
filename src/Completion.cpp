#include <sourcetools.h>

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

namespace sourcetools {

} // namespace sourcetools

extern "C" SEXP sourcetools_code_complete_at(SEXP absolutePathSEXP,
                                             SEXP positionSEXP)
{
  // read contents of file
  const char* absolutePath = CHAR(STRING_ELT(absolutePathSEXP, 0));
  std::string contents;
  if (!sourcetools::read(absolutePath, &contents))
  {
    Rf_warning("Failed to read file at path: '%s'\n", absolutePath);
    return R_NilValue;
  }

  // TODO
  return R_NilValue;
}
