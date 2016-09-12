#include <sourcetools.h>
#include <sourcetools/tests/testthat.h>

using namespace sourcetools;
using namespace sourcetools::cursors;

typedef sourcetools::tokens::Token Token;

namespace {

class OpenBracketLocator
{
public:
  inline bool operator()(TokenCursor* pCursor) const {

    if (pCursor->bwdToMatchingBracket())
      return false;

    return tokens::isLeftBracket(pCursor->currentToken());
  }
};

} // anonymous namespace

context("Tokenizer") {

  test_that("Complements are detected correctly") {

    using namespace sourcetools::tokens;

    expect_true(complement(LPAREN)    == RPAREN);
    expect_true(complement(LBRACE)    == RBRACE);
    expect_true(complement(LBRACKET)  == RBRACKET);
    expect_true(complement(LDBRACKET) == RDBRACKET);

    expect_true(complement(RPAREN)    == LPAREN);
    expect_true(complement(RBRACE)    == LBRACE);
    expect_true(complement(RBRACKET)  == LBRACKET);
    expect_true(complement(RDBRACKET) == LDBRACKET);

    expect_true(isComplement(LPAREN,    RPAREN));
    expect_true(isComplement(LBRACE,    RBRACE));
    expect_true(isComplement(LBRACKET,  RBRACKET));
    expect_true(isComplement(LDBRACKET, RDBRACKET));

    expect_true(isComplement(RPAREN,    LPAREN));
    expect_true(isComplement(RBRACE,    LBRACE));
    expect_true(isComplement(RBRACKET,  LBRACKET));
    expect_true(isComplement(RDBRACKET, LDBRACKET));
  }

  test_that("Keywords are detected correctly") {
    std::string code = "if for while break repeat";
    const std::vector<Token>& tokens = sourcetools::tokenize(code);
    for (std::vector<Token>::const_iterator it = tokens.begin();
         it != tokens.end();
         ++it)
    {
      const Token& token = *it;
      if (isWhitespace(token))
        continue;
      expect_true(isKeyword(token));

    }
  }

  test_that("TokenCursor operations work as expected") {
    std::string code = "if (foo) { print(bar) } else {}";
    const std::vector<Token>& tokens = sourcetools::tokenize(code);
    TokenCursor cursor(tokens);
    expect_true(cursor.currentToken().contentsEqual("if"));
    cursor.moveToNextSignificantToken();
    expect_true(cursor.currentToken().contentsEqual("("));
    expect_true(cursor.fwdToMatchingBracket());
    expect_true(cursor.currentToken().contentsEqual(")"));
  }

  test_that("Move to position works as expected") {
    std::string code = "if (foo) { print(1) }";
    const std::vector<Token>& tokens = sourcetools::tokenize(code);
    TokenCursor cursor(tokens);

    // move to 'if'
    expect_true(cursor.moveToPosition(0, 0));
    expect_true(cursor.isType(tokens::KEYWORD_IF));

    // move to whitespace before print
    expect_true(cursor.moveToPosition(0, 10));
    expect_true(cursor.currentToken().contentsEqual(" "));

    // move to 'print'
    expect_true(cursor.moveToPosition(0, 11));
    expect_true(cursor.currentToken().contentsEqual("print"));

    // move to 'print' but target in middle
    expect_true(cursor.moveToPosition(0, 12));
    expect_true(cursor.currentToken().contentsEqual("print"));

    expect_true(cursor.moveToPosition(0, 13));
    expect_true(cursor.currentToken().contentsEqual("print"));

    expect_true(cursor.moveToPosition(0, 14));
    expect_true(cursor.currentToken().contentsEqual("print"));

    expect_true(cursor.moveToPosition(0, 15));
    expect_true(cursor.currentToken().contentsEqual("print"));

    // move to '('
    expect_true(cursor.moveToPosition(0, 16));
    expect_true(cursor.currentToken().contentsEqual("("));
  }

  test_that("find operations work")
  {
    std::string code = "(if (foo) { print(1) })";
    const std::vector<Token>& tokens = sourcetools::tokenize(code);
    TokenCursor cursor(tokens);

    OpenBracketLocator locator;
    expect_true(cursor.moveToPosition(0, 13));
    expect_true(cursor.currentToken().contentsEqual("print"));
    expect_true(cursor.findBwd(locator));
    expect_true(cursor.currentToken().contentsEqual("{"));
    expect_true(cursor.fwdToMatchingBracket());
    expect_true(cursor.currentToken().contentsEqual("}"));
    expect_true(cursor.findBwd(locator));
    expect_true(cursor.currentToken().contentsEqual("("));
  }
}
