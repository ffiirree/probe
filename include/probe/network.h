#ifndef PROBE_NETWORK_H
#define PROBE_NETWORK_H

#include "probe/dllport.h"

#include <string>
#include <vector>

namespace probe::network
{
    enum class if_type_t
    {
        Other     = 1,
        Ethernet  = 6,
        TokenRing = 9,
        PPP       = 23,
        Loopback  = 24,
        ATM       = 37,
        IEEE80211 = 71,
        TUNNEL    = 131,
        IEEE1394  = 144
    };

    struct adapter_t
    {
        std::string name{};
        std::string id{};   // Windows: adapter name
        std::string guid{}; // Windows: network guid
        std::string interface_guid{};
        std::string description{};
        if_type_t type{ if_type_t::Other };
        std::string physical_address{};
        bool dhcp_enabled{};
        uint64_t mtu{};
        std::vector<std::string> ipv4_addresses{};
        std::vector<std::string> ipv6_addresses{};
        std::string dhcpv4_server{};
        std::string dhcpv6_server{};
        std::vector<std::string> ipv4_gateways{};
        std::vector<std::string> ipv6_gateways{};
        std::string dns_suffix{};
        std::vector<std::string> ipv4_dns_servers{};
        std::vector<std::string> ipv6_dns_servers{};
    };

    // Windows: The hostname for the local computer. This may be the fully qualified hostname (including the
    //          domain) for a computer that is joined to a domain.
    PROBE_API std::string hostname();

    PROBE_API std::vector<adapter_t> adapters();

    struct traffic_status_t
    {
        std::string guid{};          // interface guid

        uint64_t ibytes{};           // bytes
        uint64_t obytes{};           // bytes

        uint64_t unicast_ibytes{};   // bytes
        uint64_t unicast_obytes{};   // bytes

        uint64_t broadcast_ibytes{}; // bytes
        uint64_t broadcast_obytes{}; // bytes

        uint64_t multicast_ibytes{}; // bytes
        uint64_t multicast_obytes{}; // bytes
    };

    PROBE_API traffic_status_t status(const adapter_t&);
} // namespace probe::network

namespace probe
{
    PROBE_API std::string to_string(network::if_type_t);
} // namespace probe

#endif //! PROBE_NETWORK_H