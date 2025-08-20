#ifndef TICKER_H
#define TICKER_H

#include <string>
#include <unordered_map>

enum class Ticker
{
    AAPL,
    TSLA,
    GOOGL,
    AMZN,
    MSFT,
    GOOG,
    INVALID,
    NVDA,
    COIN
};

inline const std::unordered_map<std::string, Ticker> kTickerMap = {
    {"AAPL", Ticker::AAPL},   {"TSLA", Ticker::TSLA},
    {"GOOGL", Ticker::GOOGL}, {"AMZN", Ticker::AMZN},
    {"MSFT", Ticker::MSFT},   {"NVDA", Ticker::NVDA},
    {"COIN", Ticker::COIN},   {"INVALID", Ticker::INVALID}};

Ticker validateTkr(const std::string& tkr);

inline const std::unordered_map<Ticker, std::string> kTickerNameMap = []
{
    std::unordered_map<Ticker, std::string> out;
    for (const auto& [name, ticker] : kTickerMap)
    {
        out[ticker] = name;
    }
    return out;
}();

std::ostream& operator<<(std::ostream& os, Ticker tkr);

#endif  // TICKER_H
