#ifndef SOURCE_TOOLS_PARSE_PARSER_H
#define SOURCE_TOOLS_PARSE_PARSER_H

#include <iostream>

#include <sourcetools/tokenization/tokenization.h>
#include <sourcetools/collection/collection.h>
#include <sourcetools/cursor/cursor.h>

#include <sourcetools/parse/Node.h>
#include <sourcetools/parse/Precedence.h>

namespace sourcetools {
namespace parser {

class Parser
{
  typedef tokenizer::Tokenizer Tokenizer;
  typedef tokens::Token Token;

  std::string program_;
  Tokenizer tokenizer_;
  Token lookAhead_;

public:
  explicit Parser(const std::string& program)
    : program_(program),
      tokenizer_(program_)
  {
    tokenizer_.tokenize(&lookAhead_);
  }

  // forward-declare
  std::shared_ptr<Node> parsePrefixExpression(const Token& token)
  {
    auto pNew = Node::create(token);
    if (tokens::isOperator(token))
      pNew->add(parseTopLevelExpression(precedence::right(token)));
    return pNew;
  }

  std::shared_ptr<Node> parseInfixExpression(const Token& token,
                                             std::shared_ptr<Node> pNode)
  {
    auto pNew = Node::create(token);
    pNew->add(pNode);
    pNew->add(parseTopLevelExpression(precedence::left(token)));
    return pNew;
  }

  std::shared_ptr<Node> parseTopLevelExpression(int precedence)
  {
    auto token = lookAhead_;
    tokenizer_.tokenize(&lookAhead_);

    auto pNode = parsePrefixExpression(token);
    while (precedence < precedence::left(lookAhead_))
    {
      token = lookAhead_;
      tokenizer_.tokenize(&lookAhead_);
      pNode = parseInfixExpression(token, pNode);
    }

    return pNode;
  }

};

} // namespace parser

inline std::shared_ptr<parser::Node> parse(const std::string& program)
{
  parser::Parser parser(program);
  return parser.parseTopLevelExpression(0);
}

void log(std::shared_ptr<parser::Node> pNode, int depth = 0);

} // namespace sourcetools

#endif /* SOURCE_TOOLS_PARSE_PARSER_H */
