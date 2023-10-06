#ifdef _WIN32

#include "probe/defer.h"
#include "probe/serial-port.h"
#include "probe/util.h"

#include <array>
#include <cfgmgr32.h>
#include <Devguid.h>
#include <SetupAPI.h>
#include <Windows.h>

namespace probe::port
{
    static std::string name_of(HDEVINFO info_set, PSP_DEVINFO_DATA info)
    {
        HKEY hkey = ::SetupDiOpenDevRegKey(info_set, info, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
        if (hkey == INVALID_HANDLE_VALUE) return {};
        defer(::RegCloseKey(hkey));

        return probe::util::registry::read<std::string>(hkey, "", "PortName").value_or("");
    }

    std::vector<serial_port> ports()
    {
        HDEVINFO info_set = ::SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, nullptr, nullptr, DIGCF_PRESENT);

        if (INVALID_HANDLE_VALUE == info_set) return {};
        defer(::SetupDiDestroyDeviceInfoList(info_set));

        SP_DEVINFO_DATA info{ .cbSize = sizeof(SP_DEVINFO_DATA) };
        std::vector<serial_port> list{};

        for (DWORD idx = 0; ::SetupDiEnumDeviceInfo(info_set, idx, &info); ++idx) {

            auto name = name_of(info_set, &info);
            list.push_back({
                .name        = name,
                .device      = name.starts_with("COM") ? R"(\\.\)" + name : name,
                .instance_id = probe::util::setup::device_instance_id(info.DevInst),
                .description = probe::util::setup::property<std::string>(info_set, &info, SPDRP_DEVICEDESC)
                                   .value_or(""),
                .manufacturer =
                    probe::util::setup::property<std::string>(info_set, &info, SPDRP_MFG).value_or(""),

            });
        }

        return list;
    }
} // namespace probe::port

#endif