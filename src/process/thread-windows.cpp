#ifdef _WIN32

#include "probe/defer.h"
#include "probe/thread.h"
#include "probe/util.h"

#include <processthreadsapi.h>

namespace probe::thread
{
    int set_name(const std::string& name)
    {
        // >= Windows 10, version 1607
        if (FAILED(::SetThreadDescription(::GetCurrentThread(), probe::util::to_utf16(name).c_str()))) {
            return -1;
        }
        return 0;
    }

    std::string name(uint64_t id)
    {
        // >= Windows 10, version 1607
        WCHAR *buffer = nullptr;
        if (SUCCEEDED(::GetThreadDescription(reinterpret_cast<HANDLE>(id), &buffer))) {
            defer(::LocalFree(buffer));
            return probe::util::to_utf8(buffer);
        }
        return {};
    }

    std::string name() { return name(reinterpret_cast<uint64_t>(::GetCurrentThread())); }
} // namespace probe::thread

#endif