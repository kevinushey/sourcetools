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

  ::Rprintf(toString(pNode->token()).c_str());

  for (auto&& child : pNode->children())
    log(child, depth + 1);
}

namespace {

class SEXPConverter
{
private:

  static SEXP asKeywordSEXP(const tokens::Token& token)
  {
    using namespace tokens;

    switch (token.type())
    {
    case KEYWORD_FALSE:         return Rf_ScalarLogical(0);
    case KEYWORD_TRUE:          return Rf_ScalarLogical(1);
    case KEYWORD_Inf:           return Rf_ScalarReal(INFINITY);
    case KEYWORD_NA:            return Rf_ScalarLogical(NA_LOGICAL);
    case KEYWORD_NA_character_: return NA_STRING;
    // case KEYWORD_NA_complex_:   return NA_COM
    case KEYWORD_NA_integer_:   return Rf_ScalarInteger(NA_INTEGER);
    case KEYWORD_NA_real_:      return Rf_ScalarReal(NA_REAL);
    case KEYWORD_NaN:           return Rf_ScalarReal(R_NaN);
    case KEYWORD_NULL:          return R_NilValue;
    default:                    return Rf_install(token.contents().c_str());
    }
  }

  static SEXP asFunctionCallSEXP(std::shared_ptr<parser::Node> pNode)
  {
    using namespace tokens;
    SEXP langSEXP = PROTECT(R_NilValue);
    for (auto it = pNode->children().rbegin(); it != pNode->children().rend(); ++it)
    {
      auto&& node = *it;
      const Token& token = node->token();
      if (token.isType(EMPTY))
        continue;

      else if (token.isType(tokens::OPERATOR_ASSIGN_LEFT_EQUALS))
      {
        auto&& lhs = node->children()[0];
        auto&& rhs = node->children()[1];

        langSEXP = Rf_lcons(asSEXP(rhs), langSEXP);
        SET_TAG(langSEXP, asSEXP(lhs));
      }
      else
      {
        langSEXP = Rf_lcons(asSEXP(*it), langSEXP);
      }
    }
    UNPROTECT(1);
    return langSEXP;
  }

  static SEXP asFunctionArgumentListSEXP(std::shared_ptr<parser::Node> pNode)
  {
    std::size_t n = pNode->children().size();
    if (n == 0)
      return R_NilValue;

    SEXP listSEXP = PROTECT(Rf_allocList(n));
    SEXP headSEXP = listSEXP;
    for (auto&& child : pNode->children())
    {
      SET_TAG(headSEXP, Rf_install(child->token().contents().c_str()));
      if (child->children().empty())
        SETCAR(headSEXP, R_MissingArg);
      else
        SETCAR(headSEXP, asSEXP(child->children()[0]));
      headSEXP = CDR(headSEXP);
    }
    Rf_PrintValue(listSEXP);

    UNPROTECT(1);
    return listSEXP;
  }

  static SEXP asFunctionDeclSEXP(std::shared_ptr<parser::Node> pNode)
  {
    if (pNode->children().size() != 2)
      return R_NilValue;

    return Rf_lang3(
      Rf_install("function"),
      asFunctionArgumentListSEXP(pNode->children()[0]),
      asSEXP(pNode->children()[1]));
  }

public:
  static SEXP asSEXP(std::shared_ptr<parser::Node> pNode)
  {
    using namespace tokens;

    if (!pNode)
      return R_NilValue;

    auto&& token = pNode->token();

    // Handle function calls specially
    if (pNode->children().size() > 1 && token.isType(LPAREN))
      return asFunctionCallSEXP(pNode);
    else if (token.isType(KEYWORD_FUNCTION))
      return asFunctionDeclSEXP(pNode);

    SEXP elSEXP;
    if (isKeyword(token))
      elSEXP = PROTECT(asKeywordSEXP(token));
    else if (isOperator(token) || isSymbol(token) || isLeftBracket(token))
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

    SEXP listSEXP = PROTECT(R_NilValue);
    for (auto it = pNode->children().rbegin(); it != pNode->children().rend(); ++it)
      if (!(*it)->token().isType(EMPTY))
        listSEXP = Rf_lcons(asSEXP(*it), listSEXP);
    listSEXP = Rf_lcons(elSEXP, listSEXP);

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
  SEXP charSEXP = STRING_ELT(programSEXP, 0);
  sourcetools::parser::Parser parser(CHAR(charSEXP), Rf_length(charSEXP));
  auto root = parser.parse();
  for (auto&& child : root)
    sourcetools::log(child);
  return sourcetools::SEXPConverter::asSEXP(root);
}
