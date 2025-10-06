#ifndef ORDER_SIDE_H
#define ORDER_SIDE_H

#include <cstdint>
#include <ostream>

namespace solstice
{

enum class OrderSide : uint8_t
{
    Buy = 1,
    Sell = 0
};

std::ostream& operator<<(std::ostream& os, OrderSide orderSide);
}

#endif  // ORDER_SIDE_H
