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

public:

  explicit Node(const Token& token)
    : token_(token), parent_(NULL)
  {
  }

  explicit Node(const TokenType& type)
    : token_(Token(type)), parent_(NULL)
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

  static void destroy(const Node* pNode)
  {
    for (Children::const_iterator it = pNode->children().begin();
         it != pNode->children().end();
         ++it)
    {
      destroy(*it);
    }

    delete pNode;
  }

  void remove(const Node* pNode)
  {
    children_.erase(
      std::remove(children_.begin(), children_.end(), pNode),
      children_.end());
  }

  void add(Node* pNode)
  {
    if (pNode->parent())
      pNode->parent_->remove(pNode);

    pNode->parent_ = this;
    children_.push_back(pNode);
  }

private:
  class FindExpressionStart
  {
  public:
    FindExpressionStart(Position* pStart, Position* pEnd)
      : pStart_(pStart), pEnd_(pEnd)
    {
    }

    void operator()(const Node* pNode)
    {
      Position position = pNode->token().position();
      if (position < *pStart_)
        *pStart_ = position;
      else if (position > *pEnd_)
        *pEnd_ = position;
    }

  private:
    Position* pStart_;
    Position* pEnd_;
  };

public:
  Range range() const
  {
    Position startPosition = token_.position();
    Position endPosition   = token_.position();
    FindExpressionStart f(&startPosition, &endPosition);
    std::for_each(begin(), end(), f);
    return Range(startPosition, endPosition);
  }

  const Children::const_iterator begin() const { return children_.begin(); }
  const Children::const_iterator end() const { return children_.end(); }

  const Token& token() const { return token_; }
  const Node* parent() const { return parent_; }
  const Children& children() const { return children_; }
};

} // namespace parser
} // namespace sourcetools

#endif /* SOURCE_TOOLS_PARSE_NODE_H */
