#include <testthat.h>
#include <sourcetools.h>

using namespace sourcetools;
using namespace sourcetools::tokens;
using namespace sourcetools::cursors;
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
    scoped_ptr<Node> pRoot(parser.parse());

    TokenCursor cursor(tokens);
    expect_true(cursor.findFwd("="));

    Position position = cursor.currentToken().position();
    Node* pTarget = parser.getNode(position);
    expect_true((pTarget != NULL));
    if (pTarget == NULL)
      return;

    const char* begin;
    const char* end;
    pTarget->bounds(&begin, &end);

    std::string contents(begin, end);
    expect_true(contents == "a = {1 + 2}");

    expect_true(cursor.findFwd("{"));
    pTarget = parser.getNode(cursor.position());
    expect_true((pTarget != NULL));
    expect_true((pTarget->token().contentsEqual("{")));
    if (pTarget == NULL)
      return;

    pTarget->bounds(&begin, &end);
    contents = std::string(begin, end);
    expect_true(contents == "{1 + 2}");


  }

}
