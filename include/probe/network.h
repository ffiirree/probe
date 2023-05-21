#ifndef PROBE_NETWORK_H
#define PROBE_NETWORK_H

#include "probe/dllport.h"
#include "probe/types.h"

#include <string>
#include <unordered_set>
#include <vector>

namespace probe::network
{
    enum class if_type_t
    {
#ifdef _WIN32
        Ethernet  = 6,
        TokenRing = 9,
        FDDI      = 15,
        PPP       = 23,
        Loopback  = 24,
        SLIP      = 28,
        ATM       = 37,
        IEEE80211 = 71,
        TUNNEL    = 131,
        TUNNEL6   = TUNNEL,
        IEEE1394  = 144,
#elif __linux__
        Ethernet  = 1,
        TokenRing = 4,
        FDDI      = 774,
        PPP       = 512,
        Loopback  = 772,
        SLIP      = 256,
        ATM       = 19,
        IEEE80211 = 801,
        TUNNEL    = 768,
        TUNNEL6   = 769,
        IEEE1394  = 24,
#endif
    };

    inline const std::unordered_set<std::string> virtual_physical_addresses{
        "00:00:00",
        "00:03:FF", // Microsoft Corporation
        "00:05:69", // VMware Inc.
        "00:0C:29", // VMware Inc.
        "00:0F:4B", // Oracle Corporation
        "00:13:07", // Paravirtual Corporation
        "00:13:BE", // Virtual Conexions
        "00:15:5d", // Microsoft Corporation
        "00:16:3E", // Xensource Inc.
        "00:1C:42", // Parallels Inc.
        "00:21:F6", // Oracle Corporation
        "00:24:0B", // Virtual Computer Inc.
        "00:50:56", // VMware Inc.
        "00:A0:B1", // First Virtual Corporation
        "00:E0:C8", // Virtual Access Ltd
        "08:00:27", // PCS Systemtechnik GmbH
        "0A:00:27",
        "18:92:2C", // Virtual Instruments
        "3C:F3:92", // Virtualtek Ltd
    };

    enum scope_t
    {
        global   = 0x00,
        loopback = 0x10,
        link     = 0x20,
        site     = 0x40,
        compat   = 0x80,
    };

    struct adapter_t
    {
        std::string name{};
        vendor_t vendor_id{};         // PCI: vendor id
        std::string product{};        // PCI: product name
        std::string id{};             // Windows: adapter name, linux: device path
        std::string guid{};           // Windows: network guid
        std::string interface_guid{}; // Windows: interface guid
        std::string description{};
        std::string manufacturer{};   // Windows:
        bool enabled{};
        bus_type_t bus{};
        std::string bus_info{};
        std::string driver{};
        std::string driver_version{};
        bool is_virtual{}; // guessed by physical address and the description (if contains "virtual")
        if_type_t type{};
        std::string physical_address{}; // MAC
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