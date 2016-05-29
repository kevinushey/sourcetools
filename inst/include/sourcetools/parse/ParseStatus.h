#ifndef SOURCETOOLS__PARSE__PARSE_STATUS_H
#define SOURCETOOLS__PARSE__PARSE_STATUS_H

#include <sourcetools/collection/Position.h>

#include <sourcetools/parse/ParseError.h>

namespace sourcetools {
namespace parser {

class ParseNode;

class ParseStatus
{
  typedef collections::Position Position;

public:
  ParseStatus() {}

  void recordNodeLocation(const Position& position,
                          ParseNode* pNode)
  {
    map_[position] = pNode;
  }

  ParseNode* getNodeAtPosition(const Position& position)
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
  std::map<Position, ParseNode*> map_;
  std::vector<ParseError> errors_;
};
} // namespace parser
} // namespace sourcetools

#endif /* SOURCETOOLS__PARSE__PARSE_STATUS_H */
