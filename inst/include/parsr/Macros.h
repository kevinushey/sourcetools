#ifndef SOURCE_TOOLS_MACROS_H
#define SOURCE_TOOLS_MACROS_H

/* Utility */
#ifdef __GNUC__
# define LIKELY(x)   __builtin_expect(!!(x), 1)
# define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
# define LIKELY(x) x
# define UNLIKELY(x) x
#endif

#define SOURCE_TOOLS_CHECK_MASK(__SELF__, __MASK__)                   \
  ((__MASK__ & __SELF__) == __MASK__)

#define SOURCE_TOOLS_LOWER_BITS(__VALUE__, __BITS__)                   \
  (((1 << __BITS__) - 1) & __VALUE__)

#define SOURCE_TOOLS_PASTE(__X__, __Y__) __X__ ## __Y__
#define SOURCE_TOOLS_STRINGIFY(__X__) #__X__

/* Logging */
#ifdef SOURCE_TOOLS_ENABLE_DEBUG_LOGGING

#include <iostream>

#define LOG(x) std::cerr << x << std::endl
#define DEBUG_BLOCK(x)

#else

#define LOG(x)
#define DEBUG_BLOCK(x) if (false)

#endif

#endif  // SOURCE_TOOLS_MACROS_H
