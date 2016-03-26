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

  void bounds(const char** begin, const char** end)
  {
    *begin = token_.begin();
    *end   = token_.end();

    const Node* pNode;

    pNode = this;
    while (true)
    {
      const Children& children = pNode->children();
      if (children.empty())
        break;

      pNode = children[0];
      const Token& token = pNode->token();
      if (token.begin() < *begin)
        *begin = token.begin();
      else
        break;
    }

    pNode = this;
    while (true)
    {
      const Children& children = pNode->children();
      if (children.empty())
        break;

      pNode = children[children.size() - 1];
      const Token& token = pNode->token();
      if (token.end() > *end)
        *end = token.end();
      else
        break;
    }
  }

  Range range() const
  {
    Position startPosition = token_.position();
    Position endPosition   = token_.position();

    const Node* pNode;

    pNode = this;
    while (true)
    {
      const Children& children = pNode->children();
      if (children.empty())
        break;

      pNode = children[0];
      const Position& candidatePosition = pNode->token().position();
      if (candidatePosition < startPosition)
        startPosition = candidatePosition;
      else
        break;
    }

    pNode = this;
    while (true)
    {
      const Children& children = pNode->children();
      if (children.empty())
        break;

      pNode = children[children.size() - 1];
      const Position& candidatePosition = pNode->token().position();
      if (candidatePosition > startPosition)
        endPosition = candidatePosition;
      else
        break;
    }

    return Range(startPosition, endPosition);
  }

  const Children::const_iterator begin() const { return children_.begin(); }
  const Children::const_iterator end() const { return children_.end(); }

  const Token& token() const { return token_; }
  const Node* parent() const { return parent_; }
  const Children& children() const { return children_; }
};

} // namespace parser

inline void destroy(const parser::Node* pNode)
{
  parser::Node::destroy(pNode);
}

} // namespace sourcetools

#endif /* SOURCE_TOOLS_PARSE_NODE_H */
