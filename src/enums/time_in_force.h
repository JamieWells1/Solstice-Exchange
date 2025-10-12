#ifndef ORDER_TYPE_H
#define ORDER_TYPE_H

#include <cstdint>
#include <ostream>

namespace solstice
{

enum class TimeInForce : uint8_t
{
    MarketOrder,
    LimitOrder,
    IOC,
    FOK,
    GTC,
    GTD
};

std::ostream& operator<<(std::ostream& os, TimeInForce OrderType);

}  // namespace solstice

#endif  // ORDER_TYPE_H
