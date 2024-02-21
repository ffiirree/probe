#ifdef __linux__

#include "probe/memory.h"

#include <sys/sysinfo.h>

namespace probe::memory
{
    memory_status_t status()
    {
        struct sysinfo info{};
        ::sysinfo(&info);

        return {
            .avail = info.freeram,
            .total = info.totalram,
        };
    }

    std::vector<MemoryDevice> devices() { return {}; }

    std::vector<PhysicalMemoryArray> physical_device_arrays() { return {}; }
} // namespace probe::memory

#endif