#ifdef __linux__

#include "probe/power.h"

namespace probe::power
{
    std::vector<supply_t> supplies() { return {}; }
} // namespace probe::power

#endif