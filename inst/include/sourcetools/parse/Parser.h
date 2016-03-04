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

#define CHECK(__TYPE__)                                           \
  do                                                              \
  {                                                               \
    if (!current().isType(__TYPE__))                              \
    {                                                             \
      DEBUG(unexpectedTokenString(current(), __TYPE__));          \
      return ::sourcetools::parser::Node::create(current());      \
    }                                                             \
  } while (0)

#define CHECK_AND_ADVANCE(__TYPE__) \
  do                                \
  {                                 \
    CHECK(__TYPE__);                \
    advance();                      \
  } while (0)

#define CHECK_NOT(__TYPE__)                                        \
  do                                                               \
  {                                                                \
    if (!current().isType(__TYPE__))                               \
    {                                                              \
      DEBUG(unexpectedTokenString(current()));                     \
      return Node::create(INVALID);                                \
    }                                                              \
  } while (0)

#define CHECK_UNEXPECTED_END()                                 \
  do                                                           \
  {                                                            \
    if (current().isType(::sourcetools::tokens::END))          \
    {                                                          \
      ::std::cerr << "unexpected end of input" << ::std::endl; \
      return Node::create(INVALID);                            \
    }                                                          \
  } while (0)

class Parser
{
  typedef tokenizer::Tokenizer Tokenizer;
  typedef tokens::Token Token;

  enum ParseState
  {
    PARSE_STATE_TOP_LEVEL,
    PARSE_STATE_BRACE,
    PARSE_STATE_PAREN
  };

  Tokenizer tokenizer_;
  Token token_;
  Token previous_;
  ParseState state_;
  std::vector<ParseError> errors_;

#ifdef SOURCE_TOOLS_DEBUG_PARSER_STACK_OVERFLOW
  int counter_;
#endif

public:
  explicit Parser(const char* code, std::size_t n)
    : tokenizer_(code, n), state_(PARSE_STATE_TOP_LEVEL)
  {
    advance();

#ifdef SOURCE_TOOLS_DEBUG_PARSER_STACK_OVERFLOW
    counter_ = 0;
#endif
  }

private:

  // Error-related ----

  void unexpectedEndOfInput()
  {
    ParseError error("unexpected end of input");
    errors_.push_back(error);
  }

  std::string unexpectedTokenString(const Token& token)
  {
    return std::string() +
      "unexpected token '" + token.contents() + "'";
  }

  std::string unexpectedTokenString(const Token& token,
                                    tokens::TokenType expectedType)
  {
    return unexpectedTokenString(token) +
      "; expected type '" + toString(expectedType) + "'";
  }

  void unexpectedToken(const Token& token)
  {
    unexpectedToken(token, unexpectedTokenString(token));
  }

  void unexpectedToken(const Token& token,
                       const std::string& message)
  {
    ParseError error(token, message);
    errors_.push_back(error);
  }

  // Parser sub-routines ----

  Node* parseFunctionArgumentListOne()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseFunctionArgument()");
    using namespace tokens;

    Node* pNode = Node::create(current());
    CHECK_AND_ADVANCE(SYMBOL);
    if (current().isType(OPERATOR_ASSIGN_LEFT_EQUALS))
    {
      advance();
      pNode->add(parseExpression());
    }

    return pNode;
  }

  Node* parseFunctionArgumentList()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseFunctionArgumentList()");
    using namespace tokens;

    Node* pNode = Node::create(EMPTY);
    if (token_.isType(RPAREN))
      return pNode;

    while (true)
    {
      CHECK_UNEXPECTED_END();

      pNode->add(parseFunctionArgumentListOne());
      if (current().isType(RPAREN))
        return pNode;
      else if (current().isType(COMMA))
      {
        advance();
        continue;
      }

      // TODO: how should we recover here? For now, we
      // assume that there should have been a comma and
      // continue parsing.
      unexpectedToken(current(), "expected ',' or ')'");
      continue;
    }

    return pNode;
  }

  Node* parseFunctionDefinition()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseFunctionDefinition()");
    using namespace tokens;
    Node* pNode = Node::create(current());
    CHECK_AND_ADVANCE(KEYWORD_FUNCTION);
    CHECK_AND_ADVANCE(LPAREN);
    ParseState state = state_;
    state_ = PARSE_STATE_PAREN;
    pNode->add(parseFunctionArgumentList());
    state_ = state;
    CHECK_AND_ADVANCE(RPAREN);
    pNode->add(parseExpression());
    return pNode;
  }

  Node* parseFor()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseFor()");
    using namespace tokens;
    Node* pNode = Node::create(current());
    CHECK_AND_ADVANCE(KEYWORD_FOR);
    CHECK_AND_ADVANCE(LPAREN);
    ParseState state = state_;
    state_ = PARSE_STATE_PAREN;
    CHECK(SYMBOL);
    pNode->add(Node::create(consume()));
    CHECK_AND_ADVANCE(KEYWORD_IN);
    pNode->add(parseExpression());
    state_ = state;
    CHECK_AND_ADVANCE(RPAREN);
    pNode->add(parseExpression());
    return pNode;
  }

  Node* parseIf()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseIf()");
    using namespace tokens;
    Node* pNode = Node::create(current());
    CHECK_AND_ADVANCE(KEYWORD_IF);
    CHECK_AND_ADVANCE(LPAREN);
    ParseState state = state_;
    state_ = PARSE_STATE_PAREN;
    pNode->add(parseExpression());
    state_ = state;
    CHECK_AND_ADVANCE(RPAREN);
    pNode->add(parseExpression());
    if (current().isType(KEYWORD_ELSE))
    {
      advance();
      pNode->add(parseExpression());
    }
    return pNode;
  }

  Node* parseWhile()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseWhile()");
    using namespace tokens;
    Node* pNode = Node::create(current());
    CHECK_AND_ADVANCE(KEYWORD_WHILE);
    CHECK_AND_ADVANCE(LPAREN);
    ParseState state = state_;
    state_ = PARSE_STATE_PAREN;
    pNode->add(parseExpression());
    state_ = state;
    CHECK_AND_ADVANCE(RPAREN);
    pNode->add(parseExpression());
    return pNode;
  }

  Node* parseRepeat()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseRepeat()");
    using namespace tokens;
    Node* pNode = Node::create(current());
    CHECK_AND_ADVANCE(KEYWORD_REPEAT);
    pNode->add(parseExpression());
    return pNode;
  }

  Node* parseControlFlowKeyword()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseControlFlowKeyword('" << token_.contents() << "')");
    using namespace tokens;

    const Token& token = current();
    if (token.isType(KEYWORD_FUNCTION))
      return parseFunctionDefinition();
    else if (token.isType(KEYWORD_IF))
      return parseIf();
    else if (token.isType(KEYWORD_WHILE))
      return parseWhile();
    else if (token.isType(KEYWORD_FOR))
      return parseFor();
    else if (token.isType(KEYWORD_REPEAT))
      return parseRepeat();

    unexpectedToken(consume(), "expected control-flow keyword");
    return Node::create(INVALID);
  }

  Node* parseBracedExpression()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseBracedExpression()");
    using namespace tokens;
    Node* pNode = Node::create(current());

    CHECK_AND_ADVANCE(LBRACE);
    ParseState state = state_;
    state_ = PARSE_STATE_BRACE;
    skipSemicolons();
    if (current().isType(RBRACE))
    {
      pNode->add(Node::create(EMPTY));
    }
    else
    {
      while (!current().isType(RBRACE))
      {
        CHECK_UNEXPECTED_END();
        pNode->add(parseExpression());
        skipSemicolons();
      }
    }
    state_ = state;
    CHECK_AND_ADVANCE(RBRACE);

    return pNode;
  }

  Node* parseParentheticalExpression()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseParentheticalExpression()");
    using namespace tokens;
    Node* pNode = Node::create(current());
    CHECK_AND_ADVANCE(LPAREN);
    ParseState state = state_;
    state_ = PARSE_STATE_PAREN;
    if (current().isType(RPAREN))
      unexpectedToken(current());
    else
      pNode->add(parseExpression());
    state_ = state;
    CHECK_AND_ADVANCE(RPAREN);
    return pNode;
  }

  Node* parseUnaryOperator()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseUnaryOperator()");
    Node* pNode = Node::create(current());
    pNode->add(parseExpression(precedence::unary(consume())));
    return pNode;
  }

  Node* parseExpressionStart()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseExpressionStart('" << current().contents() << "')");
    SOURCE_TOOLS_DEBUG_PARSER_LOG("Type: " << toString(current().type()));
    using namespace tokens;

    skipSemicolons();
    const Token& token = current();

    if (isControlFlowKeyword(token))
      return parseControlFlowKeyword();
    else if (token.isType(LBRACE))
      return parseBracedExpression();
    else if (token.isType(LPAREN))
      return parseParentheticalExpression();
    else if (isUnaryOperator(token))
      return parseUnaryOperator();
    else if (isSymbolic(token) || isKeyword(token))
      return Node::create(consume());
    else if (token.isType(END))
      return NULL;

    unexpectedToken(consume());
    return Node::create(INVALID);
  }

  Node* parseFunctionCallOne(tokens::TokenType rhsType)
  {
    using namespace tokens;

    const Token& token = current();
    if (token.isType(COMMA) || token.isType(rhsType))
      return Node::create(Token(MISSING));

    if (peek().isType(OPERATOR_ASSIGN_LEFT_EQUALS))
    {
      Node* pLhs  = Node::create(consume());
      Node* pNode = Node::create(consume());
      pNode->add(pLhs);

      if (current().isType(COMMA) || current().isType(rhsType))
        pNode->add(Node::create(MISSING));
      else
        pNode->add(parseExpression());

      return pNode;
    }

    return parseExpression();
  }

  // Parse a function call, e.g.
  //
  //    <fn-call> ::= <expr> <fn-open> <fn-call-args> <fn-close>
  //
  //  <fn-open> can be one of '(', '[' or '[[',
  //  <fn-call-args> are (potentially named) comma-separated values
  //  <fn-close> is the complement of the above.
  //
  // Parsing a function call is surprisingly tricky, due to the
  // nature of allowing a mixture of unnamed, named, and missing
  // arguments.
  Node* parseFunctionCall(Node* pLhs)
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseFunctionCall('" << current().contents() << "')");
    using namespace tokens;
    TokenType lhsType = current().type();
    TokenType rhsType = complement(lhsType);

    Node* pNode = Node::create(current());
    pNode->add(pLhs);

    CHECK_AND_ADVANCE(lhsType);

    ParseState state = state_;
    state_ = PARSE_STATE_PAREN;

    if (current().isType(rhsType))
      pNode->add(lhsType == LPAREN ?
                   Node::create(Token(EMPTY)) :
                   Node::create(Token(MISSING)));
    else
    {
      while (true)
      {
        CHECK_UNEXPECTED_END();
        pNode->add(parseFunctionCallOne(rhsType));

        const Token& token = current();
        if (token.isType(COMMA))
        {
          consume();
          continue;
        }
        else if (token.isType(rhsType))
        {
          break;
        }

        std::string message = std::string() +
          "expected ',' or '" + toString(rhsType) + "'";
        unexpectedToken(current(), message);
      }
    }

    CHECK_AND_ADVANCE(rhsType);

    state_ = state;

    if (isCallOperator(current()) && canParseExpressionContinuation())
      return parseFunctionCall(pNode);
    return pNode;
  }

  Node* parseExpressionContinuation(Node* pNode)
  {
    using namespace tokens;
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseExpressionContinuation('" << current().contents() << "')");
    SOURCE_TOOLS_DEBUG_PARSER_LOG("Type: " << toString(current().type()));

    Token token = current();
    if (isCallOperator(token))
      return parseFunctionCall(pNode);
    else if (token.isType(END))
      return NULL;

    Node* pNew = Node::create(token);
    pNew->add(pNode);

    advance();
    int precedence =
      precedence::binary(token) -
      precedence::isRightAssociative(token);
    pNew->add(parseExpression(precedence));

    return pNew;
  }

  bool canParseExpressionContinuation(int precedence = 0)
  {
    return precedence < precedence::binary(current()) && (
      state_ == PARSE_STATE_PAREN ||
      previous().row() == current().row());
  }

  Node* parseExpression(int precedence = 0)
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseExpression(" << precedence << ")");
    using namespace tokens;
    Node* pNode = parseExpressionStart();
    while (canParseExpressionContinuation(precedence))
      pNode = parseExpressionContinuation(pNode);
    return pNode;
  }

  // Tokenization ----

  const Token& current()  const { return token_; }
  const Token& previous() const { return previous_; }

  Token consume()
  {
    Token token = current();
    advance();
    return token;
  }

  bool advance()
  {
    previous_ = token_;
    using namespace tokens;

    bool success = tokenizer_.tokenize(&token_);
    while (success && (isComment(token_) || isWhitespace(token_)))
      success = tokenizer_.tokenize(&token_);
    return success;
  }

  Token peek(std::size_t lookahead = 1,
             bool skipWhitespace = true,
             bool skipComments = true)
  {
    for (; lookahead < 100; ++lookahead)
    {
      Token result = tokenizer_.peek(lookahead);
      if (skipWhitespace && result.isType(tokens::WHITESPACE))
        continue;
      if (skipComments && result.isType(tokens::COMMENT))
        continue;
      return result;
    }
    return Token(tokens::INVALID);
  }

  // Utils ----

  void skipSemicolons()
  {
    while (current().isType(tokens::SEMI))
    {
      if (state_ == PARSE_STATE_PAREN)
        unexpectedToken(consume());
      else
        advance();
    }
  }

public:

  std::vector<Node*> parse()
  {
    std::vector<Node*> expression;

    while (true)
    {
      Node* pNode = parseExpression();
      if (!pNode)
        break;

      expression.push_back(pNode);
    }

    return expression;
  }

  const std::vector<ParseError>& errors() const
  {
    return errors_;
  }

};

} // namespace parser

void log(parser::Node* pNode, int depth = 0);

} // namespace sourcetools

#endif /* SOURCE_TOOLS_PARSE_PARSER_H */
