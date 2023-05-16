#ifndef PROBE_MEMORY_H
#define PROBE_MEMORY_H

#include "probe/dllport.h"

#include <cstdint>

namespace probe::memory
{
    struct memory_status_t
    {
        uint64_t avail; // bytes
        uint64_t total; // bytes
    };

    PROBE_API memory_status_t status();

    PROBE_API int slots();
} // namespace probe::memory

#endif //! PROBE_MEMORY_H