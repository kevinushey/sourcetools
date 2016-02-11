#include <sourcetools.h>

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

namespace sourcetools {

void log(std::shared_ptr<parser::Node> pNode, int depth)
{
  for (std::size_t i = 0; i < depth; ++i)
    ::Rprintf("  ");

  auto&& token = pNode->token();
  ::Rprintf("[%i:%i]: %s\n",
            (int) token.row(),
            (int) token.column(),
            token.contents().c_str());

  for (auto&& child : pNode->children())
    log(child, depth + 1);
}

} // namespace sourcetools

extern "C" SEXP sourcetools_parse_string(SEXP stringSEXP)
{
  const char* string = CHAR(STRING_ELT(stringSEXP, 0));
  auto root = sourcetools::parse(string);
  sourcetools::log(root);
  return R_NilValue;
}
