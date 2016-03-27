#include <sourcetools.h>
#include <testthat.h>

#include <R.h>
#include <Rinternals.h>

std::string type(wchar_t ch)
{
  std::string result;
  if (std::iswcntrl(ch))
    result += "cntrl,";
  if (std::iswprint(ch))
    result += "print,";
  if (std::iswspace(ch))
    result += "space,";
  if (std::iswblank(ch))
    result += "blank,";
  if (std::iswgraph(ch))
    result += "graph,";
  if (std::iswpunct(ch))
    result += "punct,";
  if (std::iswalnum(ch))
    result += "alnum,";
  if (std::iswalpha(ch))
    result += "alpha,";
  if (std::iswupper(ch))
    result += "upper,";
  if (std::iswlower(ch))
    result += "lower,";
  if (std::iswdigit(ch))
    result += "digit,";
  if (std::iswxdigit(ch))
    result += "xdigit,";

  if (!result.empty())
    result = result.substr(0, result.size() - 1);

  return result;
}

extern "C" SEXP sourcetools_print_multibyte(SEXP dataSEXP)
{
  const char* data = CHAR(STRING_ELT(dataSEXP, 0));
  std::size_t size = Rf_length(STRING_ELT(dataSEXP, 0));

  wchar_t ch;
  const char* it = data;
  while (true)
  {
    int length = std::mbtowc(&ch, it, MB_CUR_MAX);
    if (length == 0)
      break;

    if (length == -1)
    {
      Rf_warning("Invalid multibyte character at index %i\n", it - data);
      ++it;
      continue;
    }

    std::string type = ::type(ch);
    ::Rprintf("%5i: [%s,%i] '%lc'\n",
              (int) ch,
              type.c_str(),
              length,
              ch);

    it += length;
  }

  return R_NilValue;
}

extern "C" SEXP sourcetools_print_utf8(SEXP dataSEXP)
{
  using namespace sourcetools;

  const char* data = CHAR(STRING_ELT(dataSEXP, 0));
  utf8::iterator it(data);

  wchar_t ch = *it++;
  while (true)
  {
    wchar_t ch = *it++;
    if (ch == 0 || ch == -1)
      break;
    ::Rprintf("[%i]: %lc\n", (int) ch, ch);
  }

  return R_NilValue;

}
