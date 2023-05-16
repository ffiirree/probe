#ifdef __linux__

#include "probe/power.h"

namespace probe::power
{
    std::vector<supply_t> status() { return {}; }
} // namespace probe::power

#endif