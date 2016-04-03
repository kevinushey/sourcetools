#ifndef SOURCE_TOOLS_PARSE_NODE_H
#define SOURCE_TOOLS_PARSE_NODE_H

#include <memory>

#include <sourcetools/collection/collection.h>
#include <sourcetools/tokenization/tokenization.h>

namespace sourcetools {
namespace parser {

class Node
{
public:
  typedef collections::Position Position;
  typedef collections::Range Range;
  typedef tokens::Token Token;
  typedef tokens::TokenType TokenType;
  typedef std::vector<Node*> Children;

private:
  Token token_;
  Node* parent_;
  Children children_;

  Token begin_;
  Token end_;
  bool pseudoNode_;

public:

  explicit Node(const Token& token)
    : token_(token), parent_(NULL),
      begin_(token), end_(token), pseudoNode_(false)
  {
  }

  explicit Node(const TokenType& type)
    : token_(Token(type)), parent_(NULL),
      begin_(token_), end_(token_), pseudoNode_(true)
  {
  }

  static Node* create(const Token& token)
  {
    return new Node(token);
  }

  static Node* create(const TokenType& type)
  {
    return new Node(type);
  }

  ~Node()
  {
    for (Children::const_iterator it = children_.begin();
         it != children_.end();
         ++it)
    {
      delete *it;
    }
  }

  void remove(const Node* pNode)
  {
    children_.erase(
      std::remove(children_.begin(), children_.end(), pNode),
      children_.end());
  }

  void add(Node* pNode)
  {
    if (pNode->parent_ != NULL)
      pNode->parent_->remove(pNode);
    pNode->parent_ = this;

    if (!pNode->isPseudoNode())
    {
      const Token& begin = pNode->begin();
      const Token& end   = pNode->end();
      for (Node* pParent = this; pParent != NULL; pParent = pParent->parent_)
      {
        if (begin.begin() < pParent->begin().begin())
          pParent->setBegin(begin);
        if (end.end() > pParent->end().end())
          pParent->setEnd(end);
      }
    }

    children_.push_back(pNode);
  }

  const Token& begin() const { return begin_; }
  void setBegin(const Token& begin)
  {
    for (Node* pNode = this; pNode != NULL; pNode = pNode->parent_)
      if (begin.begin() < pNode->begin().begin())
        pNode->begin_ = begin;
  }

  const Token& end() const { return end_; }
  void setEnd(const Token& end)
  {
    end_ = end;
    for (Node* pNode = this; pNode != NULL; pNode = pNode->parent_)
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
  const Node* parent() const { return parent_; }
  const Children& children() const { return children_; }

  bool isPseudoNode() const { return pseudoNode_; }
};

} // namespace parser
} // namespace sourcetools

#endif /* SOURCE_TOOLS_PARSE_NODE_H */
