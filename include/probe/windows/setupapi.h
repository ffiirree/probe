#ifndef PROBE_WINDOWS_SETUPAPI_H
#define PROBE_WINDOWS_SETUPAPI_H

#ifdef _WIN32

#include <optional>
#include <string>

// clang-format off
#include <Windows.h>
#include <SetupAPI.h>
#include <cfgmgr32.h>
#include <devguid.h>
// clang-format on

#include "probe/dllport.h"

namespace probe::util::setup
{
    template<typename T>
    PROBE_API std::optional<T> property(HDEVINFO info_set, PSP_DEVINFO_DATA info, DWORD type);

    template<>
    PROBE_API std::optional<std::string> property<std::string>(HDEVINFO info_set, PSP_DEVINFO_DATA info,
                                                               DWORD type);

    PROBE_API std::string device_instance_id(DEVINST inst);

} // namespace probe::util::setup

#endif

#endif //! PROBE_WINDOWS_SETUPAPI_H