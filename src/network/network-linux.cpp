#ifdef __linux__

#include "probe/defer.h"
#include "probe/network.h"
#include "probe/sysfs.h"
#include "probe/util.h"

#include <arpa/inet.h>
#include <filesystem>
#include <fstream>
#include <ifaddrs.h>
#include <iostream>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <map>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <regex>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

namespace probe::network
{
    std::string hostname()
    {
        char buffer[256]{};
        if (::gethostname(buffer, 256) == 0) {
            return buffer;
        }
        return {};
    }

    static std::vector<std::string> fread_list(const std::string& file)
    {
        std::ifstream fd(file);

        if (!fd.is_open() || !fd) return {};

        std::vector<std::string> list{};
        for (std::string line; std::getline(fd, line);) {
            list.emplace_back(line);
        }

        return list;
    }

    // A physical network interface represents a network hardware device such as NIC (Network Interface
    // Card), WNIC (Wireless Network Interface Card), or a modem.
    // A virtual network interface does not represent a hardware device but is linked to a network device.
    // It can be associated with a physical or virtual interface.
    std::vector<adapter_t> adapters()
    {
        ifaddrs *addresses;
        if (::getifaddrs(&addresses) == -1) return {};
        defer(::freeifaddrs(addresses));

        std::vector<adapter_t> ret;

        // device name
        auto list = fread_list("/proc/net/dev");
        if (list.size() <= 2) return {};

        for (size_t i = 2; i < list.size(); ++i) {
            auto lpos = list[i].find_first_not_of(probe::util::whitespace);
            auto rpos = list[i].find(':');
            if (lpos != std::string::npos && rpos != std::string::npos && lpos < rpos) {
                ret.emplace_back(adapter_t{ .name = list[i].substr(lpos, rpos - lpos) });
            }
        }

        // buses
        auto buses = probe::sys::buses();

        //
        int fd = ::socket(PF_INET, SOCK_DGRAM, 0);
        if (fd < 0) return ret;

        for (size_t i = 0; i < ret.size(); ++i) {

            // physical address
            ifreq ifr{};
            ::memset(&ifr, 0, sizeof(ifr));
            ::strcpy(ifr.ifr_name, ret[i].name.c_str());
            if (::ioctl(fd, SIOCGIFHWADDR, &ifr) == 0) {
                ret[i].type        = static_cast<if_type_t>(ifr.ifr_hwaddr.sa_family);
                ret[i].description = probe::to_string(ret[i].type) + " Interface";
                // physical address
                char buffer[18]{};
                ::sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x",
                          static_cast<unsigned char>(ifr.ifr_hwaddr.sa_data[0]),
                          static_cast<unsigned char>(ifr.ifr_hwaddr.sa_data[1]),
                          static_cast<unsigned char>(ifr.ifr_hwaddr.sa_data[2]),
                          static_cast<unsigned char>(ifr.ifr_hwaddr.sa_data[3]),
                          static_cast<unsigned char>(ifr.ifr_hwaddr.sa_data[4]),
                          static_cast<unsigned char>(ifr.ifr_hwaddr.sa_data[5]));
                ret[i].physical_address = buffer;
            }

            // mtu
            if (::ioctl(fd, SIOCGIFMTU, &ifr) == 0) {
                ret[i].mtu = ifr.ifr_mtu;
            }

            // ipv4 address
            ifr.ifr_addr.sa_family = AF_INET;
            if (::ioctl(fd, SIOCGIFADDR, &ifr) == 0) {
                ret[i].ipv4_addresses.emplace_back(
                    ::inet_ntoa(reinterpret_cast<sockaddr_in *>(&ifr.ifr_addr)->sin_addr));
            }

            // ipv6 address
            {
                auto ifv6_fd = ::fopen("/proc/net/if_inet6", "r");
                if (ifv6_fd) {
                    unsigned char ipv6[16]{};
                    char          dname[IFNAMSIZ]{};
                    unsigned int  if_idx{}, plen{}, scope{};

                    while (20 == ::fscanf(ifv6_fd,
                                          "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx"
                                          "%2hhx%2hhx%2hhx%2hhx %02x %02x %02x %*s %s",
                                          &ipv6[0], &ipv6[1], &ipv6[2], &ipv6[3], &ipv6[4], &ipv6[5],
                                          &ipv6[6], &ipv6[7], &ipv6[8], &ipv6[9], &ipv6[10], &ipv6[11],
                                          &ipv6[12], &ipv6[13], &ipv6[14], &ipv6[15], &if_idx, &plen,
                                          &scope, dname)) {
                        if (::strcmp(dname, ret[i].name.c_str()) == 0) {
                            char address[INET6_ADDRSTRLEN]{};
                            ::inet_ntop(AF_INET6, ipv6, address, INET6_ADDRSTRLEN);
                            ret[i].ipv6_addresses.emplace_back(std::string{ address } + "/" +
                                                               std::to_string(plen));
                        }
                    }

                    ::fclose(ifv6_fd);
                }
            }

            // bus info
            auto [device_path, driver_path] = probe::sys::device_by_class("net", ret[i].name);
            auto bus_name                   = probe::sys::guess_bus(driver_path);
            ret[i].bus                      = bus_cast(bus_name);
            ret[i].id                       = device_path;

            if (std::regex_search(device_path.filename().string(),
                                  std::regex("^\\d\\d\\d\\d:\\d\\d:\\d\\d.\\d$"))) {
                ret[i].bus_info = device_path.filename();
            }

            // vendor
            auto vendor_fd = ::fopen((device_path / "vendor").c_str(), "r");
            if (vendor_fd) {
                uint32_t vendor_id{};
                if (::fscanf(vendor_fd, "%x", &vendor_id) == 1) {
                    ret[i].vendor_id    = vendor_cast(vendor_id);
                    ret[i].manufacturer = probe::to_string(ret[i].vendor_id);

                    // product
                    uint32_t product_id{};
                    auto     product_fd = ::fopen((device_path / "device").c_str(), "r");
                    if (product_fd) {
                        if (::fscanf(product_fd, "%x", &product_id) == 1) {
                            ret[i].product = probe::product_name(vendor_id, product_id);
                        }
                        ::fclose(product_fd);
                    }
                }

                ::fclose(vendor_fd);
            }

            // flags
            if (::ioctl(fd, SIOCGIFFLAGS, &ifr) == 0) {
                ret[i].enabled = ifr.ifr_flags & IFF_UP;
            }

            char buffer[2 * IFNAMSIZ + 1]{};
            ::strncpy(buffer, ret[i].name.c_str(), sizeof(buffer) - 1);
            if (::ioctl(fd, 0x8B01, &buffer) == 0) {
                ret[i].description = "Wireless Interface (" + std::string{ buffer + IFNAMSIZ } + ")";
            }

            // driver
            ethtool_drvinfo drvinfo{};
            drvinfo.cmd  = ETHTOOL_GDRVINFO;
            ifr.ifr_data = reinterpret_cast<caddr_t>(&drvinfo);
            if (::ioctl(fd, SIOCETHTOOL, &ifr) == 0) {
                ret[i].driver         = drvinfo.driver;
                ret[i].driver_version = drvinfo.version;
                if (!std::string{ drvinfo.bus_info }.empty()) ret[i].bus_info = drvinfo.bus_info;
            }

            ret[i].is_virtual = virtual_physical_addresses.contains(ret[i].physical_address.substr(0, 8));
        }

        return ret;
    }

    traffic_status_t status(const adapter_t&) { return {}; }
} // namespace probe::network

#endif