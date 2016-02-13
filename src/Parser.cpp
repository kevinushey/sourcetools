#include <sourcetools.h>

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

namespace sourcetools {

void log(std::shared_ptr<parser::Node> pNode, int depth)
{
  if (!pNode)
    return;

  for (int i = 0; i < depth; ++i)
    ::Rprintf("  ");

  auto&& token = pNode->token();
  ::Rprintf("[%i:%i]: %s\n",
            (int) token.row(),
            (int) token.column(),
            token.contents().c_str());

  for (auto&& child : pNode->children())
    log(child, depth + 1);
}

namespace {

class SEXPConverter
{
private:

  static SEXP asFunctionCallSEXP(std::shared_ptr<parser::Node> pNode)
  {
    return Rf_install("lol");
  }

  static SEXP asFunctionDeclSEXP(std::shared_ptr<parser::Node> pNode)
  {
    return R_NilValue;
  }

public:
  static SEXP asSEXP(std::shared_ptr<parser::Node> pNode)
  {
    using namespace tokens;

    if (!pNode)
      return R_NilValue;

    auto&& token = pNode->token();
    SEXP elSEXP;

    // Handle function calls specially
    if (pNode->children().size() > 1 && (token.isType(LPAREN) || token.isType(LBRACKET) || token.isType(LDBRACKET)))
      return asFunctionCallSEXP(pNode);
    else if (token.isType(KEYWORD_FUNCTION))
      return asFunctionDeclSEXP(pNode);

    // TODO: Make an appropriate function for turning
    // tokens into SEXP primitive
    if (isOperator(token) || isSymbol(token) || isKeyword(token) ||
        isLeftBracket(token))
      elSEXP = PROTECT(Rf_install(token.contents().c_str()));
    else if (isNumeric(token))
      elSEXP = PROTECT(Rf_ScalarReal(::atof(token.contents().c_str())));
    else if (isString(token))
    {
      const std::string& contents = token.contents();
      std::string substring = contents.substr(1, contents.size() - 2);
      elSEXP = PROTECT(Rf_mkString(substring.c_str()));
    }
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

  static SEXP asSEXP(const std::vector<std::shared_ptr<parser::Node>>& expression)
  {
    std::size_t n = expression.size();
    SEXP exprSEXP = PROTECT(Rf_allocVector(EXPRSXP, n));
    for (std::size_t i = 0; i < n; ++i)
      SET_VECTOR_ELT(exprSEXP, i, asSEXP(expression[i]));
    UNPROTECT(1);
    return exprSEXP;
  }

};

} // anonymous namespace

} // namespace sourcetools

extern "C" SEXP sourcetools_parse_string(SEXP programSEXP)
{
  const char* program = CHAR(STRING_ELT(programSEXP, 0));
  sourcetools::parser::Parser parser(program);
  auto root = parser.parse();
  return sourcetools::SEXPConverter::asSEXP(root);
}
