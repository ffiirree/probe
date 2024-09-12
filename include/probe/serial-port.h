#ifndef PROBE_SERIAL_PORT_H
#define PROBE_SERIAL_PORT_H

#include "probe/dllport.h"

#include <string>
#include <vector>

namespace probe::port
{
    struct serial_port
    {
        std::string name{};        // Windows: COM*; Linux: ttyUSB*
        std::string friendly_name{};
        std::string device{};      // Windows: \\.\COM*; Linux: /dev/ttyUSB*
        std::string instance_id{}; // Windows
        std::string description{};
        std::string manufacturer{};
        std::string driver{};
    };

    PROBE_API std::vector<serial_port> ports();
}; // namespace probe::port

#endif //! PROBE_SERIAL_PORT_H