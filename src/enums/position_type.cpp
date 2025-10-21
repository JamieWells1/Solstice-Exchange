#include <position_type.h>

#include <ostream>

namespace solstice
{

std::ostream& operator<<(std::ostream& os, const PositionType& positionType)
{
    if (positionType == PositionType::Long)
        os << "Long";
    else
        os << "Short";

    return os;
}
}  // namespace solstice
