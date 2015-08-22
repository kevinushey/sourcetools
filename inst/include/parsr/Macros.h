#ifndef PARSR_MACROS_H
#define PARSR_MACROS_H

#ifdef PARSR_ENABLE_DEBUG_LOGGING

#include <iostream>

#define DEBUG(x) std::cerr << x << std::endl
#define DEBUG_BLOCK(x)

#else

#define DEBUG(x)
#define DEBUG_BLOCK(x) if (false)

#endif

#endif  // PARSR_MACROS_H