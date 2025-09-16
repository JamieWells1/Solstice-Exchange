#ifndef ORDER_SIDE_H
#define ORDER_SIDE_H

#include <cstdint>

enum class OrderSide : uint8_t
{
    Buy = 1,
    Sell = 0
};

#endif // ORDER_SIDE_H
