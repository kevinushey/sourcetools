#include <sourcetools.h>

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

namespace sourcetools {
namespace {

void asDataFrame(SEXP listSEXP, int n)
{
  r::Protect protect;
  SEXP classSEXP = protect(Rf_mkString("data.frame"));
  Rf_setAttrib(listSEXP, R_ClassSymbol, classSEXP);

  SEXP rownamesSEXP = protect(Rf_allocVector(INTSXP, 2));
  INTEGER(rownamesSEXP)[0] = NA_INTEGER;
  INTEGER(rownamesSEXP)[1] = -n;
  Rf_setAttrib(listSEXP, R_RowNamesSymbol, rownamesSEXP);
}

SEXP asSEXP(const std::vector<tokens::Token>& tokens)
{
  r::Protect protect;
  index_type n = tokens.size();
  SEXP resultSEXP = protect(Rf_allocVector(VECSXP, 4));

  // Set vector elements
  SEXP valueSEXP = protect(Rf_allocVector(STRSXP, n));
  SET_VECTOR_ELT(resultSEXP, 0, valueSEXP);
  for (index_type i = 0; i < n; ++i) {
    const std::string& contents = tokens[i].contents();
    SEXP charSEXP = Rf_mkCharLen(contents.c_str(), contents.size());
    SET_STRING_ELT(valueSEXP, i, charSEXP);
  }

  SEXP rowSEXP = protect(Rf_allocVector(INTSXP, n));
  SET_VECTOR_ELT(resultSEXP, 1, rowSEXP);
  for (index_type i = 0; i < n; ++i)
    INTEGER(rowSEXP)[i] = tokens[i].row() + 1;

  SEXP columnSEXP = protect(Rf_allocVector(INTSXP, n));
  SET_VECTOR_ELT(resultSEXP, 2, columnSEXP);
  for (index_type i = 0; i < n; ++i)
    INTEGER(columnSEXP)[i] = tokens[i].column() + 1;

  SEXP typeSEXP = protect(Rf_allocVector(STRSXP, n));
  SET_VECTOR_ELT(resultSEXP, 3, typeSEXP);
  for (index_type i = 0; i < n; ++i) {
    const std::string& type = toString(tokens[i].type());
    SEXP charSEXP = Rf_mkCharLen(type.c_str(), type.size());
    SET_STRING_ELT(typeSEXP, i, charSEXP);
  }

  // Set names
  SEXP namesSEXP = protect(Rf_allocVector(STRSXP, 4));

  SET_STRING_ELT(namesSEXP, 0, Rf_mkChar("value"));
  SET_STRING_ELT(namesSEXP, 1, Rf_mkChar("row"));
  SET_STRING_ELT(namesSEXP, 2, Rf_mkChar("column"));
  SET_STRING_ELT(namesSEXP, 3, Rf_mkChar("type"));

  Rf_setAttrib(resultSEXP, R_NamesSymbol, namesSEXP);

  asDataFrame(resultSEXP, n);

  return resultSEXP;
}

} // anonymous namespace
} // namespace sourcetools

// [[export(.Call)]]
extern "C" SEXP sourcetools_tokenize_file(SEXP absolutePathSEXP)
{
  typedef sourcetools::tokens::Token Token;

  const char* absolutePath = CHAR(STRING_ELT(absolutePathSEXP, 0));
  std::string contents;
  if (!sourcetools::read(absolutePath, &contents))
  {
    Rf_warning("Failed to read file");
    return R_NilValue;
  }

  if (contents.empty()) return R_NilValue;
  const std::vector<Token>& tokens = sourcetools::tokenize(contents);
  return sourcetools::asSEXP(tokens);
}

// [[export(.Call)]]
extern "C" SEXP sourcetools_tokenize_string(SEXP stringSEXP)
{
  typedef sourcetools::tokens::Token Token;

  if (Rf_length(stringSEXP) == 0)
    return sourcetools::asSEXP(std::vector<Token>());

  SEXP charSEXP = STRING_ELT(stringSEXP, 0);
  const std::vector<Token>& tokens =
    sourcetools::tokenize(CHAR(charSEXP), Rf_length(charSEXP));
  return sourcetools::asSEXP(tokens);
}
