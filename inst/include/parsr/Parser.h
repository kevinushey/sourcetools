#ifndef PARSR_PARSER_H
#define PARSR_PARSER_H

#include <string>

#include <parsr/collections/Position.h>

namespace parsr {

struct ParseItem
{
  std::string::const_iterator begin;
  std::string::const_iterator end;
};

class ParseNode
{
};

class Parser
{
public:
  explicit Parser()
};

}  // namespace parsr

#endif
