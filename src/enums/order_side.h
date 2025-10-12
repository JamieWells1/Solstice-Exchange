#ifndef ORDER_SIDE_H
#define ORDER_SIDE_H

#include <cstdint>
#include <ostream>

namespace solstice
{

enum class OrderSide : uint8_t
{
    Bid = 1,
    Ask = 0
};

std::ostream& operator<<(std::ostream& os, OrderSide orderSide);

}  // namespace solstice

#endif  // ORDER_SIDE_H
