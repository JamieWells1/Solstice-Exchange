#ifndef ORDER_TYPE_H
#define ORDER_TYPE_H

#include <cstdint>
#include <ostream>

namespace solstice
{

// TODO: Add TimeInForce as a property to each order and add new matcher logic

enum class TimeInForce : uint8_t
{
    MarketOrder,
    LimitOrder,
    IOC,  // immediate or cancel
    FOK,  // fill or kill
};

std::ostream& operator<<(std::ostream& os, TimeInForce orderType);

}  // namespace solstice

#endif  // ORDER_TYPE_H
