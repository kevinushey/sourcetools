#include <parsr.h>

namespace {

SEXP asSEXP(const parsr::tokens::Token& token)
{
  SEXP tokenSEXP;
  PROTECT(tokenSEXP = Rf_allocVector(VECSXP, 4));
  SET_VECTOR_ELT(tokenSEXP, 0, Rf_mkString(token.content().c_str()));
  SET_VECTOR_ELT(tokenSEXP, 1, Rf_ScalarInteger(token.row()));
  SET_VECTOR_ELT(tokenSEXP, 2, Rf_ScalarInteger(token.column()));

  std::string typeString = parsr::toString(token.type());
  SET_VECTOR_ELT(tokenSEXP, 3, Rf_mkString(typeString.c_str()));

  SEXP namesSEXP;
  PROTECT(namesSEXP = Rf_allocVector(STRSXP, 4));

  SET_STRING_ELT(namesSEXP, 0, Rf_mkChar("value"));
  SET_STRING_ELT(namesSEXP, 1, Rf_mkChar("row"));
  SET_STRING_ELT(namesSEXP, 2, Rf_mkChar("column"));
  SET_STRING_ELT(namesSEXP, 3, Rf_mkChar("type"));

  Rf_setAttrib(tokenSEXP, R_NamesSymbol, namesSEXP);

  UNPROTECT(2);
  return tokenSEXP;
}

SEXP asSEXP(const std::vector<parsr::tokens::Token>& tokens)
{
  SEXP resultSEXP;
  std::size_t n = tokens.size();
  PROTECT(resultSEXP = Rf_allocVector(VECSXP, n));
  for (std::size_t i = 0; i < n; ++i)
    SET_VECTOR_ELT(resultSEXP, i, asSEXP(tokens[i]));
  UNPROTECT(1);
  Rf_setAttrib(resultSEXP, R_ClassSymbol, Rf_mkString("RTokens"));
  return resultSEXP;
}

} // anonymous namespace

extern "C" SEXP parsr_tokenize_file(SEXP absolutePathSEXP)
{
  const char* absolutePath = CHAR(STRING_ELT(absolutePathSEXP, 0));
  std::string contents;
  if (!parsr::read(absolutePath, &contents))
  {
    Rf_warning("Failed to read file");
    return R_NilValue;
  }

  parsr::Tokenizer tokenizer(contents);
  auto& tokens = tokenizer.tokens();
  return asSEXP(tokens);
}

extern "C" SEXP parsr_tokenize_string(SEXP stringSEXP)
{
  const char* string = CHAR(STRING_ELT(stringSEXP, 0));
  parsr::Tokenizer tokenizer(string);
  auto& tokens = tokenizer.tokens();
  return asSEXP(tokens);
}
