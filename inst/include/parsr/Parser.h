#ifndef PARSR_PARSER_H
#define PARSR_PARSER_H

#include <vector>
#include <string>
#include <memory>

#include <parsr/Tokenizer.h>

#include <parsr/tokens/Token.h>
#include <parsr/cursors/TokenCursor.h>
#include <parsr/collections/Position.h>

namespace parsr {
namespace parser {

class ParseNode
{
private:
  ParseNode* parent_;
  std::vector<ParseNode> children_;
};

class Parser
{
private:
  typedef cursors::TokenCursor Cursor;
  typedef tokens::Token Token;
  typedef tokens::TokenType TokenType;

public:

  Parser(const std::string& contents)
    : contents_(contents)
  {}

private:

  // TODO: 'handle*', 'parse*' for each type of expression
public:

  ParseNode parse()
  {
    const auto& tokens = tokenize(contents_);
    Cursor cursor(tokens);

    ParseNode root;
    do
    {
      // Parse expressions
    }
    while (cursor.moveToNextToken());
    return root;
  }

private:

  const std::string& contents_;
};

} // namespace parser
} // namespace parsr

#endif
