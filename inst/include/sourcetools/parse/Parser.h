#ifndef SOURCE_TOOLS_PARSE_PARSER_H
#define SOURCE_TOOLS_PARSE_PARSER_H

#include <iostream>

#include <sourcetools/tokenization/tokenization.h>
#include <sourcetools/collection/collection.h>
#include <sourcetools/cursor/cursor.h>

#include <sourcetools/parse/Node.h>
#include <sourcetools/parse/Precedence.h>
#include <sourcetools/parse/ParseError.h>

// Defines that will go away once the parser is more tested / game ready
// #define SOURCE_TOOLS_DEBUG_PARSER_TRACE
// #define SOURCE_TOOLS_DEBUG_PARSER_PRINT_TOKEN_INFO
// #define SOURCE_TOOLS_DEBUG_PARSER_STACK_OVERFLOW

#ifdef SOURCE_TOOLS_DEBUG_PARSER_TRACE

# define SOURCE_TOOLS_DEBUG_PARSER_LOG(__X__) std::cerr << __X__ << std::endl

#else

# define SOURCE_TOOLS_DEBUG_PARSER_LOG(__X__)

#endif

#ifdef SOURCE_TOOLS_DEBUG_PARSER_PRINT_TOKEN_INFO

# define SOURCE_TOOLS_DEBUG_TOKEN(__TOKEN__)                 \
  do                                                         \
  {                                                          \
    std::cout << __TOKEN__ << std::endl;                     \
  } while (0)

#else

# define SOURCE_TOOLS_DEBUG_TOKEN(__TOKEN__)                 \
  do                                                         \
  {                                                          \
  } while (0)                                                \

#endif

#ifdef SOURCE_TOOLS_DEBUG_PARSER_STACK_OVERFLOW

# define SOURCE_TOOLS_DEBUG_CHECK_PARSER_STACK_OVERFLOW()    \
  do                                                         \
  {                                                          \
    if (counter_++ > 10000)                                  \
    {                                                        \
      std::cerr << "ERROR: stack overflow in parser\n";      \
      return Node::create(token_);                           \
    }                                                        \
  } while (0)

#else

# define SOURCE_TOOLS_DEBUG_CHECK_PARSER_STACK_OVERFLOW()    \
  do                                                         \
  {                                                          \
  } while (0)

#endif

namespace sourcetools {
namespace parser {

#define MOVE_TO_NEXT_SIGNIFICANT_TOKEN()                     \
  do                                                         \
  {                                                          \
    if (!nextSignificantToken())                             \
    {                                                        \
      std::cerr << "unexpected end of input" << std::endl;   \
      return Node::create(token_);                           \
    }                                                        \
  } while (0)                                                \

#define CHECK_TYPE(__TYPE__)                                            \
  do                                                                    \
  {                                                                     \
    if (!token_.isType(__TYPE__))                                       \
    {                                                                   \
      std::cout << token_.position() << "unexpected token '"            \
                << token_.contents() << "'; expected '"                 \
                << toString(__TYPE__) << "'" << std::endl;              \
      return Node::create(token_);                                      \
    }                                                                   \
  } while (0)

#define CHECK_TYPE_NOT(__TYPE__)                                        \
  do                                                                    \
  {                                                                     \
    if (token_.isType(__TYPE__))                                        \
    {                                                                   \
      std::cout << token_.position() << "unexpected token '"            \
                << token_.contents() << "'" << std::endl;               \
      return Node::create(token_);                                      \
    }                                                                   \
  } while (0)

class Parser
{
  typedef tokenizer::Tokenizer Tokenizer;
  typedef tokens::Token Token;

  std::string program_;
  Tokenizer tokenizer_;
  Token token_;
  std::vector<ParseError> errors_;

#ifdef SOURCE_TOOLS_DEBUG_PARSER_STACK_OVERFLOW
  int counter_;
#endif

public:
  explicit Parser(const std::string& program)
    : program_(program),
      tokenizer_(program_)
  {
    tokenizer_.tokenize(&token_);

#ifdef SOURCE_TOOLS_DEBUG_PARSER_STACK_OVERFLOW
    counter_ = 0;
#endif
  }

private:

  void unexpectedtoken(const Token& token)
  {
    std::string message = std::string() + "unexpected token '" + token.contents() + "'";
    unexpectedToken(token, std::move(message));
  }

  void unexpectedToken(const Token& token, const std::string& message)
  {
    ParseError error(token, message);
    errors_.push_back(std::move(error));
  }

  std::shared_ptr<Node> parseControlFlowKeyword(const Token& token)
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseControlFlowKeyword('" << token.contents() << "')");
    using namespace tokens;

    if (token.isType(KEYWORD_FUNCTION))
      return parseFunction();
    else if (token.isType(KEYWORD_IF))
      return parseIf();
    else if (token.isType(KEYWORD_WHILE))
      return parseWhile();
    else if (token.isType(KEYWORD_FOR))
      return parseFor();
    else if (token.isType(KEYWORD_REPEAT))
      return parseRepeat();

    return nullptr;
  }

  std::shared_ptr<Node> parseFunctionArgument()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseFunctionArgument()");
    using namespace tokens;

    auto pNode = Node::create(token_);
    CHECK_TYPE(SYMBOL);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN();
    if (token_.isType(OPERATOR_ASSIGN_LEFT_EQUALS))
    {
      MOVE_TO_NEXT_SIGNIFICANT_TOKEN();
      pNode->add(parseTopLevelExpression());
    }

    return pNode;
  }

  std::shared_ptr<Node> parseFunctionArgumentList()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseFunctionArgumentList()");
    using namespace tokens;

    auto pNode = Node::create(Token());
    if (token_.isType(RPAREN))
      return pNode;

    while (true)
    {
      pNode->add(parseFunctionArgument());
      if (token_.isType(RPAREN))
        return pNode;
      else if (token_.isType(COMMA))
      {
        MOVE_TO_NEXT_SIGNIFICANT_TOKEN();
        continue;
      }

      // TODO: how should we 'recover' here?
      unexpectedToken(token_, "expected ',' or ')'");
      MOVE_TO_NEXT_SIGNIFICANT_TOKEN();
    }
    return pNode;
  }

  std::shared_ptr<Node> parseFunction()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseFunction()");
    using namespace tokens;

    auto pNode = Node::create(token_);
    CHECK_TYPE(KEYWORD_FUNCTION);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN();
    CHECK_TYPE(LPAREN);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN();
    pNode->add(parseFunctionArgumentList());
    CHECK_TYPE(RPAREN);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN();
    pNode->add(parseTopLevelExpression());
    return pNode;
  }

  std::shared_ptr<Node> parseFor()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseFor()");
    using namespace tokens;

    auto pNode = Node::create(token_);
    CHECK_TYPE(KEYWORD_FOR);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN();
    CHECK_TYPE(LPAREN);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN();
    CHECK_TYPE(SYMBOL);
    pNode->add(Node::create(token_));
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN();
    CHECK_TYPE(KEYWORD_IN);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN();
    pNode->add(parseTopLevelExpression());
    CHECK_TYPE(RPAREN);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN();
    pNode->add(parseTopLevelExpression());
    return pNode;
  }

  std::shared_ptr<Node> parseIf()
  {
    using namespace tokens;
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseIf()");

    auto pNode = Node::create(token_);
    CHECK_TYPE(KEYWORD_IF);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN();
    CHECK_TYPE(LPAREN);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN();
    pNode->add(parseTopLevelExpression());
    CHECK_TYPE(RPAREN);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN();
    pNode->add(parseTopLevelExpression());
    while (token_.isType(KEYWORD_ELSE))
    {
      MOVE_TO_NEXT_SIGNIFICANT_TOKEN();
      pNode->add(parseTopLevelExpression());
    }
    return pNode;
  }

  std::shared_ptr<Node> parseWhile()
  {
    using namespace tokens;
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseWhile()");

    auto pNode = Node::create(token_);
    CHECK_TYPE(KEYWORD_WHILE);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN();
    CHECK_TYPE(LPAREN);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN();
    CHECK_TYPE_NOT(RPAREN);
    pNode->add(parseTopLevelExpression());
    CHECK_TYPE(RPAREN);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN();
    pNode->add(parseTopLevelExpression());
    return pNode;
  }

  std::shared_ptr<Node> parseRepeat()
  {
    using namespace tokens;
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseRepeat()");

    auto pNode = Node::create(token_);
    CHECK_TYPE(KEYWORD_REPEAT);
    MOVE_TO_NEXT_SIGNIFICANT_TOKEN();
    pNode->add(parseTopLevelExpression());
    return pNode;
  }

  std::shared_ptr<Node> parsePrefixExpression(const Token& token)
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parsePrefixExpression('" << token.contents() << "')");
    using namespace tokens;

    auto pNode = Node::create(token);
    nextSignificantToken();

    if (isOperator(token))
      pNode->add(parseTopLevelExpression(precedence::right(token)));
    else if (token.isType(LBRACE))
    {
      while (true)
      {
        while (token_.isType(SEMI))
          nextSignificantToken();

        if (token_.isType(RBRACE))
        {
          nextSignificantToken();
          return pNode;
        }

        pNode->add(parseTopLevelExpression());
      }
    }
    else if (token.isType(LPAREN))
    {
      CHECK_TYPE_NOT(RPAREN);
      pNode->add(parseTopLevelExpression());
      CHECK_TYPE(RPAREN);
      nextSignificantToken();
    }

    return pNode;
  }

  std::shared_ptr<Node> parseInfixExpression(const Token& token,
                                             std::shared_ptr<Node> pNode)
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseInfixExpression('" << token.contents() << "')");

    auto pNew = Node::create(token);
    nextSignificantToken();
    pNew->add(pNode);
    pNew->add(parseTopLevelExpression(precedence::left(token) - precedence::isRightAssociative(token)));
    return pNew;
  }

  std::shared_ptr<Node> parseTopLevelExpression(int precedence = 0)
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseTopLevelExpression(" << precedence << ")");
    using namespace tokens;

    SOURCE_TOOLS_DEBUG_CHECK_PARSER_STACK_OVERFLOW();
    SOURCE_TOOLS_DEBUG_TOKEN(token_);

    auto token = token_;
    if (isEnd(token))
      return nullptr;

    std::shared_ptr<Node> pNode = nullptr;
    if (isControlFlowKeyword(token))
      pNode = parseControlFlowKeyword(token);
    else
      pNode = parsePrefixExpression(token);

    if (isEnd(token))
      return pNode;

    while (precedence < precedence::left(token_))
    {
      token = token_;
      if (tokens::isEnd(token))
        return nullptr;

      pNode = parseInfixExpression(token, pNode);
    }

    return pNode;
  }

  bool nextSignificantToken()
  {
    using namespace tokens;

    bool success = tokenizer_.tokenize(&token_);
    while (success && (isComment(token_) || isWhitespace(token_)))
      success = tokenizer_.tokenize(&token_);
    return success;
  }

public:

  std::vector<std::shared_ptr<Node>> parse()
  {
    std::vector<std::shared_ptr<Node>> expression;

    while (true)
    {
      auto pNode = parseTopLevelExpression();
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
