#ifdef _WIN32

#include "probe/memory.h"

#include <Windows.h>

namespace probe::memory
{
    memory_status_t status()
    {
        MEMORYSTATUSEX statex{ .dwLength = sizeof(MEMORYSTATUSEX) };

        if (!::GlobalMemoryStatusEx(&statex)) return {};

        return {
            .avail = statex.ullAvailPhys,
            .total = statex.ullTotalPhys,
        };
    }
} // namespace probe::memory

#endif