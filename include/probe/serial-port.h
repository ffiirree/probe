#ifndef PROBE_SERIAL_PORT_H
#define PROBE_SERIAL_PORT_H

#include "probe/dllport.h"

#include <string>
#include <vector>

namespace probe::port
{
    struct serial_port
    {
        std::string name{};
        std::string device{};
        std::string instance_id{};
        std::string description{};
        std::string manufacturer{};
    };

    PROBE_API std::vector<serial_port> ports();
};     // namespace probe::port
#endif //! PROBE_SERIAL_PORT_H