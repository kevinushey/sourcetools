#ifndef SOURCETOOLS_R_R_UTILS_H
#define SOURCETOOLS_R_R_UTILS_H

#include <vector>

#include <sourcetools/core/core.h>

#include <sourcetools/r/RHeaders.h>
#include <sourcetools/r/RProtect.h>
#include <sourcetools/r/RConverter.h>

namespace sourcetools {
namespace r {

class RObjectFactory : noncopyable
{
public:

  RObjectFactory()
    : n_(0)
  {
  }

  template <typename T, typename F>
  SEXP create(SEXPTYPE type, const std::vector<T>& vector, F f)
  {
    ++n_;
    index_type n = vector.size();
    SEXP resultSEXP = PROTECT(Rf_allocVector(type, n));
    for (index_type i = 0; i < n; ++i)
      f(resultSEXP, i, vector[i]);
    return resultSEXP;
  }

  SEXP create(SEXPTYPE type, index_type n)
  {
    ++n_;
    return PROTECT(Rf_allocVector(type, n));
  }

  ~RObjectFactory()
  {
    UNPROTECT(n_);
  }

private:
  index_type n_;
};

class ListBuilder : noncopyable
{
public:

  void add(const std::string& name, SEXP value)
  {
    names_.push_back(name);
    data_.push_back(protect_(value));
  }

  operator SEXP() const
  {
    index_type n = data_.size();

    SEXP resultSEXP = protect_(Rf_allocVector(VECSXP, n));
    SEXP namesSEXP  = protect_(Rf_allocVector(STRSXP, n));

    for (index_type i = 0; i < n; ++i)
    {
      SET_VECTOR_ELT(resultSEXP, i, data_[i]);
      SET_STRING_ELT(namesSEXP, i, createChar(names_[i]));
    }

    Rf_setAttrib(resultSEXP, R_NamesSymbol, namesSEXP);
    return resultSEXP;
  }

private:
  std::vector<std::string> names_;
  std::vector<SEXP> data_;
  mutable Protect protect_;
};

} // namespace r
} // namespace sourcetools

#endif /* SOURCETOOLS_R_R_UTILS_H */
