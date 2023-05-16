#ifdef __linux__

#include "probe/thread.h"

#include <pthread.h>

namespace probe::thread
{
    int set_name(const std::string& name)
    {
        return ::pthread_setname_np(::pthread_self(), name.substr(0, 15).c_str());
    }

    std::string name(uint64_t id)
    {
        char buffer[128];
        ::pthread_getname_np(static_cast<pthread_t>(id), buffer, 128);
        return buffer;
    }

    std::string name() { return name(static_cast<uint64_t>(::pthread_self())); }
} // namespace probe::thread

#endif