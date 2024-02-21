#ifdef _WIN32

#include "probe/system.h"
#include "probe/util.h"

#include <Windows.h>

extern "C" NTSYSAPI NTSTATUS NTAPI RtlGetVersion(_Out_ PRTL_OSVERSIONINFOW lpVersionInformation);
constexpr auto                     VERSION_KEY = R"(Software\Microsoft\Windows NT\CurrentVersion)";
constexpr auto PERSONALIZE_KEY = R"(Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)";

static unsigned int build_number()
{
    if (const auto ubr = probe::util::registry::read<DWORD>(HKEY_LOCAL_MACHINE, VERSION_KEY, "UBR");
        ubr.has_value()) {
        return ubr.value();
    }

    // Fall back to BuildLabEx in the early version of Windows 8.1 and less.
    auto buildlabex =
        probe::util::registry::read<std::string>(HKEY_LOCAL_MACHINE, VERSION_KEY, "BuildLabEx");
    if (!buildlabex.has_value()) {
        return 0;
    }

    char *ctx{};
    auto  token = strtok_s(&buildlabex.value()[0], ".", &ctx);
    token       = strtok_s(nullptr, ".", &ctx);
    return token ? std::strtoul(token, nullptr, 10) : 0;
}

namespace probe::system
{
    std::string name()
    {
        auto name = probe::util::registry::read<std::string>(HKEY_LOCAL_MACHINE, VERSION_KEY, "ProductName")
                        .value_or("Windows");

        // Windows 11
        if (version() >= WIN_11 &&
            probe::util::registry::read<std::string>(HKEY_LOCAL_MACHINE, VERSION_KEY, "InstallationType")
                    .value_or("") == "Client") {

            if (const auto pos = name.find("Windows 10"); pos != std::string::npos) {
                name.replace(pos + 8, 2, "11");
            }
        }

        return name;
    }

    theme_t theme()
    {
        if (version() >= probe::WIN_10) {
            if (!probe::util::registry::read<DWORD>(HKEY_CURRENT_USER, PERSONALIZE_KEY, "AppsUseLightTheme")
                     .value_or(1)) {
                return theme_t::dark;
            }
        }

        return theme_t::light;
    }

    version_t version()
    {
        auto ver = kernel::version();
        ver.codename =
            probe::util::registry::read<std::string>(HKEY_LOCAL_MACHINE, VERSION_KEY, "DisplayVersion")
                .value_or("");

        return ver;
    }

    std::string hostname()
    {
        WCHAR buffer[MAX_COMPUTERNAME_LENGTH + 1]{};
        DWORD size = sizeof(buffer);
        if (::GetComputerName(buffer, &size) != 0) {
            return probe::util::to_utf8(buffer);
        }
        return {};
    }

    std::string username()
    {
        WCHAR buffer[256]{};
        DWORD size = sizeof(buffer);
        if (::GetUserName(buffer, &size) != 0) {
            return probe::util::to_utf8(buffer);
        }
        return {};
    }

    namespace kernel
    {
        std::string name() { return "Windows NT"; }

        version_t version()
        {
            // version
            RTL_OSVERSIONINFOW os_version_info  = {};
            os_version_info.dwOSVersionInfoSize = sizeof(os_version_info);
            RtlGetVersion(&os_version_info);

            // https://learn.microsoft.com/en-us/windows/win32/sysinfo/operating-system-version
            // https://en.wikipedia.org/wiki/Comparison_of_Microsoft_Windows_versions
            // Windows              11 : 10.0.22000
            // Windows              10 : 10.0
            // Windows     Server 2022 : 10.0
            // Windows     Server 2019 : 10.0
            // Windows     Server 2016 : 10.0
            // Windows             8.1 :  6.3
            // Windows  Server 2012 R2 :  6.3
            // Windows             8.0 :  6.2
            // Windows     Server 2012 :  6.2
            // Windows               7 :  6.1
            // Windows  Server 2008 R2 :  6.1
            // Windows     Server 2008 :  6.0
            // Windows           Vista :  6.0
            // Windows  Server 2003 R2 :  5.2
            // Windows     Server 2003 :  5.2
            // Windows              XP :  5.1
            // Windows            2000 :  5.0
            return {
                os_version_info.dwMajorVersion,
                os_version_info.dwMinorVersion,
                os_version_info.dwBuildNumber,
                build_number(),
            };
        }
    } // namespace kernel

} // namespace probe::system

namespace probe::system
{
    desktop_environment_t desktop_environment() { return desktop_environment_t::WindowsShell; }

    version_t desktop_environment_version() { return version(); }
} // namespace probe::system

namespace probe::system
{
    windowing_system_t windowing_system() { return windowing_system_t::DesktopWindowManager; }
} // namespace probe::system

#endif // _WIN32