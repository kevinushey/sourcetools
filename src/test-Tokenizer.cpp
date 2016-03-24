#include <testthat.h>
#include <sourcetools.h>

using namespace sourcetools;
using namespace sourcetools::tokens;
using namespace sourcetools::cursors;
typedef sourcetools::tokens::Token Token;

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
}
