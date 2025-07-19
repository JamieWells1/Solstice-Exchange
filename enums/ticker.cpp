#include <ticker.h>

Ticker validateTkr(const std::string& tkrStr) {
    auto iterator = kTickerMap.find(tkrStr);

    if (iterator != kTickerMap.end()) {
        return iterator->second;
    }
    return Ticker::INVALID;
}
