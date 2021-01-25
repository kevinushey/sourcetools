#ifndef SOURCETOOLS_R_R_CONVERTER_H
#define SOURCETOOLS_R_R_CONVERTER_H

#include <vector>
#include <string>

#include <sourcetools/r/RHeaders.h>
#include <sourcetools/r/RProtect.h>

namespace sourcetools {
namespace r {

inline SEXP createChar(const std::string& data)
{
  return Rf_mkCharLenCE(data.c_str(), data.size(), CE_UTF8);
}

inline SEXP createString(const std::string& data)
{
  Protect protect;
  SEXP resultSEXP = protect(Rf_allocVector(STRSXP, 1));
  SET_STRING_ELT(resultSEXP, 0, createChar(data));
  return resultSEXP;
}

inline SEXP create(const std::vector<std::string>& vector)
{
  Protect protect;
  index_type n = vector.size();
  SEXP resultSEXP = protect(Rf_allocVector(STRSXP, n));
  for (index_type i = 0; i < n; ++i)
    SET_STRING_ELT(resultSEXP, i, createChar(vector[i]));
  return resultSEXP;
}

} // namespace r
} // namespace sourcetools

#endif /* SOURCETOOLS_R_R_CONVERTER_H */
