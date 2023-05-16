#include "probe/power.h"

namespace probe
{
    std::string to_string(power::supply_type_t t)
    {
        // clang-format off
        switch (t) {
        case power::supply_type_t::Battery:     return "Battery";
        case power::supply_type_t::UPS:         return "UPS";
        case power::supply_type_t::Mains:       return "Mains";
        case power::supply_type_t::USB:         return "USB";
        case power::supply_type_t::Wireless:    return "Wireless";
        default:                                return "Unknown";
        }
        // clang-format on
    }

    std::string to_string(power::supply_status_t s)
    {
        // clang-format off
        switch (s) {
        case power::supply_status_t::Charging:      return "Charging";
        case power::supply_status_t::Discharging:   return "Discharging";
        case power::supply_status_t::NotCharging:   return "NotCharging";
        case power::supply_status_t::Full:          return "Full";
        default:                                    return "Unknown";
        }
        // clang-format on
    }

    std::string to_string(power::charge_type_t t)
    {
        // clang-format off
        switch (t) {
        case power::charge_type_t::None:            return "None";
        case power::charge_type_t::Trickle:         return "Trickle";
        case power::charge_type_t::Fast:            return "Fast";
        case power::charge_type_t::Standard:        return "Standard";
        case power::charge_type_t::Adaptive:        return "Adaptive";
        case power::charge_type_t::Custom:          return "Custom";
        case power::charge_type_t::Longlife:        return "Longlife";
        case power::charge_type_t::Bypass:          return "Bypass";
        default:                                    return "Unknown";
        }
        // clang-format on
    }
} // namespace probe