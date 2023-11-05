#ifndef PROBE_POWER_H
#define PROBE_POWER_H

#include "probe/dllport.h"
#include "probe/types.h"

#include <string>
#include <vector>

namespace probe::power
{
    enum class supply_type_t
    {
        Unknown = 0x00,
        Battery,
        UPS,
        Mains,
        USB,
        Wireless
    };

    enum class supply_status_t
    {
        Unknown = 0x00,
        Charging,
        Discharging,
        NotCharging,
        Full,
    };

    enum class charge_type_t
    {
        Unknown = 0x00,
        None,
        Trickle,
        Fast,
        Standard,
        Adaptive,
        Custom,
        Longlife,
        Bypass,
    };

    struct supply_t
    {
        std::string     name{};
        std::string     path{};
        std::string     serial{};
        std::string     manufacturer{};
        uint32_t        cycle{};
        uint64_t        capabilities{};
        std::string     chemistry{};
        uint64_t        capacity{};              // mWh
        uint64_t        full_charged_capacity{}; // mWh
        supply_type_t   type{};
        supply_status_t status{};
        charge_type_t   charge_type{};
        float           percent{};
    };

    PROBE_API std::vector<supply_t> supplies();
} // namespace probe::power

namespace probe
{
    PROBE_API std::string to_string(probe::power::supply_type_t);
    PROBE_API std::string to_string(probe::power::supply_status_t);
    PROBE_API std::string to_string(probe::power::charge_type_t);
} // namespace probe

#endif //! PROBE_POWER_H