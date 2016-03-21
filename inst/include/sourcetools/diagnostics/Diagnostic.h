#ifndef SOURCE_TOOLS_DIAGNOSTICS_DIAGNOSTIC_H
#define SOURCE_TOOLS_DIAGNOSTICS_DIAGNOSTIC_H

#include <vector>
#include <string>

#include <sourcetools/collection/collection.h>

namespace sourcetools {
namespace diagnostics {

enum DiagnosticType
{
  DIAGNOSTIC_ERROR,
  DIAGNOSTIC_WARNING,
  DIAGNOSTIC_INFO,
  DIAGNOSTIC_STYLE
};

class Diagnostic
{
public:
  Diagnostic(DiagnosticType type,
             const std::string& message,
             const collections::Range& range)
    : type_(type), message_(message), range_(range)
  {
  }

  const std::string message() const { return message_; }
  DiagnosticType type() const { return type_; }
  collections::Range range() const { return range_; }
  collections::Position start() const { return range_.start(); }
  collections::Position end() const { return range_.end(); }

private:
  DiagnosticType type_;
  std::string message_;
  collections::Range range_;
};

class Diagnostics
{
  typedef collections::Range Range;

public:

  void add(DiagnosticType type, const std::string& message, const Range& range)
  {
    diagnostics_.push_back(Diagnostic(type, message, range));
  }

  void addError(const std::string& message, const Range& range)
  {
    add(DIAGNOSTIC_ERROR, message, range);
  }

  void addWarning(const std::string& message, const Range& range)
  {
    add(DIAGNOSTIC_WARNING, message, range);
  }

  void addInfo(const std::string& message, const Range& range)
  {
    add(DIAGNOSTIC_INFO, message, range);
  }

  operator const std::vector<Diagnostic>&() const { return diagnostics_; }

private:
  std::vector<Diagnostic> diagnostics_;
};

} // namespace diagnostics
} // namespace sourcetools

#endif /* SOURCE_TOOLS_DIAGNOSTICS_DIAGNOSTIC_H */
