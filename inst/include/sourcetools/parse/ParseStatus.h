#ifndef SOURCE_TOOLS_PARSE_PARSE_STATUS_H
#define SOURCE_TOOLS_PARSE_PARSE_STATUS_H

#include <sourcetools/collection/Position.h>

#include <sourcetools/parse/ParseError.h>

namespace sourcetools {
namespace parser {

class Node;

class ParseStatus
{
  typedef collections::Position Position;

public:
  ParseStatus() {}

  void recordNodeLocation(const Position& position,
                          Node* pNode)
  {
    map_[position] = pNode;
  }

  Node* getNodeAtPosition(const Position& position)
  {
    return map_[position];
  }

  void addError(const ParseError& error)
  {
    errors_.push_back(error);
  }

  const std::vector<ParseError>& getErrors() const
  {
    return errors_;
  }

private:
  std::map<Position, Node*> map_;
  std::vector<ParseError> errors_;
};
} // namespace parser
} // namespace sourcetools

#endif /* SOURCE_TOOLS_PARSE_PARSE_STATUS_H */
