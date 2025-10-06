#ifndef LOGGING_LEVEL_H
#define LOGGING_LEVEL_H

#include <cstdint>

enum class LogLevel : uint8_t
{
    ERROR = 0,
    WARNING = 1,
    INFO = 2,
    DEBUG = 3,
};

#endif // LOGGING_LEVEL_H
