#ifndef SOURCE_TOOLS_PARSE_NODE_H
#define SOURCE_TOOLS_PARSE_NODE_H

#include <memory>

#include <sourcetools/tokenization/tokenization.h>

namespace sourcetools {
namespace parser {

class Node
{
private:
  typedef tokens::Token Token;
  typedef tokens::TokenType TokenType;
  typedef std::vector<std::shared_ptr<Node>> Children;

  Token token_;
  Node* parent_;
  Children children_;

public:

  explicit Node(const Token& token)
    : token_(token), parent_(nullptr)
  {
  }

  explicit Node(const TokenType& type)
    : token_(Token(type)), parent_(nullptr)
  {
  }

  static std::shared_ptr<Node> create(const Token& token)
  {
    return std::make_shared<Node>(token);
  }

  static std::shared_ptr<Node> create(const TokenType& type)
  {
    return std::make_shared<Node>(type);
  }

  void remove(const std::shared_ptr<Node>& pNode)
  {
    children_.erase(
      std::remove(children_.begin(), children_.end(), pNode),
      children_.end());
  }

  void add(const std::shared_ptr<Node>& pNode)
  {
    if (pNode->parent())
      pNode->parent_->remove(pNode);

    pNode->parent_ = this;
    children_.emplace_back(pNode);
  }

  const Token& token() const { return token_; }
  const Node* parent() const { return parent_; }
  const Children& children() const { return children_; }
};

} // namespace parser
} // namespace sourcetools

#endif /* SOURCE_TOOLS_PARSE_NODE_H */
