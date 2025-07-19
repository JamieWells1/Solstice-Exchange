#ifndef TICKER_H
#define TICKER_H

#include <unordered_map>
#include <string>

enum class Ticker { AAPL, TSLA, GOOGL, AMZN, MSFT, GOOG, INVALID };

inline const std::unordered_map<std::string, Ticker> kTickerMap = {
    {"AAPL", Ticker::AAPL},
    {"TSLA", Ticker::TSLA},
    {"GOOGL", Ticker::GOOGL},
    {"AMZN", Ticker::AMZN},
    {"MSFT", Ticker::MSFT},
    {"GOOG", Ticker::GOOG},
    {"INVALID", Ticker::INVALID}
};

Ticker validateTkr(const std::string& tkr);

#endif  // TICKER_H
