#include <order_side.h>

#include <ostream>

namespace solstice
{

std::ostream& operator<<(std::ostream& os, const OrderSide& orderSide)
{
    if (orderSide == OrderSide::Bid)
        os << "Bid";
    else
        os << "ask";

    return os;
}
}  // namespace solstice
