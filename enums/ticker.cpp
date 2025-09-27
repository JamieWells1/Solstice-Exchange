#include <ticker.h>

#include <random>

namespace solstice
{

Ticker validateTkr(const std::string& tkrStr)
{
    auto it = kTickerMap.find(tkrStr);
    return (it != kTickerMap.end()) ? it->second : Ticker::INVALID;
}

const std::string tkrToString(Ticker tkr)
{
    auto it = kTickerNameMap.find(tkr);
    if (it != kTickerNameMap.end()) return it->second;
    return "UNKNOWN";
}

Ticker getRandomTkr()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    int numberOfTkrs = Config::d_numberOfTkrs;

    std::uniform_int_distribution<> dist(
        0, kValidTickers.size() - 2);  // exclude INVALID
    return kValidTickers[dist(gen)];
}

std::ostream& operator<<(std::ostream& os, Ticker tkr)
{
    return os << tkrToString(tkr);
}
}  // namespace solstice
