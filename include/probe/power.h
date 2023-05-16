#ifndef PROBE_BATTERY_H
#define PROBE_BATTERY_H

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
        std::string name{};
        supply_type_t type{};
        supply_status_t status{};
        charge_type_t charge_type{};
        float percent{};
        vendor_t vendor{};
    };

    PROBE_API std::vector<supply_t> status();
} // namespace probe::power

namespace probe
{
    PROBE_API std::string to_string(probe::power::supply_type_t);
    PROBE_API std::string to_string(probe::power::supply_status_t);
    PROBE_API std::string to_string(probe::power::charge_type_t);
} // namespace probe

#endif //! PROBE_BATTERY_H