#ifndef PARSR_PARSER_H
#define PARSR_PARSER_H

namespace parsr {

struct ParseItem
{
  const char* symbol;
  std::size_t row;
  std::size_t column;
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
