#ifdef _WIN32

#include "probe/defer.h"
#include "probe/power.h"
#include "probe/util.h"

#include <Poclass.h>
#include <SetupAPI.h>
#include <Windows.h>
#include <devguid.h>

namespace probe::power
{
    // https://learn.microsoft.com/en-us/windows/win32/power/enumerating-battery-devices
    void batteries()
    {
        auto set =
            ::SetupDiGetClassDevs(&GUID_DEVCLASS_BATTERY, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

        if (set == INVALID_HANDLE_VALUE) return;
        defer(::SetupDiDestroyDeviceInfoList(set));

        std::vector<supply_t> ret;

        SP_DEVICE_INTERFACE_DATA idevice{ .cbSize = sizeof(SP_DEVICE_INTERFACE_DATA) };
        PSP_DEVICE_INTERFACE_DETAIL_DATA idevice_detail{};
        DWORD size{};

        for (DWORD idx = 0;
             ::SetupDiEnumDeviceInterfaces(set, nullptr, &GUID_DEVCLASS_BATTERY, idx, &idevice); ++idx) {

            ::SetupDiGetDeviceInterfaceDetail(set, &idevice, nullptr, 0, &size, nullptr);
            if (!(ERROR_INSUFFICIENT_BUFFER == ::GetLastError())) continue;

            idevice_detail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(size);
            defer(free(idevice_detail); idevice_detail = nullptr);

            ::ZeroMemory(idevice_detail, size);
            idevice_detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

            if (!::SetupDiGetDeviceInterfaceDetail(set, &idevice, idevice_detail, size, nullptr, nullptr)) {
                continue;
            }

            auto handle = ::CreateFile(idevice_detail->DevicePath, GENERIC_READ | GENERIC_WRITE,
                                       FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
                                       FILE_ATTRIBUTE_NORMAL, nullptr);

            if (handle != INVALID_HANDLE_VALUE) {
                defer(::CloseHandle(handle));

                BATTERY_QUERY_INFORMATION query{};

                DWORD wait{};
                DWORD out{};
                if (::DeviceIoControl(handle, IOCTL_BATTERY_QUERY_TAG, &wait, sizeof(wait),
                                      &query.BatteryTag, sizeof(query.BatteryTag), &out, nullptr) &&
                    query.BatteryTag) {
                    BATTERY_INFORMATION info{};
                    query.InformationLevel = BatteryInformation;
                    if (::DeviceIoControl(handle, IOCTL_BATTERY_QUERY_INFORMATION, &query, sizeof(query),
                                          &info, sizeof(info), &out, nullptr)) {
                        if (info.Capabilities & BATTERY_SYSTEM_BATTERY) {}
                    }
                }
            }
        }
    }

    std::vector<supply_t> status()
    {
        SYSTEM_POWER_STATUS pstat{};
        ::GetSystemPowerStatus(&pstat);

        std::vector<supply_t> ret;
        if (pstat.ACLineStatus != 255) {
            ret.emplace_back(supply_t{
                .name = "AC",
                .type = supply_type_t::Mains,
                .status =
                    (pstat.ACLineStatus == 0) ? supply_status_t::Discharging : supply_status_t::Charging,
            });
        }
        return ret;
    }
} // namespace probe::power

#endif