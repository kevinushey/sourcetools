#include <sourcetools.h>

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

namespace sourcetools {
namespace {

void asDataFrame(SEXP listSEXP, int n)
{
  SEXP classSEXP = PROTECT(Rf_mkString("data.frame"));
  Rf_setAttrib(listSEXP, R_ClassSymbol, classSEXP);

  SEXP rownamesSEXP = PROTECT(Rf_allocVector(INTSXP, 2));
  INTEGER(rownamesSEXP)[0] = NA_INTEGER;
  INTEGER(rownamesSEXP)[1] = -n;
  Rf_setAttrib(listSEXP, R_RowNamesSymbol, rownamesSEXP);

  UNPROTECT(2);
}

SEXP asSEXP(const std::vector<tokens::Token>& tokens)
{
  std::size_t n = tokens.size();
  SEXP resultSEXP = PROTECT(Rf_allocVector(VECSXP, 4));

  // Set vector elements
  SEXP valueSEXP = PROTECT(Rf_allocVector(STRSXP, n));
  SET_VECTOR_ELT(resultSEXP, 0, valueSEXP);
  for (std::size_t i = 0; i < n; ++i) {
    std::string contents = tokens[i].contents();
    SEXP charSEXP = PROTECT(Rf_mkChar(contents.c_str()));
    SET_STRING_ELT(valueSEXP, i, charSEXP);
  }
  UNPROTECT(n);

  SEXP rowSEXP = PROTECT(Rf_allocVector(INTSXP, n));
  SET_VECTOR_ELT(resultSEXP, 1, rowSEXP);
  for (std::size_t i = 0; i < n; ++i)
    INTEGER(rowSEXP)[i] = tokens[i].row();

  SEXP columnSEXP = PROTECT(Rf_allocVector(INTSXP, n));
  SET_VECTOR_ELT(resultSEXP, 2, columnSEXP);
  for (std::size_t i = 0; i < n; ++i)
    INTEGER(columnSEXP)[i] = tokens[i].column();

  SEXP typeSEXP = PROTECT(Rf_allocVector(STRSXP, n));
  SET_VECTOR_ELT(resultSEXP, 3, typeSEXP);
  for (std::size_t i = 0; i < n; ++i) {
    std::string type = toString(tokens[i].type());
    SEXP charSEXP = PROTECT(Rf_mkChar(type.c_str()));
    SET_STRING_ELT(typeSEXP, i, charSEXP);
  }
  UNPROTECT(n);

  // Set names
  SEXP namesSEXP = PROTECT(Rf_allocVector(STRSXP, 4));

  SET_STRING_ELT(namesSEXP, 0, Rf_mkChar("value"));
  SET_STRING_ELT(namesSEXP, 1, Rf_mkChar("row"));
  SET_STRING_ELT(namesSEXP, 2, Rf_mkChar("column"));
  SET_STRING_ELT(namesSEXP, 3, Rf_mkChar("type"));

  Rf_setAttrib(resultSEXP, R_NamesSymbol, namesSEXP);

  asDataFrame(resultSEXP, n);

  UNPROTECT(6);
  return resultSEXP;
}

} // anonymous namespace
} // namespace sourcetools

extern "C" SEXP sourcetools_tokenize_file(SEXP absolutePathSEXP)
{
  const char* absolutePath = CHAR(STRING_ELT(absolutePathSEXP, 0));
  std::string contents;
  if (!sourcetools::read(absolutePath, &contents))
  {
    Rf_warning("Failed to read file");
    return R_NilValue;
  }

  const auto& tokens = sourcetools::tokenize(contents);
  return sourcetools::asSEXP(tokens);
}

extern "C" SEXP sourcetools_tokenize_string(SEXP stringSEXP)
{
  std::string string(CHAR(STRING_ELT(stringSEXP, 0)));
  const auto& tokens = sourcetools::tokenize(string);
  return sourcetools::asSEXP(tokens);
}
