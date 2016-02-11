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

private:
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
    pNew->add(parseTopLevelExpression(precedence::left(token) - precedence::isRightAssociative(token)));
    return pNew;
  }

  std::shared_ptr<Node> parseTopLevelExpression(int precedence)
  {
    auto token = lookAhead_;
    if (tokens::isEnd(token))
      return nullptr;

    nextSignificantToken();

    auto pNode = parsePrefixExpression(token);
    while (precedence < precedence::left(lookAhead_))
    {
      token = lookAhead_;
      if (tokens::isEnd(token))
        return nullptr;

      nextSignificantToken();
      pNode = parseInfixExpression(token, pNode);
    }

    return pNode;
  }

  void nextSignificantToken()
  {
    using namespace tokens;

    bool success = tokenizer_.tokenize(&lookAhead_);
    while (success && (isComment(lookAhead_) || isWhitespace(lookAhead_)))
      success = tokenizer_.tokenize(&lookAhead_);
  }

public:

  std::vector<std::shared_ptr<Node>> parse()
  {
    std::vector<std::shared_ptr<Node>> expression;

    while (true)
    {
      auto pNode = parseTopLevelExpression(0);
      if (!pNode)
        break;

      expression.emplace_back(pNode);
    }

    return expression;
  }

};

} // namespace parser

void log(std::shared_ptr<parser::Node> pNode, int depth = 0);

} // namespace sourcetools

#endif /* SOURCE_TOOLS_PARSE_PARSER_H */
