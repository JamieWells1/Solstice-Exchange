#ifndef POSITION_TYPE_H
#define POSITION_TYPE_H

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

#endif  // POSITION_TYPE_H
