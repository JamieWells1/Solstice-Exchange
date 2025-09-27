#include <ticker.h>

namespace solstice
{

Ticker validateTkr(const std::string& tkrStr)
{
    auto iterator = kTickerMap.find(tkrStr);

    if (iterator != kTickerMap.end())
    {
        return iterator->second;
    }
    return Ticker::INVALID;
}

inline std::string to_string(Ticker tkr)
{
    auto it = kTickerNameMap.find(tkr);
    if (it != kTickerNameMap.end()) return it->second;
    return "UNKNOWN";
}

std::ostream& operator<<(std::ostream& os, Ticker ticker)
{
    auto it = kTickerNameMap.find(ticker);
    return os << (it != kTickerNameMap.end() ? it->second : "UNKNOWN");
}
}  // namespace solstice
