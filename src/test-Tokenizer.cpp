#include <tests/testthat.h>
#include <parsr.h>

context("Tokenizer") {

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
