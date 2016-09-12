#include <sourcetools.h>
#include <sourcetools/tests/testthat.h>

using namespace sourcetools;
using namespace sourcetools::parser;
using namespace sourcetools::cursors;
using namespace sourcetools::collections;

typedef sourcetools::tokens::Token Token;

context("Parser") {

  test_that("we can extract partial parse trees from code")
  {
    std::string code = "foo <- function(a = {1 + 2}) {}";

    std::vector<Token> tokens = tokenize(code);
    Parser parser(code);

    ParseStatus status;
    scoped_ptr<ParseNode> pRoot(parser.parse(&status));

    TokenCursor cursor(tokens);
    expect_true(cursor.findFwd("="));

    Position position = cursor.currentToken().position();
    ParseNode* pTarget = status.getNodeAtPosition(position);
    expect_true((pTarget != NULL));
    if (pTarget == NULL)
      return;

    const char* begin;
    const char* end;
    pTarget->bounds(&begin, &end);

    std::string contents(begin, end);
    expect_true(contents == "a = {1 + 2}");

    expect_true(cursor.findFwd("{"));
    pTarget = status.getNodeAtPosition(cursor.position());
    expect_true((pTarget != NULL));
    expect_true((pTarget->token().contentsEqual("{")));
    if (pTarget == NULL)
      return;

    pTarget->bounds(&begin, &end);
    contents = std::string(begin, end);
    expect_true(contents == "{1 + 2}");
  }

}
