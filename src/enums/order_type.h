#ifndef ORDER_TYPE_H
#define ORDER_TYPE_H

#include <cstdint>
#include <ostream>

namespace solstice
{

enum class OrderType : uint8_t
{
    CrossSpread,
    InsideSpread,
    AtSpread
};

std::ostream& operator<<(std::ostream& os, OrderType orderType);

}  // namespace solstice

#endif  // ORDER_TYPE_H
