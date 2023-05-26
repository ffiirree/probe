#ifdef __linux__

#ifndef PROBE_SYSFS_H
#define PROBE_SYSFS_H

#include "probe/dllport.h"

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

// https://elixir.bootlin.com/linux/v4.6/source/Documentation/sysfs-rules.txt
//
// - sysfs is always at /sys
// - devices are only "devices"
//   - devpath
//   - kernel name
//   - subsystem
//   - driver
//   - attributes
namespace probe::sys
{
    // retrive all bus types in '/sys/bus'
    PROBE_API std::vector<std::string> buses();

    // retrive the <name, device_path, driver_path> of devices in '/sys/class/<cls>'
    PROBE_API std::vector<std::tuple<std::string, std::filesystem::path, std::filesystem::path>>
    devices_by_class(const std::string& cls);

    // retrive the <device_path, driver_path> /sys/class/<cls>/<name>
    PROBE_API std::pair<std::filesystem::path, std::filesystem::path>
    device_by_class(const std::string& cls, const std::string& name);

    // guess bus type by the driver path of device
    PROBE_API std::string guess_bus(const std::string& path);

    // pci devices
    struct pci_device_t 
    {
        uint32_t class_id{};
        uint32_t vendor_id{};
        uint32_t product_id{};
        std::string bus_info{};
        std::string device_path{};
        std::string driver_path{};
    };

    PROBE_API std::vector<pci_device_t> pci_devices(uint32_t=0);
} // namespace probe::sys

#endif //! PROBE_SYSFS_H

#endif