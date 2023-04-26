#ifdef _WIN32

#include "probe/defer.h"
#include "probe/disk.h"
#include "probe/util.h"

#include <SetupAPI.h>
#include <Windows.h>
#include <bitset>
#include <iostream>

namespace probe::disk
{
    std::vector<drive_t> physical_devices()
    {
        std::vector<drive_t> device_paths{};

        HDEVINFO device_sets =
            ::SetupDiGetClassDevs(&DiskClassGuid, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

        if(INVALID_HANDLE_VALUE == device_sets) return {};
        defer(::SetupDiDestroyDeviceInfoList(device_sets));

        SP_DEVICE_INTERFACE_DATA device_interface{ .cbSize = sizeof(SP_DEVICE_INTERFACE_DATA) };
        PSP_DEVICE_INTERFACE_DETAIL_DATA device_interface_detail{};
        STORAGE_DEVICE_NUMBER number;

        for(DWORD idx = 0;
            ::SetupDiEnumDeviceInterfaces(device_sets, nullptr, &DiskClassGuid, idx, &device_interface);
            ++idx) {
            DWORD size{};
            ::SetupDiGetDeviceInterfaceDetail(device_sets, &device_interface, nullptr, 0, &size, nullptr);
            if(!(ERROR_INSUFFICIENT_BUFFER == GetLastError())) return device_paths;

            device_interface_detail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(size);
            defer(free(device_interface_detail); device_interface_detail = nullptr);

            ::ZeroMemory(device_interface_detail, size);
            device_interface_detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

            if(!::SetupDiGetDeviceInterfaceDetail(device_sets, &device_interface, device_interface_detail,
                                                  size, nullptr, nullptr)) {
                return device_paths;
            }

            // get the physical driver number
            HANDLE handle =
                ::CreateFile(device_interface_detail->DevicePath, GENERIC_READ,
                             FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);

            if(INVALID_HANDLE_VALUE == handle) continue;

            defer(::CloseHandle(handle); handle = INVALID_HANDLE_VALUE);

            DWORD bytes{};
            if(!::DeviceIoControl(handle, IOCTL_STORAGE_GET_DEVICE_NUMBER, nullptr, 0, &number,
                                  sizeof(STORAGE_DEVICE_NUMBER), &bytes, nullptr)) {
                return device_paths;
            }

            DISK_GEOMETRY geometry{};
            if(!::DeviceIoControl(handle, IOCTL_DISK_GET_DRIVE_GEOMETRY, nullptr, 0, &geometry,
                                  sizeof(DISK_GEOMETRY), &bytes, nullptr)) {
                return device_paths;
            }

            STORAGE_PROPERTY_QUERY query          = { StorageDeviceProperty, PropertyStandardQuery };
            STORAGE_DESCRIPTOR_HEADER desc_header = { 0 };
            if(!::DeviceIoControl(handle, IOCTL_STORAGE_QUERY_PROPERTY, &query,
                                  sizeof(STORAGE_PROPERTY_QUERY), &desc_header,
                                  sizeof(STORAGE_DESCRIPTOR_HEADER), &bytes, nullptr)) {

                return device_paths;
            }
            // std::cout << desc_header.Size << ", " << desc_header.Version << "\n";

            std::vector<char> buffer(desc_header.Size, {});

            if(!::DeviceIoControl(handle, IOCTL_STORAGE_QUERY_PROPERTY, &query,
                                  sizeof(STORAGE_PROPERTY_QUERY), buffer.data(), desc_header.Size, &bytes,
                                  nullptr)) {

                return device_paths;
            }

            auto descriptor = reinterpret_cast<PSTORAGE_DEVICE_DESCRIPTOR>(buffer.data());
            device_paths.push_back({
                .name   = "\\\\.\\PhysicalDrive" + std::to_string(number.DeviceNumber),
                .path   = probe::util::to_utf8(device_interface_detail->DevicePath),
                .number = number.DeviceNumber,
                .serial_number =
                    !descriptor->SerialNumberOffset ? "" : buffer.data() + descriptor->SerialNumberOffset,
                .vendor_id = !descriptor->VendorIdOffset ? "" : buffer.data() + descriptor->VendorIdOffset,
                .product_id =
                    !descriptor->ProductIdOffset ? "" : buffer.data() + descriptor->ProductIdOffset,
                .cylinders           = static_cast<uint64_t>(geometry.Cylinders.QuadPart),
                .tracks_per_cylinder = geometry.TracksPerCylinder,
                .sectors_per_track   = geometry.SectorsPerTrack,
                .bytes_per_sector    = geometry.BytesPerSector,
            });
        }

        return device_paths;
    }

    std::vector<volume_t> volumes(const drive_t& drive)
    {
        HANDLE handle =
            ::CreateFile(probe::util::to_utf16(drive.path).c_str(), GENERIC_READ,
                         FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
        if(INVALID_HANDLE_VALUE == handle) return {};
        defer(::CloseHandle(handle); handle = INVALID_HANDLE_VALUE);

        std::vector<volume_t> ret;
        PDRIVE_LAYOUT_INFORMATION_EX layout{};

        //
        layout = (PDRIVE_LAYOUT_INFORMATION_EX)malloc(
            FIELD_OFFSET(DRIVE_LAYOUT_INFORMATION_EX, PartitionEntry[6]));

        defer(free(layout));

        DWORD bytes{};
        if(!::DeviceIoControl(handle, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, nullptr, 0, layout,
                              FIELD_OFFSET(DRIVE_LAYOUT_INFORMATION_EX, PartitionEntry[6]), &bytes,
                              nullptr)) {
            return ret;
        }

        for(DWORD i = 0; i < layout->PartitionCount; ++i) {
            ret.push_back({
                .name  = "",
                .style = static_cast<partition_style_t>(layout->PartitionEntry[i].PartitionStyle),
            });
        }

        return ret;
    }

} // namespace probe::disk

#endif