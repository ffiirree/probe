#ifndef PROBE_THREAD_H
#define PROBE_THREAD_H

#include "probe/dllport.h"

#include <cstdint>
#include <string>

namespace probe::thread
{
    // linux: The thread name is a meaningful C language string, whose length is
    //        restricted to 16 characters, including the terminating null byte ('\0')
    PROBE_API int set_name(const std::string&);

    // get name of specific thread
    PROBE_API std::string name(uint64_t);

    // get name of the current thread
    PROBE_API std::string name();
} // namespace probe::thread

#endif //! PROBE_THREAD_H