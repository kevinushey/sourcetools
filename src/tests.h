#ifndef SOURCETOOLS_TESTS_H
#define SOURCETOOLS_TESTS_H

// Disable testthat when running with gcc 4.4
// as it cannot handle some of the idioms used
// by catch
#if defined(__GNUC__) && defined(__GNUC_MINOR__) && !defined(__clang__)
# if __GNUC__ < 4
#  define TESTTHAT_DISABLED
# elif __GNUC__ == 4 && __GNUC_MINOR__ < 6
#  define TESTTHAT_DISABLED
# endif
#endif

#include <testthat.h>

#endif

