#ifndef SOURCETOOLS_PARSE_PARSER_H
#define SOURCETOOLS_PARSE_PARSER_H
#define SOURCE_TOOLS_PARSE_PARSER_H

#include <iostream>

#include <sourcetools/tokenization/tokenization.h>
#include <sourcetools/collection/collection.h>
#include <sourcetools/cursor/cursor.h>

#include <sourcetools/parse/ParseNode.h>
#include <sourcetools/parse/Precedence.h>
#include <sourcetools/parse/ParseError.h>
#include <sourcetools/parse/ParseStatus.h>

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

namespace sourcetools {
namespace parser {

class Parser
{
  typedef tokenizer::Tokenizer Tokenizer;
  typedef tokens::Token Token;
  typedef tokens::TokenType TokenType;
  typedef collections::Position Position;

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
  ParseStatus* pStatus_;

public:
  explicit Parser(const std::string& code)
    : tokenizer_(code.c_str(), code.size()),
      state_(PARSE_STATE_TOP_LEVEL)
  {
    advance();
  }

  explicit Parser(const char* code, index_type n)
    : tokenizer_(code, n),
      state_(PARSE_STATE_TOP_LEVEL)
  {
    advance();
  }

private:

  // Error-related ----

  void unexpectedEndOfInput()
  {
    ParseError error("unexpected end of input");
    pStatus_->addError(error);
  }

  std::string unexpectedTokenString(const Token& token)
  {
    return std::string() +
      "unexpected token '" + token.contents() + "'";
  }

  std::string unexpectedTokenString(const Token& token,
                                    TokenType expectedType)
  {
    return unexpectedTokenString(token) +
      "; expected type '" + toString(expectedType) + "'";
  }

  void unexpectedToken(const Token& token)
  {
    unexpectedToken(token, unexpectedTokenString(token));
  }

  void unexpectedToken(const Token& token,
                       TokenType type)
  {
    unexpectedToken(token, unexpectedTokenString(token, type));
  }

  void unexpectedToken(const Token& token,
                       const std::string& message)
  {
    ParseError error(token, message);
    pStatus_->addError(error);
  }

  bool checkUnexpectedEnd(const Token& token)
  {
    if (UNLIKELY(token.isType(tokens::END)))
    {
      ParseError error(token, "unexpected end of input");
      pStatus_->addError(error);
      return true;
    }

    return false;
  }

  // Parser sub-routines ----

  ParseNode* parseFunctionArgumentListOne()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseFunctionArgument()");
    using namespace tokens;

    check(SYMBOL);

    Token lookahead = peek(1);
    if (lookahead.isType(COMMA) || lookahead.isType(RPAREN))
      return ParseNode::create(consume());
    else if (lookahead.isType(OPERATOR_ASSIGN_LEFT_EQUALS))
      return parseExpression();

    if (isOperator(lookahead))
      unexpectedToken(lookahead, "expected '=', ',' or ')' following argument name");

    return parseExpression();
  }

  ParseNode* parseFunctionArgumentList()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseFunctionArgumentList()");
    using namespace tokens;

    ParseNode* pNode = createNode(EMPTY);
    if (token_.isType(RPAREN))
      return pNode;

    while (true)
    {
      if (checkUnexpectedEnd(current()))
        break;

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

  ParseNode* parseFunctionDefinition()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseFunctionDefinition()");
    using namespace tokens;
    ParseNode* pNode = createNode(current());
    checkAndAdvance(KEYWORD_FUNCTION);
    checkAndAdvance(LPAREN, false);
    ParseState state = state_;
    state_ = PARSE_STATE_PAREN;
    pNode->add(parseFunctionArgumentList());
    state_ = state;
    checkAndAdvance(RPAREN, false);
    pNode->add(parseNonEmptyExpression());
    return pNode;
  }

  ParseNode* parseFor()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseFor()");
    using namespace tokens;
    ParseNode* pNode = createNode(current());
    checkAndAdvance(KEYWORD_FOR);
    checkAndAdvance(LPAREN, false);
    ParseState state = state_;
    state_ = PARSE_STATE_PAREN;
    check(SYMBOL);
    pNode->add(createNode(consume()));
    checkAndAdvance(KEYWORD_IN, false);
    pNode->add(parseNonEmptyExpression());
    state_ = state;
    checkAndAdvance(RPAREN, false);
    pNode->add(parseNonEmptyExpression());
    return pNode;
  }

  ParseNode* parseIf()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseIf()");
    using namespace tokens;
    ParseNode* pNode = createNode(current());
    checkAndAdvance(KEYWORD_IF);
    checkAndAdvance(LPAREN, false);
    ParseState state = state_;
    state_ = PARSE_STATE_PAREN;
    pNode->add(parseNonEmptyExpression());
    state_ = state;
    checkAndAdvance(RPAREN, false);
    pNode->add(parseNonEmptyExpression());
    if (current().isType(KEYWORD_ELSE))
    {
      advance();
      pNode->add(parseNonEmptyExpression());
    }
    return pNode;
  }

  ParseNode* parseWhile()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseWhile()");
    using namespace tokens;
    ParseNode* pNode = createNode(current());
    checkAndAdvance(KEYWORD_WHILE);
    checkAndAdvance(LPAREN, false);
    ParseState state = state_;
    state_ = PARSE_STATE_PAREN;
    pNode->add(parseNonEmptyExpression());
    state_ = state;
    checkAndAdvance(RPAREN, false);
    pNode->add(parseNonEmptyExpression());
    return pNode;
  }

  ParseNode* parseRepeat()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseRepeat()");
    using namespace tokens;
    ParseNode* pNode = createNode(current());
    checkAndAdvance(KEYWORD_REPEAT);
    pNode->add(parseNonEmptyExpression());
    return pNode;
  }

  ParseNode* parseControlFlowKeyword()
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
    return createNode(INVALID);
  }

  ParseNode* parseBracedExpression()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseBracedExpression()");
    using namespace tokens;
    ParseNode* pNode = createNode(current());

    checkAndAdvance(LBRACE);
    ParseState state = state_;
    state_ = PARSE_STATE_BRACE;
    skipSemicolons();
    if (current().isType(RBRACE))
    {
      pNode->add(createNode(EMPTY));
    }
    else
    {
      while (!current().isType(RBRACE))
      {
        if (checkUnexpectedEnd(current()))
          break;
        pNode->add(parseNonEmptyExpression());
        skipSemicolons();
      }
    }
    state_ = state;
    pNode->setEnd(current());
    checkAndAdvance(RBRACE);

    return pNode;
  }

  ParseNode* parseParentheticalExpression()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseParentheticalExpression()");
    using namespace tokens;
    ParseNode* pNode = createNode(current());
    checkAndAdvance(LPAREN);
    ParseState state = state_;
    state_ = PARSE_STATE_PAREN;
    if (current().isType(RPAREN))
      unexpectedToken(current());
    else
      pNode->add(parseNonEmptyExpression());
    state_ = state;
    pNode->setEnd(current());
    checkAndAdvance(RPAREN);
    return pNode;
  }

  ParseNode* parseUnaryOperator()
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseUnaryOperator()");
    ParseNode* pNode = createNode(current());
    pNode->add(parseNonEmptyExpression(precedence::unary(consume())));
    return pNode;
  }

  ParseNode* parseExpressionStart()
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
      return createNode(consume());
    else if (token.isType(END))
      return NULL;

    unexpectedToken(consume());
    return createNode(INVALID);
  }

  ParseNode* parseFunctionCallOne(TokenType rhsType)
  {
    using namespace tokens;

    const Token& token = current();
    if (token.isType(COMMA) || token.isType(rhsType))
      return createNode(Token(MISSING));

    if (peek(1).isType(OPERATOR_ASSIGN_LEFT_EQUALS))
    {
      ParseNode* pLhs  = createNode(consume());
      ParseNode* pNode = createNode(consume());
      pNode->add(pLhs);

      if (current().isType(COMMA) || current().isType(rhsType))
        pNode->add(createNode(MISSING));
      else
        pNode->add(parseNonEmptyExpression());

      return pNode;
    }

    return parseNonEmptyExpression();
  }

  // Parse a function call, e.g.
  //
  //    <fn-call> ::= <expr> <fn-open> <fn-call-args> <fn-close>
  //
  // <fn-open> can be one of '(', '[' or '[[',
  // <fn-call-args> are (potentially named) comma-separated values
  // <fn-close> is the complement of the above.
  //
  // Parsing a function call is surprisingly tricky, due to the
  // nature of allowing a mixture of unnamed, named, and missing
  // arguments.
  ParseNode* parseFunctionCall(ParseNode* pLhs)
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseFunctionCall('" << current().contents() << "')");
    using namespace tokens;
    TokenType lhsType = current().type();
    TokenType rhsType = complement(lhsType);

    ParseNode* pNode = createNode(current());
    pNode->add(pLhs);

    checkAndAdvance(lhsType);

    ParseState state = state_;
    state_ = PARSE_STATE_PAREN;

    if (current().isType(rhsType))
    {
      pNode->add(lhsType == LPAREN ?
                   createNode(Token(EMPTY)) :
                   createNode(Token(MISSING)));
    }
    else
    {
      while (true)
      {
        if (checkUnexpectedEnd(current()))
          break;

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

    checkAndAdvance(rhsType);

    state_ = state;

    if (isCallOperator(current()) && canParseExpressionContinuation())
      return parseFunctionCall(pNode);
    return pNode;
  }

  ParseNode* parseExpressionContinuation(ParseNode* pNode)
  {
    using namespace tokens;
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseExpressionContinuation('" << current().contents() << "')");
    SOURCE_TOOLS_DEBUG_PARSER_LOG("Type: " << toString(current().type()));

    Token token = current();
    if (isCallOperator(token))
      return parseFunctionCall(pNode);
    else if (token.isType(END))
      return createNode(token);

    ParseNode* pNew = createNode(token);
    pNew->add(pNode);

    advance();
    int precedence =
      precedence::binary(token) -
      precedence::isRightAssociative(token);
    pNew->add(parseNonEmptyExpression(precedence));

    return pNew;
  }

  bool canParseExpressionContinuation(int precedence = 0)
  {
    return precedence < precedence::binary(current()) && (
      state_ == PARSE_STATE_PAREN ||
      previous().row() == current().row());
  }

  ParseNode* parseExpression(int precedence = 0)
  {
    SOURCE_TOOLS_DEBUG_PARSER_LOG("parseExpression(" << precedence << ")");
    using namespace tokens;
    ParseNode* pNode = parseExpressionStart();
    while (canParseExpressionContinuation(precedence))
      pNode = parseExpressionContinuation(pNode);
    return pNode;
  }

  ParseNode* parseNonEmptyExpression(int precedence = 0)
  {
    if (checkUnexpectedEnd(current()))
      return ParseNode::create(tokens::MISSING);
    return parseExpression(precedence);
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

  bool check(TokenType type)
  {
    const Token& token = current();
    bool success = token.isType(type);
    if (!success)
      unexpectedToken(token, type);
    return success;
  }

  bool checkAndAdvance(TokenType type, bool advanceOnError = true)
  {
    bool result = check(type);
    if (result || advanceOnError) advance();
    return result;
  }

  Token peek(index_type lookahead = 0,
             bool skipWhitespace = true,
             bool skipComments = true)
  {
    index_type offset = lookahead;

    while (true)
    {
      Token result = tokenizer_.peek(offset);
      if ((skipWhitespace && result.isType(tokens::WHITESPACE)) ||
          (skipComments && result.isType(tokens::COMMENT)))
      {
        ++offset;
        continue;
      }

      if (lookahead == 0)
        return result;

      --lookahead;
    }

  }

  // Utils ----

  ParseNode* createNode(TokenType type)
  {
    return ParseNode::create(type);
  }

  ParseNode* createNode(const Token& token)
  {
    ParseNode* pNode = ParseNode::create(token);
    pStatus_->recordNodeLocation(token.position(), pNode);
    return pNode;
  }

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

  ParseNode* parse(ParseStatus* pStatus)
  {
    pStatus_ = pStatus;
    ParseNode* root = createNode(tokens::ROOT);

    while (true)
    {
      ParseNode* pNode = parseExpression();
      if (!pNode)
        break;

      root->add(pNode);
    }

    return root;
  }

};

} // namespace parser

void log(parser::ParseNode* pNode, int depth = 0);

} // namespace sourcetools

#endif /* SOURCETOOLS_PARSE_PARSER_H */
