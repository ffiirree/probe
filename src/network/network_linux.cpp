#ifdef __linux__

#include "probe/network.h"

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

    std::vector<adapter_t> adapters() { return {}; }

    traffic_status_t status(const adapter_t&) { return {}; }
} // namespace probe::network

#endif