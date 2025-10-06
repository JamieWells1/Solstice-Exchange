#include <truncate.h>

#include <string>

namespace solstice::matching
{

std::string truncate(std::string str, size_t length)
{
    if (str.length() > length)
    {
        return str.substr(0, length) + "...";
    }

    return str;
}

}  // namespace solstice::matching
