#ifndef SOURCETOOLS__PARSE__PARSE_NODE_H
#define SOURCETOOLS__PARSE__PARSE_NODE_H

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
  bool pseudoNode_;

public:

  explicit ParseNode(const Token& token)
    : token_(token), parent_(NULL),
      begin_(token), end_(token), pseudoNode_(false)
  {
  }

  explicit ParseNode(const TokenType& type)
    : token_(Token(type)), parent_(NULL),
      begin_(token_), end_(token_), pseudoNode_(true)
  {
  }

  static ParseNode* create(const Token& token)
  {
    return new ParseNode(token);
  }

  static ParseNode* create(const TokenType& type)
  {
    return new ParseNode(type);
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

    if (!pNode->isPseudoNode())
    {
      const Token& begin = pNode->begin();
      const Token& end   = pNode->end();
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

  bool isPseudoNode() const { return pseudoNode_; }
};

} // namespace parser
} // namespace sourcetools

#endif /* SOURCETOOLS__PARSE__PARSE_NODE_H */
