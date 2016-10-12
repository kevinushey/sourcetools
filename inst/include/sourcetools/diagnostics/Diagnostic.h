#ifndef SOURCETOOLS_DIAGNOSTICS_DIAGNOSTIC_H
#define SOURCETOOLS_DIAGNOSTICS_DIAGNOSTIC_H

#include <vector>
#include <string>

#include <sourcetools/r/r.h>
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

namespace r {

inline SEXP create(diagnostics::DiagnosticType type)
{
  using namespace diagnostics;

  switch (type)
  {
  case DIAGNOSTIC_ERROR:   return Rf_mkString("error");
  case DIAGNOSTIC_WARNING: return Rf_mkString("warning");
  case DIAGNOSTIC_INFO:    return Rf_mkString("info");
  case DIAGNOSTIC_STYLE:   return Rf_mkString("style");
  }

  // happy compiler
  return Rf_mkString("error");
}

inline SEXP create(const diagnostics::Diagnostic& diagnostic)
{
  using namespace diagnostics;

  ListBuilder builder;

  builder.add("type",    create(diagnostic.type()));
  builder.add("file",    Rf_mkString(""));
  builder.add("line",    Rf_ScalarInteger(diagnostic.start().row));
  builder.add("column",  Rf_ScalarInteger(diagnostic.start().column));
  builder.add("message", Rf_mkString(diagnostic.message()));

  return builder;
}

inline SEXP create(const std::vector<diagnostics::Diagnostic>& diagnostics)
{
  using namespace diagnostics;

  Protect protect;
  index_type n = diagnostics.size();
  SEXP resultSEXP = protect(Rf_allocVector(VECSXP, n));
  for (index_type i = 0; i < n; ++i)
    SET_VECTOR_ELT(resultSEXP, i, create(diagnostics[i]));
  return resultSEXP;
}

} // namespace r

} // namespace sourcetools

#endif /* SOURCETOOLS_DIAGNOSTICS_DIAGNOSTIC_H */
