#ifdef __linux__

#include "probe/serial-port.h"

#include <filesystem>

namespace probe::port
{
    //   /sys/class/tty/tty*
    // or
    //   /dev/ttyS*
    //   /dev/ttyUSB*
    //   /dev/ttyACM*
    std::vector<serial_port> ports()
    {
        std::vector<serial_port> list{};

        for (const auto& entry : std::filesystem::directory_iterator{ "/dev" }) {
            if (!entry.is_directory() && entry.path().filename().string().starts_with("ttyUSB")) {
                list.push_back({
                    .name   = entry.path().filename().string(),
                    .device = entry.path().string(),
                });
            }
        }

        return list;
    }
}; // namespace probe::port

#endif