#ifndef MARKET_SIDE_H
#define MARKET_SIDE_H

#include <cstdint>
#include <ostream>

namespace solstice
{

enum class MarketSide : uint8_t
{
    Bid = 1,
    Ask = 0
};

std::ostream& operator<<(std::ostream& os, MarketSide marketSide);

}  // namespace solstice

#endif  // MARKET_SIDE_H
