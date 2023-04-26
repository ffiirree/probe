#ifdef __linux__

#include "probe/graphics.h"
#include "probe/util.h"

#include <array>

namespace probe::graphics
{
    std::array<char, 256> edid_of(const std::string&, const std::string&)
    {
        std::array<char, 256> arr{};
        return arr;
    }
} // namespace probe::graphics

#endif