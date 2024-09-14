#ifdef _WIN32

#include "probe/defer.h"
#include "probe/power.h"
#include "probe/util.h"
#include "probe/windows/setupapi.h"

#include <Poclass.h>

namespace probe::power
{
    // https://learn.microsoft.com/en-us/windows/win32/power/enumerating-battery-devices
    static void batteries(std::vector<supply_t>& list)
    {
        auto set = ::SetupDiGetClassDevs(&GUID_DEVCLASS_BATTERY, nullptr, nullptr,
                                         DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

        if (set == INVALID_HANDLE_VALUE) return;
        defer(::SetupDiDestroyDeviceInfoList(set));

        SP_DEVICE_INTERFACE_DATA         idevice{ .cbSize = sizeof(SP_DEVICE_INTERFACE_DATA) };
        PSP_DEVICE_INTERFACE_DETAIL_DATA idevice_detail{};
        DWORD                            size{};

        for (DWORD idx = 0;
             ::SetupDiEnumDeviceInterfaces(set, nullptr, &GUID_DEVCLASS_BATTERY, idx, &idevice); ++idx) {

            ::SetupDiGetDeviceInterfaceDetail(set, &idevice, nullptr, 0, &size, nullptr);
            if (ERROR_INSUFFICIENT_BUFFER != ::GetLastError()) continue;

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

                    // information
                    BATTERY_INFORMATION info{};
                    query.InformationLevel = BatteryInformation;
                    if (::DeviceIoControl(handle, IOCTL_BATTERY_QUERY_INFORMATION, &query, sizeof(query),
                                          &info, sizeof(info), &out, nullptr)) {
                        if (!(info.Capabilities & BATTERY_SYSTEM_BATTERY)) {
                            continue;
                        }
                    }

                    // manufacturer
                    query.InformationLevel = BatteryManufactureName;
                    wchar_t mfg[MAX_PATH]{};
                    if (!::DeviceIoControl(handle, IOCTL_BATTERY_QUERY_INFORMATION, &query, sizeof(query),
                                           mfg, sizeof(mfg), &out, nullptr)) {
                        continue;
                    }

                    // serial number
                    query.InformationLevel = BatterySerialNumber;
                    wchar_t serial[MAX_PATH]{};
                    if (!::DeviceIoControl(handle, IOCTL_BATTERY_QUERY_INFORMATION, &query, sizeof(query),
                                           serial, sizeof(serial), &out, nullptr)) {
                        continue;
                    }

                    // name
                    query.InformationLevel = BatteryDeviceName;
                    wchar_t buffer[MAX_PATH]{};
                    if (::DeviceIoControl(handle, IOCTL_BATTERY_QUERY_INFORMATION, &query, sizeof(query),
                                          buffer, sizeof(buffer), &out, nullptr)) {
                        list.emplace_back(supply_t{
                            .name                  = probe::util::to_utf8(buffer),
                            .path                  = probe::util::to_utf8(idevice_detail->DevicePath),
                            .serial                = probe::util::to_utf8(serial),
                            .manufacturer          = probe::util::to_utf8(mfg),
                            .cycle                 = info.CycleCount,
                            .chemistry             = reinterpret_cast<char *>(info.Chemistry),
                            .capacity              = info.DesignedCapacity,
                            .full_charged_capacity = info.FullChargedCapacity,
                            .type                  = supply_type_t::Battery,
                        });
                    }
                }
            }
        }
    }

    std::vector<supply_t> supplies()
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

        batteries(ret);
        return ret;
    }
} // namespace probe::power

#endif