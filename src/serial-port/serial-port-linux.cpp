#ifdef __linux__

#include "probe/serial-port.h"

namespace probe::port
{
    std::vector<serial_port> ports() { return {}; }
}; // namespace probe::port

#endif