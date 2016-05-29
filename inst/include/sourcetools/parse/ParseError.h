#ifndef SOURCETOOLS__PARSE__PARSE_ERROR_H
#define SOURCETOOLS__PARSE__PARSE_ERROR_H

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

  ParseError(const tokens::Token& token,
             const std::string& message)
    : start_(token.position()),
      end_(token.position()),
      message_(message)
  {
    end_.column += token.end() - token.begin();
  }

  ParseError(const Position& start,
             const Position& end,
             const std::string& message)
    : start_(start),
      end_(end),
      message_(message)
  {
  }

  explicit ParseError(const std::string& message)
    : start_(0, 0),
      end_(0, 0),
      message_(message)
  {
  }

  const Position& start() const { return start_; }
  const Position& end() const { return end_; }
  const std::string& message() const { return message_; }
};

} // namespace parser
} // namespace sourcetools

#endif /* SOURCETOOLS__PARSE__PARSE_ERROR_H */
