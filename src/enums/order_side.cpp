#include <order_side.h>
#include <ostream>

namespace solstice::matching
{

std::ostream& operator<<(std::ostream& os, const OrderSide& orderSide)
{
    if (orderSide == OrderSide::Buy) os << "Buy";
    else os << "Sell";

    return os;
}
}  // namespace solstice::matching
