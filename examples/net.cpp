#include "probe/network.h"

#include <iostream>
#include <iomanip>
#ifdef _WIN32
#include <Windows.h>
#endif

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IOFBF, 1000);
#endif

    auto adapters = probe::network::adapters();

    std::cout << "Host Name            : " << probe::network::hostname() << "\n\n";

    std::cout << "Network Adapters: \n";
    for (const auto& adapter : adapters) {
        std::cout << "  " << adapter.name << "\n"
                  << "    ID               : " << adapter.id << '\n'
                  << "    GUID             : " << adapter.guid << '\n'
                  << "    Interface GUID   : " << adapter.interface_guid << '\n'
                  << "    Description      : " << adapter.description << '\n'
                  << "    Interface Type   : " << probe::to_string(adapter.type) << '\n'
                  << "    DHCP Enabled     : " << adapter.dhcp_enabled << '\n'
                  << "    Physical Address : " << adapter.physical_address << '\n'
                  << "    IPv4 Address     : ";
        for (size_t i = 0; i < adapter.ipv4_addresses.size(); ++i) {
            std::cout << adapter.ipv4_addresses[i] << '\n';
            if (i != adapter.ipv4_addresses.size() - 1) std::cout << "                       ";
        }
        std::cout << "    IPv6 Address     : ";
        for (size_t i = 0; i < adapter.ipv6_addresses.size(); ++i) {
            std::cout << adapter.ipv6_addresses[i] << '\n';
            if (i != adapter.ipv6_addresses.size() - 1) std::cout << "                       ";
        }

        if (!adapter.dhcpv4_server.empty()) {
            std::cout << "    DHCPv4 Server    : " << adapter.dhcpv4_server << '\n';
        }

        if (!adapter.dhcpv6_server.empty()) {
            std::cout << "    DHCPv6 Server    : " << adapter.dhcpv6_server << '\n';
        }

        if (!adapter.ipv4_gateways.empty()) {
            std::cout << "    IPv4 Gateways    : ";
            for (size_t i = 0; i < adapter.ipv4_gateways.size(); ++i) {
                std::cout << adapter.ipv4_gateways[i] << '\n';
                if (i != adapter.ipv4_gateways.size() - 1) std::cout << "                       ";
            }
        }

        if (!adapter.ipv6_gateways.empty()) {
            std::cout << "    IPv6 Gateways    : ";
            for (size_t i = 0; i < adapter.ipv6_gateways.size(); ++i) {
                std::cout << adapter.ipv6_gateways[i] << '\n';
                if (i != adapter.ipv6_gateways.size() - 1) std::cout << "                       ";
            }
        }

        if (!adapter.ipv4_dns_servers.empty()) {
            std::cout << "    IPv4 DNS Servers : ";
            for (size_t i = 0; i < adapter.ipv4_dns_servers.size(); ++i) {
                std::cout << adapter.ipv4_dns_servers[i] << '\n';
                if (i != adapter.ipv4_dns_servers.size() - 1) std::cout << "                       ";
            }
        }

        if (!adapter.ipv6_dns_servers.empty()) {
            std::cout << "    IPv6 DNS Servers : ";
            for (size_t i = 0; i < adapter.ipv6_dns_servers.size(); ++i) {
                std::cout << adapter.ipv6_dns_servers[i] << '\n';
                if (i != adapter.ipv6_dns_servers.size() - 1) std::cout << "                       ";
            }
        }

        auto status = probe::network::status(adapter);

        std::cout << "    I Data           : " << std::setw(12) << status.ibytes << " B\n"
                  << "    O Data           : " << std::setw(12) << status.obytes << " B\n";

        std::cout << "\n";
    }

    return 0;
}