#ifndef SOURCETOOLS_COMPLETION_CODE_COMPLETION_H
#define SOURCETOOLS_COMPLETION_CODE_COMPLETION_H

#include <vector>
#include <string>

#include <sourcetools/parse/parse.h>

namespace sourcetools {
namespace completion {

enum CompletionType
{
  CompletionTypeUnknown
};

class Completion
{
public:
  Completion(const std::string& value, CompletionType type)
    : value_(value), type_(type)
  {
  }

private:
  std::string value_;
  CompletionType type_;
};

std::vector<Completion> completions(const char* code,
                                    std::size_t n,
                                    const collections::Position& position)
{
  std::vector<Completion> completions;

  // TODO:
  //
  // 1) produce parse tree
  // 2) get node at position (note: token immediately before position?)
  // 3) figure out completion context type
  //    ('$', '@', file, identifier, special context, etc)
  // 4) dispatch to appropriate completer for context
  // 5) return completions

  return completions;
}

} // namespace completion
} // namespace sourcetools

#endif /* SOURCETOOLS_COMPLETION_CODE_COMPLETION_H */
