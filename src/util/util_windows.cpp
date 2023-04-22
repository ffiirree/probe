#ifdef _WIN32

#include "probe/defer.h"
#include "probe/util.h"

#include <Windows.h>
#include <processthreadsapi.h>

namespace probe::util::registry
{
    // https://learn.microsoft.com/en-us/windows/win32/sysinfo/registry-value-types
    // REG_DWORD        : A 32-bit number.
    // REG_QWORD        : A 64-bit number.
    template<>
    std::optional<DWORD> read<DWORD>(HKEY key, const std::string& subkey, const std::string& valuename)
    {
        DWORD value = 0;
        DWORD size  = sizeof(uint32_t);
        if(::RegGetValue(key, to_utf16(subkey).c_str(), to_utf16(valuename).c_str(), RRF_RT_REG_DWORD,
                         nullptr, &value, &size) == ERROR_SUCCESS) {
            return value;
        }

        return std::nullopt;
    }

    // REG_SZ	        : A null - terminated string.
    //                    It's either a Unicode or an ANSI string,
    //                    depending on whether you use the Unicode or ANSI functions.
    template<>
    std::optional<std::string> read<std::string>(HKEY key, const std::string& subkey,
                                                 const std::string& valuename)
    {
        DWORD size = 0;
        if(::RegGetValue(key, to_utf16(subkey).c_str(), to_utf16(valuename).c_str(), RRF_RT_REG_SZ, nullptr,
                         nullptr, &size) != ERROR_SUCCESS) {
            return std::nullopt;
        }

        std::string value(size, {});
        if(::RegGetValue(key, to_utf16(subkey).c_str(), to_utf16(valuename).c_str(), RRF_RT_REG_SZ, nullptr,
                         reinterpret_cast<LPBYTE>(value.data()), &size) != ERROR_SUCCESS) {
            return std::nullopt;
        }

        return to_utf8(value);
    }

} // namespace probe::util::registry

namespace probe::util
{
    int thread_set_name(const std::string& name)
    {
        // >= Windows 10, version 1607
        if(FAILED(::SetThreadDescription(::GetCurrentThread(), to_utf16(name).c_str()))) {
            return -1;
        }
        return 0;
    }

    std::string thread_get_name()
    {
        // >= Windows 10, version 1607
        WCHAR *buffer = nullptr;
        if(SUCCEEDED(::GetThreadDescription(::GetCurrentThread(), &buffer))) {
            defer(::LocalFree(buffer));
            return to_utf8(buffer);
        }
        return {};
    }
} // namespace probe::util

#endif // _WIN32