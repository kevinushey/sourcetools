#include <sourcetools.h>
using namespace sourcetools;

namespace {

typedef sourcetools::validators::SyntaxError Error;
struct RowSetter
{
  void operator()(SEXP dataSEXP, std::size_t i, const Error& error)
  {
    INTEGER(dataSEXP)[i] = error.row() + 1;
  }
};

struct ColSetter
{
  void operator()(SEXP dataSEXP, std::size_t i, const Error& error)
  {
    INTEGER(dataSEXP)[i] = error.column() + 1;
  }
};

struct ErrSetter
{
  void operator()(SEXP dataSEXP, std::size_t i, const Error& error)
  {
    const std::string& msg = error.message();
    SET_STRING_ELT(dataSEXP, i, Rf_mkCharLen(msg.c_str(), msg.size()));
  }
};

} // anonymous namespace

extern "C" SEXP sourcetools_validate_syntax(SEXP contentsSEXP) {

  const char* contents = CHAR(STRING_ELT(contentsSEXP, 0));

  using namespace sourcetools;
  using namespace sourcetools::tokens;
  using namespace sourcetools::validators;

  const std::vector<tokens::Token>& tokens = sourcetools::tokenize(contents);
  SyntaxValidator validator(tokens);
  const std::vector<SyntaxError>& errors = validator.errors();
  std::size_t n = errors.size();

  r::RObjectFactory factory;
  SEXP resultSEXP = factory.create(VECSXP, 3);
  SET_VECTOR_ELT(resultSEXP, 0, factory.create(INTSXP, errors, RowSetter()));
  SET_VECTOR_ELT(resultSEXP, 1, factory.create(INTSXP, errors, ColSetter()));
  SET_VECTOR_ELT(resultSEXP, 2, factory.create(STRSXP, errors, ErrSetter()));

  const char* names[] = {"row", "column", "error"};
  r::util::setNames(resultSEXP, names, 3);
  r::util::listToDataFrame(resultSEXP, n);

  return resultSEXP;
}
