#include <sourcetools.h>

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

namespace sourcetools {

void log(std::shared_ptr<parser::Node> pNode, int depth)
{
  for (std::size_t i = 0; i < depth; ++i)
    ::Rprintf("  ");

  auto&& token = pNode->token();
  ::Rprintf("[%i:%i]: %s\n",
            (int) token.row(),
            (int) token.column(),
            token.contents().c_str());

  for (auto&& child : pNode->children())
    log(child, depth + 1);
}

SEXP asSEXP(std::shared_ptr<parser::Node> pNode)
{
  auto&& token = pNode->token();
  SEXP elSEXP;

  // TODO: Make an appropriate function for turning
  // tokens into SEXP primitive
  if (tokens::isOperator(token))
    elSEXP = PROTECT(Rf_install(token.contents().c_str()));
  else if (tokens::isNumeric(token))
    elSEXP = PROTECT(Rf_ScalarReal(::atof(token.contents().c_str())));
  else
    elSEXP = PROTECT(Rf_mkString(token.contents().c_str()));

  if (pNode->children().empty())
  {
    UNPROTECT(1);
    return elSEXP;
  }

  SEXP listSEXP = PROTECT(Rf_lcons(elSEXP, R_NilValue));
  SEXP tailSEXP = listSEXP;
  for (auto&& child : pNode->children())
  {
    SETCDR(tailSEXP, Rf_lcons(asSEXP(child), R_NilValue));
    tailSEXP = CDR(tailSEXP);
  }

  UNPROTECT(2);
  return listSEXP;
}

} // namespace sourcetools

extern "C" SEXP sourcetools_parse_string(SEXP stringSEXP)
{
  const char* string = CHAR(STRING_ELT(stringSEXP, 0));
  auto root = sourcetools::parse(string);
  // sourcetools::log(root);
  return sourcetools::asSEXP(root);
}
