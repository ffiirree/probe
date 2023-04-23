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

    int RegistryListener::listen(const std::any& obj, const std::function<void(const std::any&)>& callback)
    {
        const auto& [key, subkey] = std::any_cast<std::pair<HKEY, std::string>>(obj);

        if(::RegOpenKeyEx(key, to_utf16(subkey).c_str(), 0, KEY_NOTIFY, &key_) != ERROR_SUCCESS) {
            return -1;
        }

        if((STOP_EVENT = ::CreateEvent(nullptr, TRUE, FALSE, L"Registry Stop Event")) == nullptr) {
            return -1;
        }

        if((NOTIFY_EVENT = ::CreateEvent(nullptr, FALSE, FALSE, L"Registry Notify Evnent")) == nullptr) {
            return -1;
        }

        running_ = true;
        thread_  = std::thread([=]() {
            thread_set_name("listen-" + subkey);

            const HANDLE events[] = { STOP_EVENT, NOTIFY_EVENT };

            while(running_) {
                if(::RegNotifyChangeKeyValue(key_, TRUE, REG_LEGAL_CHANGE_FILTER, NOTIFY_EVENT, TRUE) !=
                   ERROR_SUCCESS) {
                    ::SetEvent(STOP_EVENT);
                }

                switch(::WaitForMultipleObjects(2, events, false, INFINITE)) {
                case WAIT_OBJECT_0 + 0: // STOP_EVENT
                    running_ = false;
                    break;

                case WAIT_OBJECT_0 + 1: // NOTIFY_EVENT
                    callback(key_);
                    break;

                default: break;
                }
            }
        });

        return 0;
    }

    void RegistryListener::stop()
    {
        auto expected = true;
        if(running_.compare_exchange_strong(expected, false)) {
            ::SetEvent(STOP_EVENT);

            if(thread_.joinable()) thread_.join();

            ::CloseHandle(NOTIFY_EVENT);
            ::CloseHandle(STOP_EVENT);
            ::RegCloseKey(key_);
        }
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