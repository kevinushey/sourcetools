#ifndef SOURCETOOLS_R_RPROTECT_H
#define SOURCETOOLS_R_RPROTECT_H

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
    ++n_;
    return PROTECT(objectSEXP);
  }

private:
  int n_;
};

} // end namespace r
} // end namespace sourcetools

#endif /* SOURCETOOLS_R_RPROTECT_H */
