#ifdef __linux__

#include "probe/graphics.h"
#include "probe/util.h"

#include <array>

namespace probe::graphics
{
    std::array<char, 256> edid_of(const std::string& name, const std::string& driver)
    {
        std::array<char, 256> arr{};
        return arr;
    }
} // namespace probe::graphics

#endif