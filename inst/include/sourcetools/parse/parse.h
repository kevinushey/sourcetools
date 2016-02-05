#ifndef SOURCE_TOOLS_PARSE_PARSE_H
#define SOURCE_TOOLS_PARSE_PARSE_H

#include <vector>
#include <string>
#include <memory>
#include <iostream>

#include <sourcetools/Tokenizer.h>

#include <sourcetools/tokens/Token.h>
#include <sourcetools/cursors/TokenCursor.h>
#include <sourcetools/collections/Position.h>

namespace sourcetools {
namespace parser {

class ParseNode
{
private:
  typedef tokens::Token Token;

public:
  static std::shared_ptr<ParseNode> createRootNode()
  {
    return std::make_shared<ParseNode>(nullptr);
  }

  ParseNode* push(const Token& token)
  {
    auto node = std::make_shared<ParseNode>(parent_, token);
    children_.push_back(node);
    return node.get();
  }

  const ParseNode* parent() const { return parent_; }

  // TODO: I don't want these ctors to be public but apparently
  // 'std::make_shared()' demands it?
  ParseNode(ParseNode* parent)
    : token_(tokens::ERR), parent_(parent)
  {}

  ParseNode(ParseNode* parent, const Token& token)
    : token_(token), parent_(parent)
  {}

private:
  Token token_;

  ParseNode* parent_;
  std::vector<std::shared_ptr<ParseNode>> children_;
};

class Parser
{
private:
  typedef cursors::TokenCursor TokenCursor;
  typedef tokens::Token Token;
  typedef tokens::TokenType TokenType;

private:
  void checkType(TokenCursor* pCursor, TokenType type,
                 const char* fileName, int lineNumber)
  {
    if (!pCursor->isType(type))
    {
      DEBUG_BLOCK() {
        std::string file(fileName);
        std::string shortFile = file.substr(file.rfind("/") + 1);
        std::cerr << "(" << shortFile << ":" << lineNumber << "): Expected type "
                  << "'" << sourcetools::toString(type) << "'" << "; got "
                  << "'" << sourcetools::toString(pCursor->type()) << "'" << std::endl;
      }
    }
  }

#define CHECK_TYPE(__CURSOR__, __TYPE__)                       \
  do                                                           \
  {                                                            \
    checkType(__CURSOR__, __TYPE__, __FILE__, __LINE__);       \
  } while (0)

#define CHECK_TYPE_NOT(__CURSOR__, __TYPE__)                   \
  do                                                           \
  {                                                            \
    !checkType(__CURSOR__, __TYPE__, __FILE__, __LINE__);      \
  } while (0)

#define PARSE_ACTION(__CURSOR__, __ACTION__)                   \
  do {                                                         \
    if (!__CURSOR__->__ACTION__())                             \
      return;                                                  \
  } while (0)

#define MOVE_TO_NEXT_TOKEN(__CURSOR__)                         \
  PARSE_ACTION(__CURSOR__, moveToNextToken)

#define MOVE_TO_NEXT_SIGNIFICANT_TOKEN(__CURSOR__)             \
  PARSE_ACTION(__CURSOR__, moveToNextSignificantToken)

#define FWD_OVER_WHITESPACE_AND_COMMENTS(__CURSOR__)           \
  PARSE_ACTION(__CURSOR__, fwdOverWhitespaceAndComments)

  void parseFunctionArgumentList(TokenCursor* pCursor, std::shared_ptr<ParseNode>& pNode)
  {
  }

  void parseFunction(TokenCursor* pCursor, std::shared_ptr<ParseNode>& pNode)
  {
    CHECK_TYPE(pCursor, tokens::KEYWORD_FUNCTION);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN(pCursor);
    CHECK_TYPE(pCursor, tokens::LPAREN);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN(pCursor);
    parseFunctionArgumentList(pCursor, pNode);
    CHECK_TYPE(pCursor, tokens::RPAREN);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN(pCursor);
    parseTopLevelExpression(pCursor, pNode);
  }

  void parseFor(TokenCursor* pCursor, std::shared_ptr<ParseNode>& pNode)
  {
    CHECK_TYPE(pCursor, tokens::KEYWORD_FOR);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN(pCursor);
    CHECK_TYPE(pCursor, tokens::LPAREN);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN(pCursor);
    CHECK_TYPE(pCursor, tokens::SYMBOL);
  }
  void parseWhile(TokenCursor* pCursor, std::shared_ptr<ParseNode>& pNode)
  {
    CHECK_TYPE(pCursor, tokens::KEYWORD_WHILE);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN(pCursor);
    CHECK_TYPE(pCursor, tokens::LPAREN);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN(pCursor);
    parseTopLevelExpression(pCursor, pNode);
    CHECK_TYPE(pCursor, tokens::RPAREN);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN(pCursor);
    parseTopLevelExpression(pCursor, pNode);
  }

  void parseRepeat(TokenCursor* pCursor, std::shared_ptr<ParseNode>& pNode)
  {
    CHECK_TYPE(pCursor, tokens::KEYWORD_REPEAT);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN(pCursor);
    parseTopLevelExpression(pCursor, pNode);
  }

  void parseIf(TokenCursor* pCursor, std::shared_ptr<ParseNode>& pNode)
  {
    CHECK_TYPE(pCursor, tokens::KEYWORD_IF);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN(pCursor);
    CHECK_TYPE(pCursor, tokens::LPAREN);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN(pCursor);
    parseTopLevelExpression(pCursor, pNode);
    CHECK_TYPE(pCursor, tokens::RPAREN);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN(pCursor);
    parseTopLevelExpression(pCursor, pNode);
    if (pCursor->isType(tokens::KEYWORD_ELSE)) {
      MOVE_TO_NEXT_SIGNIFICANT_TOKEN(pCursor);
      parseTopLevelExpression(pCursor, pNode);
    }
  }

  void parseTopLevelExpression(TokenCursor* pCursor, std::shared_ptr<ParseNode>& pNode)
  {
    FWD_OVER_WHITESPACE_AND_COMMENTS(pCursor);

    if (tokens::isKeyword(pCursor->currentToken()))
    {
      using namespace tokens::types;
      TokenType type = pCursor->type();
      if (type == KEYWORD_IF)
        parseIf(pCursor, pNode);
      else if (type == KEYWORD_REPEAT)
        parseRepeat(pCursor, pNode);
      else if (type == KEYWORD_FUNCTION)
        parseFunction(pCursor, pNode);
      else if (type == KEYWORD_FOR)
        parseFor(pCursor, pNode);
      else if (type == KEYWORD_WHILE)
        parseWhile(pCursor, pNode);
    }

    else if (pCursor->type() == tokens::LBRACE)
    {
      MOVE_TO_NEXT_TOKEN(pCursor);
      FWD_OVER_WHITESPACE_AND_COMMENTS(pCursor);
      parseTopLevelExpression(pCursor, pNode);
      FWD_OVER_WHITESPACE_AND_COMMENTS(pCursor);
      CHECK_TYPE(pCursor, tokens::RBRACE);
      MOVE_TO_NEXT_TOKEN(pCursor);
    }
    else if (pCursor->type() == tokens::LPAREN)
    {
      MOVE_TO_NEXT_TOKEN(pCursor);
      FWD_OVER_WHITESPACE_AND_COMMENTS(pCursor);
      parseTopLevelExpression(pCursor, pNode);
      FWD_OVER_WHITESPACE_AND_COMMENTS(pCursor);
      CHECK_TYPE(pCursor, tokens::RPAREN);
      MOVE_TO_NEXT_TOKEN(pCursor);
    }
  }

public:
  std::shared_ptr<ParseNode> parse(const std::string& contents)
  {
    const auto& tokens = tokenize(contents);
    LOG(tokens);

    TokenCursor cursor(tokens);
    std::shared_ptr<ParseNode> root = ParseNode::createRootNode();

    do
    {
      cursor.fwdOverWhitespaceAndComments();
      parseTopLevelExpression(&cursor, root);
    }
    while (cursor.moveToNextToken());
    return root;
  }

};

} // namespace parser

inline std::shared_ptr<parser::ParseNode> parse(const std::string& contents)
{
  parser::Parser parser;
  return parser.parse(contents);
}

} // namespace sourcetools


#endif /* SOURCE_TOOLS_PARSE_PARSE_H */
