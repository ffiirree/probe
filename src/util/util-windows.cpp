#ifdef _WIN32

#include "probe/defer.h"
#include "probe/thread.h"
#include "probe/util.h"
#include "probe/windows/setupapi.h"

#include <array>

namespace probe::util
{
    std::string format_system_error(uint64_t ec)
    {
        char *msg    = nullptr;
        DWORD length = ::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                                            FORMAT_MESSAGE_IGNORE_INSERTS,
                                        nullptr, static_cast<DWORD>(ec),
                                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msg, 0, nullptr);
        defer(::LocalFree(msg));

        if (length && msg[length - 1] == '\n') msg[--length] = '\0';
        if (length && msg[length - 1] == '\r') msg[--length] = '\0';
        if (length)
            return msg;
        else
            return "system error code: " + std::to_string(errno);
    };

} // namespace probe::util

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
        if (::RegGetValue(key, to_utf16(subkey).c_str(), to_utf16(valuename).c_str(), RRF_RT_REG_DWORD,
                          nullptr, &value, &size) == ERROR_SUCCESS) {
            return value;
        }

        return std::nullopt;
    }

    // REG_SZ           : A null - terminated string.
    //                    It's either a Unicode or an ANSI string,
    //                    depending on whether you use the Unicode or ANSI functions.
    template<>
    std::optional<std::string> read<std::string>(HKEY key, const std::string& subkey,
                                                 const std::string& valuename)
    {
        DWORD size = 0;
        if (::RegGetValue(key, to_utf16(subkey).c_str(), to_utf16(valuename).c_str(), RRF_RT_REG_SZ,
                          nullptr, nullptr, &size) != ERROR_SUCCESS) {
            return std::nullopt;
        }

        std::string value(size, {});
        if (::RegGetValue(key, to_utf16(subkey).c_str(), to_utf16(valuename).c_str(), RRF_RT_REG_SZ,
                          nullptr, reinterpret_cast<LPBYTE>(value.data()), &size) != ERROR_SUCCESS) {
            return std::nullopt;
        }

        return to_utf8(value);
    }
} // namespace probe::util::registry

namespace probe::util::registry
{
    int RegistryListener::listen(const std::any& obj, const std::function<void(const std::any&)>& callback)
    {
        const auto& [key, subkey] = std::any_cast<std::pair<HKEY, std::string>>(obj);

        if (::RegOpenKeyEx(key, to_utf16(subkey).c_str(), 0, KEY_NOTIFY, &key_) != ERROR_SUCCESS) {
            return -1;
        }

        if (STOP_EVENT.attach(::CreateEvent(nullptr, TRUE, FALSE, L"Stop")); !STOP_EVENT) {
            return -1;
        }

        if (NOTIFY_EVENT.attach(::CreateEvent(nullptr, FALSE, FALSE, L"Notify")); !NOTIFY_EVENT) {
            return -1;
        }

        running_ = true;
        thread_  = std::jthread([=, this] {
            probe::thread::set_name("listen-" + subkey);

            const HANDLE events[] = { STOP_EVENT.get(), NOTIFY_EVENT.get() };

            while (running_) {
                if (::RegNotifyChangeKeyValue(key_, TRUE, REG_LEGAL_CHANGE_FILTER, NOTIFY_EVENT.get(),
                                               TRUE) != ERROR_SUCCESS) {
                    ::SetEvent(STOP_EVENT.get());
                }

                switch (::WaitForMultipleObjects(2, events, false, INFINITE)) {
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
        if (running_.compare_exchange_strong(expected, false)) {
            ::SetEvent(STOP_EVENT.get());

            if (thread_.joinable()) thread_.join();

            ::RegCloseKey(key_);
        }
    }

    RegistryListener::~RegistryListener() { stop(); }
} // namespace probe::util::registry

namespace probe::util::setup
{
    template<>
    PROBE_API std::optional<std::string> property<std::string>(HDEVINFO info_set, PSP_DEVINFO_DATA info,
                                                               DWORD type)
    {
        DWORD size{};
        ::SetupDiGetDeviceRegistryProperty(info_set, info, type, nullptr, nullptr, 0, &size);

        std::vector<char> buffer(size + 2, 0);
        if (!::SetupDiGetDeviceRegistryProperty(info_set, info, type, nullptr,
                                                reinterpret_cast<LPBYTE>(buffer.data()), size, nullptr))
            return std::nullopt;

        return probe::util::to_utf8(buffer.data());
    }

    std::string device_instance_id(DEVINST inst)
    {
        std::array<wchar_t, MAX_DEVICE_ID_LEN + 1> buffer{};

        if (::CM_Get_Device_ID(inst, buffer.data(), MAX_DEVICE_ID_LEN + 1, 0) != CR_SUCCESS) {
            return "";
        }
        return probe::util::to_utf8(buffer.data());
    }
} // namespace probe::util::setup
#endif // _WIN32