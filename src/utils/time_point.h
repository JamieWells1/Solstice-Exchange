#ifndef TIMEPOINT_H
#define TIMEPOINT_H

#include <chrono>

using TimePoint = std::chrono::system_clock::time_point;

TimePoint getTimeNow();

#endif  // TIMEPOINT_H
