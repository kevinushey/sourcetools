#ifndef SOURCETOOLS_PARSE_PARSE_NODE_H
#define SOURCETOOLS_PARSE_PARSE_NODE_H

#include <map>
#include <memory>

#include <sourcetools/collection/collection.h>
#include <sourcetools/tokenization/tokenization.h>

namespace sourcetools {
namespace parser {

class ParseNode
{
public:
  typedef collections::Position Position;
  typedef collections::Range Range;
  typedef tokens::Token Token;
  typedef tokens::TokenType TokenType;

private:
  Token token_;
  ParseNode* parent_;
  std::vector<ParseNode*> children_;

  Token begin_;
  Token end_;

public:

  explicit ParseNode(const Token& token)
    : token_(token), parent_(NULL),
      begin_(token), end_(token)
  {
  }

  static ParseNode* create(const Token& token)
  {
    return new ParseNode(token);
  }

  static ParseNode* create(const TokenType& type)
  {
    static std::map<TokenType, Token> tokens;
    if (!tokens.count(type))
      tokens[type] = Token(type);

    const Token& token = tokens[type];
    return new ParseNode(token);
  }

  ~ParseNode()
  {
    for (std::vector<ParseNode*>::const_iterator it = children_.begin();
         it != children_.end();
         ++it)
    {
      delete *it;
    }
  }

  void remove(const ParseNode* pNode)
  {
    children_.erase(
      std::remove(children_.begin(), children_.end(), pNode),
      children_.end());
  }

  void add(ParseNode* pNode)
  {
    if (pNode->parent_ != NULL)
      pNode->parent_->remove(pNode);
    pNode->parent_ = this;

    const Token& begin = pNode->begin();
    const Token& end   = pNode->end();
    if (begin.offset() != -1 && end.offset() != -1)
    {
      for (ParseNode* pParent = this; pParent != NULL; pParent = pParent->parent_)
      {
        if (begin.begin() < pParent->begin().begin())
          pParent->setBegin(begin);
        if (end.end() > pParent->end().end())
          pParent->setEnd(end);
      }
    }

    children_.push_back(pNode);
  }

  const Token& begin() const
  {
    return begin_;
  }

  void setBegin(const Token& begin)
  {
    for (ParseNode* pNode = this; pNode != NULL; pNode = pNode->parent_)
      if (begin.begin() < pNode->begin().begin())
        pNode->begin_ = begin;
  }

  const Token& end() const
  {
    return end_;
  }

  void setEnd(const Token& end)
  {
    end_ = end;
    for (ParseNode* pNode = this; pNode != NULL; pNode = pNode->parent_)
      if (end.end() > pNode->end().end())
        pNode->end_ = end;
  }

  void bounds(const char** begin, const char** end)
  {
    *begin = begin_.begin();
    *end   = end_.end();
  }

  Range range() const
  {
    return Range(begin_.position(), end_.position() + end_.size());
  }

  const Token& token() const { return token_; }
  const ParseNode* parent() const { return parent_; }
  const std::vector<ParseNode*>& children() const { return children_; }
};

} // namespace parser
} // namespace sourcetools

#endif /* SOURCETOOLS_PARSE_PARSE_NODE_H */
