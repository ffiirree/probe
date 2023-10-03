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

        //        HKEY hKey = nullptr;
        //        if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_READ,
        //        &hKey) !=
        //            ERROR_SUCCESS) {
        //            return {};
        //        }
        //        defer(::RegCloseKey(hKey));
        //
        //
        //        TCHAR name[MAX_PATH]{};
        //        BYTE desc[MAX_PATH]{};
        //        for (DWORD i = 0;; i++) {
        //            DWORD nlen = MAX_PATH;
        //            DWORD dlen = MAX_PATH;
        //            auto ret   = ::RegEnumValue(hKey, i, name, &nlen, nullptr, nullptr, desc, &dlen);
        //            if (ret == ERROR_NO_MORE_ITEMS) {
        //                break;
        //            }
        //            else if (ret != ERROR_SUCCESS) {
        //                return list;
        //            }
        //
        //            list.emplace_back(probe::util::to_utf8(reinterpret_cast<wchar_t *>(desc)),
        //                              probe::util::to_utf8(name));
        //        }

        return list;
    }
}; // namespace probe::port

#endif