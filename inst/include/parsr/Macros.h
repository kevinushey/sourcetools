#ifndef PARSR_MACROS_H
#define PARSR_MACROS_H

#ifdef __GNUC__
# define LIKELY(x)   __builtin_expect(!!(x), 1)
# define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
# define LIKELY(x) x
# define UNLIKELY(x) x
#endif

#ifdef PARSR_ENABLE_DEBUG_LOGGING

#include <iostream>

#define LOG(x) std::cerr << x << std::endl
#define DEBUG_BLOCK(x)

#else

#define LOG(x)
#define DEBUG_BLOCK(x) if (false)

#endif

#endif  // PARSR_MACROS_H
