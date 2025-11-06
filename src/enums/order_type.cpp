#include <order_type.h>

#include <ostream>

namespace solstice
{

std::ostream& operator<<(std::ostream& os, const OrderType& orderType)
{
    if (orderType == OrderType::CrossSpread)
        os << "CrossSpread";
    else if ((orderType == OrderType::InsideSpread))
        os << "InsideSpread";
    else if ((orderType == OrderType::AtSpread))
        os << "AtSpread";

    return os;
}
}  // namespace solstice
