#include <sourcetools.h>

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

namespace sourcetools {

void log(parser::ParseNode* pNode, int depth)
{
  if (!pNode)
    return;

  for (int i = 0; i < depth; ++i)
    Rprintf("  ");

  Rprintf(toString(pNode->token()).c_str());

  using parser::ParseNode;
  const std::vector<ParseNode*>& children = pNode->children();
  for (std::vector<ParseNode*>::const_iterator it = children.begin();
       it != children.end();
       ++it)
  {
    log(*it, depth + 1);
  }
}

namespace {

class SEXPConverter
{
private:
  typedef parser::ParseNode ParseNode;

  static SEXP asKeywordSEXP(const tokens::Token& token)
  {
    using namespace tokens;

    switch (token.type())
    {
    case KEYWORD_FALSE:         return Rf_ScalarLogical(0);
    case KEYWORD_TRUE:          return Rf_ScalarLogical(1);
    case KEYWORD_Inf:           return Rf_ScalarReal(R_PosInf);
    case KEYWORD_NA:            return Rf_ScalarLogical(NA_LOGICAL);
    case KEYWORD_NA_character_: return Rf_ScalarString(NA_STRING);
    // case KEYWORD_NA_complex_:   return NA_COM
    case KEYWORD_NA_integer_:   return Rf_ScalarInteger(NA_INTEGER);
    case KEYWORD_NA_real_:      return Rf_ScalarReal(NA_REAL);
    case KEYWORD_NaN:           return Rf_ScalarReal(R_NaN);
    case KEYWORD_NULL:          return R_NilValue;
    default:                    return Rf_install(token.contents().c_str());
    }
  }

  static SEXP asFunctionCallSEXP(const ParseNode* pNode)
  {
    using namespace tokens;

    const Token& token = pNode->token();

    // Figure out the 'head' of this language object.
    // '[' and '[[' get these tokens as-is, while '('
    // instead uses the name of the first child.
    SEXP langSEXP;
    if (token.isType(LBRACKET))
      langSEXP = Rf_lang1(Rf_install("["));
    else if (token.isType(LDBRACKET))
      langSEXP = Rf_lang1(Rf_install("[["));
    else
      langSEXP = Rf_lang1(R_NilValue);

    // Start appending the child nodes to our list.
    r::Protect protect;
    SEXP headSEXP = protect(langSEXP);
    for (std::vector<ParseNode*>::const_iterator it = pNode->children().begin();
         it != pNode->children().end();
         ++it)
    {
      const ParseNode* node = *it;
      const Token& token = node->token();
      if (token.isType(EMPTY))
        break;
      else if (token.isType(MISSING))
        SETCDR(langSEXP, Rf_lang1(R_MissingArg));

      else if (token.isType(tokens::OPERATOR_ASSIGN_LEFT_EQUALS))
      {
        const ParseNode* lhs = node->children()[0];
        const ParseNode* rhs = node->children()[1];

        if (rhs->token().isType(MISSING))
          SETCDR(langSEXP, Rf_lang1(R_MissingArg));
        else
          SETCDR(langSEXP, Rf_lang1(asSEXP(rhs)));

        const Token& token = lhs->token();
        SEXP nameSEXP = Rf_install(tokens::stringValue(token).c_str());
        SET_TAG(CDR(langSEXP), nameSEXP);
      }
      else
      {
        SETCDR(langSEXP, Rf_lang1(asSEXP(node)));
      }

      langSEXP = CDR(langSEXP);
    }

    SEXP resultSEXP = CAR(headSEXP) == R_NilValue
      ? CDR(headSEXP)
      : headSEXP;

    // Convert strings to symbols at head position
    if (TYPEOF(CAR(resultSEXP)) == STRSXP)
      SETCAR(resultSEXP, Rf_install(CHAR(STRING_ELT(CAR(resultSEXP), 0))));

    return resultSEXP;
  }

  static SEXP asFunctionArgumentListSEXP(const ParseNode* pNode)
  {
    index_type n = pNode->children().size();
    if (n == 0)
      return R_NilValue;

    r::Protect protect;
    SEXP listSEXP = protect(Rf_allocList(n));
    SEXP headSEXP = listSEXP;
    for (std::vector<ParseNode*>::const_iterator it = pNode->children().begin();
         it != pNode->children().end();
         ++it)
    {
      const ParseNode* pChild = *it;
      const tokens::Token& token = pChild->token();

      if (tokens::isOperator(token))
      {
        const ParseNode* pLhs = pChild->children()[0];
        const ParseNode* pRhs = pChild->children()[1];

        if (pLhs->token().isType(tokens::SYMBOL))
          SET_TAG(headSEXP, Rf_install(tokens::stringValue(pLhs->token()).c_str()));
        SETCAR(headSEXP, asSEXP(pRhs));
      }
      else if (token.isType(tokens::SYMBOL))
      {
        SETCAR(headSEXP, R_MissingArg);
        SET_TAG(headSEXP, Rf_install(tokens::stringValue(token).c_str()));
      }

      headSEXP = CDR(headSEXP);
    }

    return listSEXP;
  }

  static SEXP asFunctionDeclSEXP(const ParseNode* pNode)
  {
    if (pNode->children().size() != 2)
      return R_NilValue;

    r::Protect protect;
    SEXP argsSEXP = protect(asFunctionArgumentListSEXP(pNode->children()[0]));
    SEXP bodySEXP = protect(asSEXP(pNode->children()[1]));
    SEXP fnSEXP = Rf_install("function");
    SEXP resultSEXP = Rf_lang4(fnSEXP, argsSEXP, bodySEXP, R_NilValue);
    return resultSEXP;
  }

  static SEXP asNumericSEXP(const tokens::Token& token)
  {
    if (*(token.end() - 1) == 'L')
      return Rf_ScalarInteger(::atof(token.begin()));
    else
      return Rf_ScalarReal(::atof(token.begin()));
  }

  static bool isFunctionCall(const ParseNode* pNode)
  {
    const tokens::Token& token = pNode->token();
    if (token.isType(tokens::LBRACKET) || token.isType(tokens::LDBRACKET))
      return true;

    if (!token.isType(tokens::LPAREN))
      return false;

    // Differentiate between '(a, b)' and 'a(b)' by looking at
    // the token positions. Not great, I know...
    const ParseNode* child = pNode->children()[0];
    return child->token().begin() < token.begin();
  }

public:
  static SEXP asSEXP(const ParseNode* pNode)
  {
    using namespace tokens;

    if (!pNode)
      return R_NilValue;

    if (pNode->token().isType(tokens::ROOT))
    {
      const std::vector<ParseNode*>& children = pNode->children();
      index_type n = pNode->children().size();
      r::Protect protect;
      SEXP exprSEXP = protect(Rf_allocVector(EXPRSXP, n));
      for (index_type i = 0; i < n; ++i)
        SET_VECTOR_ELT(exprSEXP, i, asSEXP(children[i]));
      return exprSEXP;
    }

    // Handle function calls specially
    if (isFunctionCall(pNode))
      return asFunctionCallSEXP(pNode);

    const tokens::Token& token = pNode->token();
    if (token.isType(KEYWORD_FUNCTION))
      return asFunctionDeclSEXP(pNode);

    SEXP elSEXP;
    r::Protect protect;
    if (token.isType(MISSING))
      elSEXP = R_MissingArg;
    else if (token.isType(OPERATOR_EXPONENTATION_STARS))
      elSEXP = Rf_install("^");
    else if (token.isType(KEYWORD_BREAK))
      elSEXP = Rf_lang1(Rf_install("break"));
    else if (token.isType(KEYWORD_NEXT))
      elSEXP = Rf_lang1(Rf_install("next"));
    else if (isKeyword(token))
      elSEXP = asKeywordSEXP(token);
    else if (isOperator(token) || isLeftBracket(token))
      elSEXP = Rf_install(token.contents().c_str());
    else if (isNumeric(token))
      elSEXP = asNumericSEXP(token);
    else if (isSymbol(token))
      elSEXP = Rf_install(tokens::stringValue(token).c_str());
    else if (isString(token))
      elSEXP = Rf_mkString(tokens::stringValue(token).c_str());
    else
      elSEXP = Rf_mkString(token.contents().c_str());

    if (pNode->children().empty())
      return elSEXP;

    SEXP headSEXP = protect(Rf_lang1(protect(elSEXP)));
    SEXP listSEXP = headSEXP;
    for (std::vector<ParseNode*>::const_iterator it = pNode->children().begin();
         it != pNode->children().end();
         ++it)
    {
      const ParseNode* child = *it;
      if (!child->token().isType(EMPTY))
        listSEXP = SETCDR(listSEXP, Rf_lang1(asSEXP(child)));
    }

    return headSEXP;
  }

  static SEXP asSEXP(const std::vector<ParseNode*>& expression)
  {
    index_type n = expression.size();
    r::Protect protect;
    SEXP exprSEXP = protect(Rf_allocVector(EXPRSXP, n));
    for (index_type i = 0; i < n; ++i)
      SET_VECTOR_ELT(exprSEXP, i, asSEXP(expression[i]));
    return exprSEXP;
  }

};

void reportErrors(const std::vector<parser::ParseError>& errors)
{
  if (errors.empty())
    return;

  std::stringstream ss;
  ss << "\n  ";
  typedef std::vector<parser::ParseError>::const_iterator Iterator;
  for (Iterator it = errors.begin();
       it != errors.end();
       ++it)
  {
    ss << "[" << it->start().row << ":" << it->start().column << "]: "
       << it->message() << std::endl << "  ";
  }

  Rf_warning(ss.str().c_str());
}

} // anonymous namespace
} // namespace sourcetools

// [[export(.Call)]]
extern "C" SEXP sourcetools_parse_string(SEXP programSEXP)
{
  using namespace sourcetools;
  using parser::ParseStatus;
  using parser::Parser;
  using parser::ParseNode;

  SEXP charSEXP = STRING_ELT(programSEXP, 0);
  Parser parser(CHAR(charSEXP), Rf_length(charSEXP));

  ParseStatus status;
  scoped_ptr<ParseNode> pRoot(parser.parse(&status));

  sourcetools::reportErrors(status.getErrors());

  return sourcetools::SEXPConverter::asSEXP(pRoot);
}

// [[export(.Call)]]
extern "C" SEXP sourcetools_diagnose_string(SEXP strSEXP)
{
  using namespace sourcetools;
  using parser::Parser;
  using parser::ParseStatus;
  using parser::ParseNode;
  using r::Protect;

  SEXP charSEXP = STRING_ELT(strSEXP, 0);
  Parser parser(CHAR(charSEXP), Rf_length(charSEXP));

  ParseStatus status;
  scoped_ptr<ParseNode> pNode(parser.parse(&status));

  using namespace diagnostics;
  scoped_ptr<DiagnosticsSet> pDiagnostics(createDefaultDiagnosticsSet());
  std::vector<Diagnostic> diagnostics = pDiagnostics->run(pNode);
  return r::create(diagnostics);
}
