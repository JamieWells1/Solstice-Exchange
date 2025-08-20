#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>

#ifdef ENABLE_LOGGING
#define LOG(x) std::cout << x << std::endl
#else
#define LOG(x) \
    do               \
    {                \
    } while (0)
#endif

#endif  // LOGGING_H
