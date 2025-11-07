#ifndef TIMEPOINT_H
#define TIMEPOINT_H

#include <chrono>

using TimePoint = std::chrono::system_clock::time_point;

struct CurrentDate
{
    int year;
    int month;
    int day;
};

TimePoint timeNow();

CurrentDate currentDate();

int monthToInt(const std::string& month);

#endif  // TIMEPOINT_H
