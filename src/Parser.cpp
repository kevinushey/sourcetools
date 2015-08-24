#include <parsr.h>

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

extern "C" SEXP parsr_parse_string(const std::string& string)
{
  using namespace parsr::parser;

  Parser parser(string);
  ParseNode node = parser.parse();

  return R_NilValue;
}
