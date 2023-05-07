#ifdef _WIN32

#include "probe/defer.h"
#include "probe/network.h"

// clang-format off
#include <WinSock2.h>
#include <netioapi.h>
#include <iphlpapi.h>
#include <ws2ipdef.h>
#include <WS2tcpip.h>
#include "probe/util.h"
// clang-format on

#include <format>
#include <iostream>

namespace probe::network
{
    static std::string to_string(SOCKADDR_IN *addr)
    {
        char buffer[INET_ADDRSTRLEN]{};
        if (addr->sin_family == AF_INET) {
            ::inet_ntop(AF_INET, &(addr->sin_addr), buffer, INET_ADDRSTRLEN);
        }
        return buffer;
    }

    static std::string to_string(SOCKADDR_IN6 *addr)
    {
        char buffer[INET6_ADDRSTRLEN]{};
        if (addr->sin6_family == AF_INET6) {
            ::inet_ntop(AF_INET6, &(addr->sin6_addr), buffer, INET6_ADDRSTRLEN);
        }
        return buffer;
    }

    static std::string to_string(const LPSOCKADDR addr)
    {
        if (addr->sa_family == AF_INET) {
            return to_string(reinterpret_cast<SOCKADDR_IN *>(addr));
        }
        else if (addr->sa_family == AF_INET6) {
            return to_string(reinterpret_cast<SOCKADDR_IN6 *>(addr));
        }

        return {};
    }

    static std::string to_string(const GUID& guid)
    {
        WCHAR buffer[128]{};
        ::StringFromGUID2(guid, buffer, 128);
        return probe::util::to_utf8(buffer);
    }

    std::string hostname()
    {
        FIXED_INFO *info{};
        DWORD size{};
        if (::GetNetworkParams(nullptr, &size) == ERROR_BUFFER_OVERFLOW) {
            info = reinterpret_cast<FIXED_INFO *>(malloc(size));
            if (::GetNetworkParams(info, &size) != NO_ERROR) {
                return {};
            }
        }
        defer(free(info));

        return info->HostName;
    }

    // https://learn.microsoft.com/en-us/windows/win32/network-interfaces
    std::vector<adapter_t> adapters()
    {
        ULONG size{};

        PIP_ADAPTER_ADDRESSES infos{};

        if (::GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_GATEWAYS, nullptr,
                                   nullptr, &size) == ERROR_BUFFER_OVERFLOW) {
            infos = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(malloc(size));

            if (::GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_GATEWAYS,
                                       nullptr, infos, &size) != ERROR_SUCCESS) {
                return {};
            }
        }
        defer(free(infos));

        std::vector<adapter_t> ret;

        for (auto info = infos; info; info = info->Next) {
            // ignore the software loopback adapter
            if (info->IfType == IF_TYPE_SOFTWARE_LOOPBACK) continue;

            // Physical Address
            std::string mac{};
            if (info->PhysicalAddressLength != 0) {
                for (ULONG i = 0; i < info->PhysicalAddressLength; ++i) {
                    mac += (i == info->PhysicalAddressLength - 1)
                               ? std::format("{:02X}", static_cast<int>(info->PhysicalAddress[i]))
                               : std::format("{:02X}:", static_cast<int>(info->PhysicalAddress[i]));
                }
            }

            // IP Addresses
            std::vector<std::string> ipv4_addrs{}, ipv6_addrs{};
            for (auto addr = info->FirstUnicastAddress; addr; addr = addr->Next) {
                if (addr->Address.lpSockaddr->sa_family == AF_INET) {
                    ipv4_addrs.emplace_back(to_string(addr->Address.lpSockaddr));
                }
                else if (addr->Address.lpSockaddr->sa_family == AF_INET6) {
                    ipv6_addrs.emplace_back(to_string(addr->Address.lpSockaddr));
                }
            }

            // DHCPv4 Server
            char DHCPv4Addr[INET_ADDRSTRLEN]{};
            if (info->Dhcpv4Server.lpSockaddr && info->Dhcpv4Server.lpSockaddr->sa_family == AF_INET) {
                auto ipv4 = reinterpret_cast<SOCKADDR_IN *>(info->Dhcpv4Server.lpSockaddr);
                inet_ntop(AF_INET, &(ipv4->sin_addr), DHCPv4Addr, INET_ADDRSTRLEN);
            }

            // DHCPv6 Server
            char DHCPv6Addr[INET6_ADDRSTRLEN]{};
            if (info->Dhcpv6Server.lpSockaddr && info->Dhcpv6Server.lpSockaddr->sa_family == AF_INET6) {
                auto ipv6 = reinterpret_cast<SOCKADDR_IN6 *>(info->Dhcpv6Server.lpSockaddr);
                inet_ntop(AF_INET6, &(ipv6->sin6_addr), DHCPv6Addr, INET6_ADDRSTRLEN);
            }

            // Gateways
            std::vector<std::string> IPv4Gateways{}, IPv6Gateways{};
            for (auto addr = info->FirstGatewayAddress; addr; addr = addr->Next) {
                if (addr->Address.lpSockaddr->sa_family == AF_INET) {
                    IPv4Gateways.emplace_back(to_string(addr->Address.lpSockaddr));
                }
                else if (addr->Address.lpSockaddr->sa_family == AF_INET6) {
                    IPv6Gateways.emplace_back(to_string(addr->Address.lpSockaddr));
                }
            }

            // DNS Servers
            std::vector<std::string> IPv4DNSServers{}, IPv6DNSServers{};
            for (auto addr = info->FirstDnsServerAddress; addr; addr = addr->Next) {
                if (addr->Address.lpSockaddr->sa_family == AF_INET) {
                    IPv4DNSServers.emplace_back(to_string(addr->Address.lpSockaddr));
                }
                else if (addr->Address.lpSockaddr->sa_family == AF_INET6) {
                    IPv6DNSServers.emplace_back(to_string(addr->Address.lpSockaddr));
                }
            }

            // Interface LUID
            GUID if_guid{};
            ::ConvertInterfaceLuidToGuid(&info->Luid, &if_guid);

            ret.push_back(adapter_t{
                .name             = probe::util::to_utf8(info->FriendlyName),
                .id               = info->AdapterName,
                .guid             = to_string(info->NetworkGuid),
                .interface_guid   = to_string(if_guid),
                .description      = probe::util::to_utf8(info->Description),
                .type             = static_cast<if_type_t>(info->IfType),
                .physical_address = mac,
                .dhcp_enabled     = !!info->Dhcpv4Enabled,
                .mtu              = info->Mtu,
                .ipv4_addresses   = std::move(ipv4_addrs),
                .ipv6_addresses   = std::move(ipv6_addrs),
                .dhcpv4_server    = DHCPv4Addr,
                .dhcpv6_server    = DHCPv6Addr,
                .ipv4_gateways    = std::move(IPv4Gateways),
                .ipv6_gateways    = std::move(IPv6Gateways),
                .dns_suffix       = probe::util::to_utf8(info->DnsSuffix),
                .ipv4_dns_servers = std::move(IPv4DNSServers),
                .ipv6_dns_servers = std::move(IPv6DNSServers),
            });
        }
        return ret;
    }

    static int to_luid(const std::string& guid_str, NET_LUID *luid)
    {
        GUID guid{};

        if (::CLSIDFromString(probe::util::to_utf16(guid_str).c_str(), &guid) != NOERROR) {
            return -1;
        }
        if (::ConvertInterfaceGuidToLuid(&guid, luid) != NO_ERROR) {
            return -1;
        }

        return 0;
    }

    traffic_status_t status(const adapter_t& adapter)
    {
        NET_LUID luid{};
        if (to_luid(adapter.interface_guid, &luid) != 0) {
            return {};
        }

        MIB_IF_ROW2 row{ .InterfaceLuid = luid };
        if (::GetIfEntry2(&row) != NO_ERROR) {
            return {};
        }

        return {
            .guid             = adapter.interface_guid,
            .ibytes           = row.InOctets,
            .obytes           = row.OutOctets,
            .unicast_ibytes   = row.InUcastOctets,
            .unicast_obytes   = row.OutUcastOctets,
            .broadcast_ibytes = row.InBroadcastOctets,
            .broadcast_obytes = row.OutBroadcastOctets,
            .multicast_ibytes = row.InMulticastOctets,
            .multicast_obytes = row.OutMulticastOctets,
        };
    }
} // namespace probe::network

#endif