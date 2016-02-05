#include <sourcetools.h>
using namespace sourcetools;

SEXP asSEXP(const sourcetools::validators::SyntaxError& error)
{
  SEXP result = PROTECT(Rf_allocVector(VECSXP, 3));

  SET_VECTOR_ELT(result, 0, Rf_ScalarInteger(error.row()));
  SET_VECTOR_ELT(result, 1, Rf_ScalarInteger(error.column()));
  SET_VECTOR_ELT(result, 2, Rf_mkString(error.message().c_str()));

  UNPROTECT(1);
  return result;
}

extern "C" SEXP sourcetools_validate_syntax(SEXP contentsSEXP) {

  const char* contents = CHAR(STRING_ELT(contentsSEXP, 0));

  using namespace sourcetools;
  using namespace sourcetools::tokens;
  using namespace sourcetools::validators;

  const auto& tokens = sourcetools::tokenize(contents);
  SyntaxValidator validator(tokens);
  const std::vector<SyntaxError>& errors = validator.errors();
  std::size_t n = errors.size();

  SEXP resultSEXP = PROTECT(Rf_allocVector(VECSXP, 3));
  r::util::setNames(resultSEXP, "row", "column", "error");
  r::util::listToDataFrame(resultSEXP, n);

  UNPROTECT(1);
  return resultSEXP;

}
