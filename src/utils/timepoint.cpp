#include <timepoint.h>

TimePoint getTimeNow() {
    return std::chrono::system_clock::now();
}
