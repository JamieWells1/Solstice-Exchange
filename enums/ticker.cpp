#include <ticker.h>

Ticker validateTkr(const std::string& tkrStr) {
    auto iterator = kTickerMap.find(tkrStr);

    if (iterator != kTickerMap.end()) {
        return iterator->second;
    }
    return Ticker::INVALID;
}

std::ostream& operator<<(std::ostream& os, Ticker ticker) {
    auto it = kTickerNameMap.find(ticker);
    return os << (it != kTickerNameMap.end() ? it->second : "UNKNOWN");
}
