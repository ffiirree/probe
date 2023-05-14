#ifdef __linux__

#include "probe/dllport.h"

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
    PROBE_API std::vector<std::tuple<std::string, std::string, std::string>>
    devices_by_class(const std::string& cls);

    // retrive the <device_path, driver_path> /sys/class/<cls>/<name>
    PROBE_API std::pair<std::string, std::string> device_by_class(const std::string& cls,
                                                                  const std::string& name);

    PROBE_API std::string guess_bus(const std::string& path);

} // namespace probe::sys

#endif