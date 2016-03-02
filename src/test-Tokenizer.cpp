#include <tests/testthat.h>
#include <sourcetools.h>

namespace {
typedef sourcetools::tokens::Token Token;
using namespace sourcetools::tokens;
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
}
