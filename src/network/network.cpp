#include "probe/network.h"

namespace probe
{
    std::string to_string(network::if_type_t type)
    {
        // clang-format off
        switch (type) {
        case network::if_type_t::Ethernet:      return "Ethernet";
        case network::if_type_t::TokenRing:     return "Token Ring";
        case network::if_type_t::PPP:           return "PPP";
        case network::if_type_t::Loopback:      return "Software Loopback";
        case network::if_type_t::ATM:           return "ATM";
        case network::if_type_t::IEEE80211:     return "IEEE 802.11";
        case network::if_type_t::TUNNEL:        return "TUNNEL";
        case network::if_type_t::IEEE1394:      return "IEEE 1394";
        case network::if_type_t::Other:
        default:                                return "Other";
        }
        // clang-format on
    }
} // namespace probe