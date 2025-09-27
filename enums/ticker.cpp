#include <ticker.h>

#include <random>
#include <algorithm>

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

const std::vector<Ticker>& getTickerPool(int tkrPoolCount)
{
    static std::vector<Ticker> tickerPool;
    static bool d_tkrPoolInitialized = false;

    if (!d_tkrPoolInitialized) {
        d_tkrPoolInitialized = true;

        // start with all tickers
        tickerPool = kValidTickers;

        // if config specifies a smaller pool, shuffle and trim
        if (tkrPoolCount > 0 && tkrPoolCount < tickerPool.size()) {
            static std::random_device rd;
            static std::mt19937 gen(rd());

            std::shuffle(tickerPool.begin(), tickerPool.end(), gen);
            tickerPool.resize(tkrPoolCount);
        }
    }

    return tickerPool;
}

Ticker getRandomTkr(int tkrPoolCount)
{
    if (tkrPoolCount > kValidTickers.size())
        tkrPoolCount = kValidTickers.size();

    const auto& pool = getTickerPool(tkrPoolCount);

    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<> dist(
        0, pool.size() - 1);  // exclude INVALID
    return pool[dist(gen)];
}

std::ostream& operator<<(std::ostream& os, Ticker tkr)
{
    return os << tkrToString(tkr);
}
}  // namespace solstice
