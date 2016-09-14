#include <cstring>

#include <sourcetools/read/read.h>
#include <sourcetools/r/r.h>

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

extern "C" SEXP sourcetools_read(SEXP absolutePathSEXP)
{
  const char* absolutePath = CHAR(STRING_ELT(absolutePathSEXP, 0));

  std::string contents;
  bool result = sourcetools::read(absolutePath, &contents);
  if (!result)
  {
    Rf_warning("Failed to read file");
    return R_NilValue;
  }

  sourcetools::r::Protect protect;
  SEXP resultSEXP = protect(Rf_allocVector(STRSXP, 1));
  SET_STRING_ELT(resultSEXP, 0, Rf_mkCharLen(contents.c_str(), contents.size()));
  return resultSEXP;
}

extern "C" SEXP sourcetools_read_lines(SEXP absolutePathSEXP)
{
  const char* absolutePath = CHAR(STRING_ELT(absolutePathSEXP, 0));

  std::vector<std::string> lines;
  bool result = sourcetools::read_lines(absolutePath, &lines);
  if (!result)
  {
    Rf_warning("Failed to read file");
    return R_NilValue;
  }

  std::size_t n = lines.size();
  sourcetools::r::Protect protect;
  SEXP resultSEXP = protect(Rf_allocVector(STRSXP, n));
  for (std::size_t i = 0; i < n; ++i)
    SET_STRING_ELT(resultSEXP, i, Rf_mkCharLen(lines[i].c_str(), lines[i].size()));
  return resultSEXP;
}

extern "C" SEXP sourcetools_read_bytes(SEXP absolutePathSEXP)
{
  const char* absolutePath = CHAR(STRING_ELT(absolutePathSEXP, 0));

  std::string contents;
  bool result = sourcetools::read(absolutePath, &contents);
  if (!result)
  {
    Rf_warning("Failed to read file");
    return R_NilValue;
  }

  sourcetools::r::Protect protect;
  SEXP resultSEXP = protect(Rf_allocVector(RAWSXP, contents.size()));
  std::memcpy(RAW(resultSEXP), contents.c_str(), contents.size());
  return resultSEXP;
}

extern "C" SEXP sourcetools_read_lines_bytes(SEXP absolutePathSEXP)
{
  const char* absolutePath = CHAR(STRING_ELT(absolutePathSEXP, 0));

  std::vector<std::string> lines;
  bool result = sourcetools::read_lines(absolutePath, &lines);
  if (!result)
  {
    Rf_warning("Failed to read file");
    return R_NilValue;
  }

  std::size_t n = lines.size();
  sourcetools::r::Protect protect;
  SEXP resultSEXP = protect(Rf_allocVector(VECSXP, n));
  for (std::size_t i = 0; i < n; ++i)
  {
    SEXP rawSEXP = Rf_allocVector(RAWSXP, lines[i].size());
    std::memcpy(RAW(rawSEXP), lines[i].c_str(), lines[i].size());
    SET_VECTOR_ELT(resultSEXP, i, rawSEXP);
  }
  return resultSEXP;
}
