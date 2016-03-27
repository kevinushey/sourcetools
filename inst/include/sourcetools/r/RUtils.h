#ifndef SOURCE_TOOLS_R_UTILS_H
#define SOURCE_TOOLS_R_UTILS_H

#include <vector>

#include <sourcetools/core/core.h>

#include <sourcetools/r/RHeaders.h>

namespace sourcetools {
namespace r {

class Protect : noncopyable
{
public:
  Protect(): n_(0) {}
  ~Protect() { UNPROTECT(n_); }

  SEXP operator()(SEXP objectSEXP)
  {
    PROTECT(objectSEXP);
    ++n_;
    return objectSEXP;
  }

private:
  std::size_t n_;
};

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
    n_ += 1;
    std::size_t n = vector.size();
    SEXP resultSEXP = PROTECT(Rf_allocVector(type, n));
    for (std::size_t i = 0; i < n; ++i)
      f(resultSEXP, i, vector[i]);
    return resultSEXP;
  }

  SEXP create(SEXPTYPE type, std::size_t n)
  {
    n_ += 1;
    return PROTECT(Rf_allocVector(type, n));
  }

  ~RObjectFactory()
  {
    UNPROTECT(n_);
  }

private:
  std::size_t n_;
};

} // namespace r
} // namespace sourcetools

#endif /* SOURCE_TOOLS_R_UTILS_H */
