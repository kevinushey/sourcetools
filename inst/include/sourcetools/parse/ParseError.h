#ifndef SOURCETOOLS_PARSE_PARSE_ERROR_H
#define SOURCETOOLS_PARSE_PARSE_ERROR_H

#include <sourcetools/collection/collection.h>
#include <sourcetools/tokenization/tokenization.h>

namespace sourcetools {
namespace parser {

class ParseError
{
  typedef collections::Position Position;
  typedef tokens::Token Token;

  Position start_;
  Position end_;
  std::string message_;

public:

  ParseError(const tokens::Token& token, std::string message)
    : start_(token.position()),
      end_(token.position()),
      message_(std::move(message))
  {
    end_.column += token.end() - token.begin();
  }

  ParseError(Position start, Position end, std::string message)
    : start_(std::move(start)),
      end_(std::move(end)),
      message_(std::move(message))
  {
  }

  const Position& start() const { return start_; }
  const Position& end() const { return end_; }
  const std::string& message() const { return message_; }
};

} // namespace parser
} // namespace sourcetools

#endif /* SOURCETOOLS_PARSE_PARSE_ERROR_H */
