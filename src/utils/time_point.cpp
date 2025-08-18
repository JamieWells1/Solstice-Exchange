#include <time_point.h>

TimePoint getTimeNow() { return std::chrono::system_clock::now(); }
