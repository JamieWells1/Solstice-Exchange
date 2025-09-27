#ifndef TICKER_H
#define TICKER_H

#include <string>
#include <unordered_map>
#include <vector>

namespace solstice
{

#define TICKER_LIST(X) \
    X(AAPL)            \
    X(TSLA)            \
    X(GOOGL)           \
    X(GOOG)            \
    X(AMZN)            \
    X(MSFT)            \
    X(NVDA)            \
    X(COIN)            \
    X(INVALID)

enum class Ticker
{
#define X(name) name,
    TICKER_LIST(X)
#undef X
};

inline const std::unordered_map<std::string, Ticker> kTickerMap = {
#define X(name) {#name, Ticker::name},
    TICKER_LIST(X)
#undef X
};

inline const std::unordered_map<Ticker, std::string> kTickerNameMap = {
#define X(name) {Ticker::name, #name},
    TICKER_LIST(X)
#undef X
};

inline const std::vector<Ticker> kValidTickers = {
#define X(name) Ticker::name,
    TICKER_LIST(X)
#undef X
};

Ticker getRandomTkr();

Ticker validateTkr(const std::string& tkr);

const std::string tkrToString(Ticker tkr);

std::ostream& operator<<(std::ostream& os, Ticker tkr);

}  // namespace solstice

#endif  // TICKER_H
