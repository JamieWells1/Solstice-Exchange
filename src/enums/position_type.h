#ifndef MARKET_SIDE_H
#define MARKET_SIDE_H

#include <cstdint>
#include <ostream>

namespace solstice
{

enum class PositionType : uint8_t
{
    Long,
    Short
};

std::ostream& operator<<(std::ostream& os, PositionType positionType);

}  // namespace solstice

#endif  // MARKET_SIDE_H
