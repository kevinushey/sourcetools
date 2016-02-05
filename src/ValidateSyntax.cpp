#include <sourcetools.h>
using namespace sourcetools;

extern "C" SEXP sourcetools_validate_syntax(SEXP contentsSEXP) {

  const char* contents = CHAR(STRING_ELT(contentsSEXP, 0));

  using namespace sourcetools;
  using namespace sourcetools::tokens;
  using namespace sourcetools::validators;

  const auto& tokens = sourcetools::tokenize(contents);
  SyntaxValidator validator(tokens);
  const std::vector<SyntaxError>& errors = validator.errors();
  std::size_t n = errors.size();

  r::RObjectFactory factory;
  SEXP resultSEXP = factory.create(VECSXP, 3);

  auto rowFn = [](SEXP dataSEXP, std::size_t i, const SyntaxError& error)
  {
    INTEGER(dataSEXP)[i] = error.row() + 1;
  };
  SET_VECTOR_ELT(resultSEXP, 0, factory.create(INTSXP, errors, rowFn));

  auto colFn = [](SEXP dataSEXP, std::size_t i, const SyntaxError& error)
  {
    INTEGER(dataSEXP)[i] = error.column() + 1;
  };
  SET_VECTOR_ELT(resultSEXP, 1, factory.create(INTSXP, errors, colFn));

  auto errFn = [](SEXP dataSEXP, std::size_t i, const SyntaxError& error)
  {
    const std::string& msg = error.message();
    SET_STRING_ELT(dataSEXP, i, Rf_mkCharLen(msg.c_str(), msg.size()));
  };
  SET_VECTOR_ELT(resultSEXP, 2, factory.create(STRSXP, errors, errFn));

  r::util::setNames(resultSEXP, "row", "column", "error");
  r::util::listToDataFrame(resultSEXP, n);

  return resultSEXP;
}
