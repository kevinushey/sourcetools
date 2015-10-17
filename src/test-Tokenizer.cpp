#include <tests/testthat.h>
#include <parsr.h>

context("Tokenizer") {

  test_that("Complements are detected correctly") {

    using namespace parsr::tokens;

    expect_true(utils::complement(LPAREN)    == RPAREN);
    expect_true(utils::complement(LBRACE)    == RBRACE);
    expect_true(utils::complement(LBRACKET)  == RBRACKET);
    expect_true(utils::complement(LDBRACKET) == RDBRACKET);

    expect_true(utils::complement(RPAREN)    == LPAREN);
    expect_true(utils::complement(RBRACE)    == LBRACE);
    expect_true(utils::complement(RBRACKET)  == LBRACKET);
    expect_true(utils::complement(RDBRACKET) == LDBRACKET);

    expect_true(utils::isComplement(LPAREN,    RPAREN));
    expect_true(utils::isComplement(LBRACE,    RBRACE));
    expect_true(utils::isComplement(LBRACKET,  RBRACKET));
    expect_true(utils::isComplement(LDBRACKET, RDBRACKET));

    expect_true(utils::isComplement(RPAREN,    LPAREN));
    expect_true(utils::isComplement(RBRACE,    LBRACE));
    expect_true(utils::isComplement(RBRACKET,  LBRACKET));
    expect_true(utils::isComplement(RDBRACKET, LDBRACKET));
  }

  test_that("Keywords are detected correctly") {
    std::string code = "if for while";
    const auto& tokens = parsr::tokenize(code);
    for (auto& token : tokens) {
      if (parsr::tokens::utils::isWhitespace(token))
        continue;
      expect_true(parsr::tokens::utils::isKeyword(token));
    }
  }

}
