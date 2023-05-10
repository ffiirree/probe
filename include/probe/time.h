#ifndef PROBE_BATTERY_H
#define PROBE_BATTERY_H

#include "probe/dllport.h"

#include <chrono>

#ifdef _WIN32
#include <Windows.h>
#endif

using namespace std::chrono_literals;

namespace probe::time
{
    // ns
    PROBE_API inline uint64_t system_time()
    {
        using namespace std::chrono;
        return duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
    }

    /**
     * Get the current time in nanoseconds since some unspecified starting point.
     * On platforms that support it, the time comes from a monotonic clock
     * This property makes this time source ideal for measuring relative time.
     * The returned values may not be monotonic on platforms where a monotonic
     * clock is not available.
     */
    PROBE_API inline uint64_t relative_time()
    {
        using namespace std::chrono;
        return duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();
    }

#ifdef _WIN32
    // ns, could be used with system_time()
    PROBE_API inline uint64_t to_time(FILETIME ft)
    {
        ULARGE_INTEGER ulint{};
        ulint.HighPart    = ft.dwHighDateTime;
        ulint.LowPart     = ft.dwLowDateTime;

        SYSTEMTIME st{};
        FileTimeToSystemTime(&ft, &st);

        uint64_t win_time = ulint.QuadPart; // 100ns, starts 1601-01-01T00:00:00Z

        return (win_time - 11644473600'000'000'0LL) * 100;
    }
#endif
} // namespace probe::time

#endif //! PROBE_BATTERY_H