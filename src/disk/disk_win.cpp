#ifdef _WIN32

#include "probe/defer.h"
#include "probe/disk.h"
#include "probe/util.h"

#include <SetupAPI.h>
#include <Windows.h>
#include <array>
#include <bitset>
#include <format>
#include <iostream>

namespace probe::disk
{
    static drive_t drive_info(const WCHAR *path)
    {
        drive_t drive{ .path = probe::util::to_utf8(path) };
        STORAGE_DEVICE_NUMBER number;

        // get the physical driver number
        HANDLE handle = ::CreateFile(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                     OPEN_EXISTING, 0, nullptr);

        if(INVALID_HANDLE_VALUE == handle) return drive;
        defer(::CloseHandle(handle); handle = INVALID_HANDLE_VALUE);

        // number
        DWORD bytes{};
        if(::DeviceIoControl(handle, IOCTL_STORAGE_GET_DEVICE_NUMBER, nullptr, 0, &number,
                             sizeof(STORAGE_DEVICE_NUMBER), &bytes, nullptr)) {
            drive.number = number.DeviceNumber;
            drive.name   = std::format("\\\\.\\PhysicalDrive{}", number.DeviceNumber);
        }

        // geometry
        DISK_GEOMETRY geometry{};
        if(::DeviceIoControl(handle, IOCTL_DISK_GET_DRIVE_GEOMETRY, nullptr, 0, &geometry,
                             sizeof(DISK_GEOMETRY), &bytes, nullptr)) {
            drive.cylinders           = static_cast<uint64_t>(geometry.Cylinders.QuadPart);
            drive.tracks_per_cylinder = geometry.TracksPerCylinder;
            drive.sectors_per_track   = geometry.SectorsPerTrack;
            drive.bytes_per_sector    = geometry.BytesPerSector;
        }

        // Storage Device Property
        STORAGE_PROPERTY_QUERY query          = { StorageDeviceProperty, PropertyStandardQuery };
        STORAGE_DESCRIPTOR_HEADER desc_header = { 0 };
        if(::DeviceIoControl(handle, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(STORAGE_PROPERTY_QUERY),
                             &desc_header, sizeof(STORAGE_DESCRIPTOR_HEADER), &bytes, nullptr)) {

            std::vector<char> buffer(desc_header.Size, {});

            if(::DeviceIoControl(handle, IOCTL_STORAGE_QUERY_PROPERTY, &query,
                                 sizeof(STORAGE_PROPERTY_QUERY), buffer.data(), desc_header.Size, &bytes,
                                 nullptr)) {

                auto descriptor = reinterpret_cast<PSTORAGE_DEVICE_DESCRIPTOR>(buffer.data());

                drive.bus       = static_cast<bus_type_t>(descriptor->BusType);
                drive.removable = !!descriptor->RemovableMedia;

                if(descriptor->SerialNumberOffset)
                    drive.serial_number = buffer.data() + descriptor->SerialNumberOffset;

                if(descriptor->VendorIdOffset) drive.vendor_id = buffer.data() + descriptor->VendorIdOffset;

                if(descriptor->ProductIdOffset)
                    drive.product_id = buffer.data() + descriptor->ProductIdOffset;

                std::cout << (int)descriptor->DeviceType << "\n";
            }
        }

        // writable
        drive.writable =
            ::DeviceIoControl(handle, IOCTL_DISK_IS_WRITABLE, nullptr, 0, nullptr, 0, nullptr, nullptr);

        // layout
        PDRIVE_LAYOUT_INFORMATION_EX layout = (PDRIVE_LAYOUT_INFORMATION_EX)malloc(
            FIELD_OFFSET(DRIVE_LAYOUT_INFORMATION_EX, PartitionEntry[128]));
        defer(free(layout));
        if(::DeviceIoControl(handle, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, nullptr, 0, layout,
                             FIELD_OFFSET(DRIVE_LAYOUT_INFORMATION_EX, PartitionEntry[128]), &bytes,
                             nullptr)) {
            drive.partitions = layout->PartitionCount;
            drive.style      = static_cast<partition_style_t>(layout->PartitionStyle);

            std::string disk_id{};
            switch(layout->PartitionStyle) {
            case PARTITION_STYLE_MBR: drive.id = std::format("{:X}", layout->Mbr.Signature); break;
            case PARTITION_STYLE_GPT: {
                WCHAR buffer[256]{};
                if(StringFromGUID2(layout->Gpt.DiskId, buffer, sizeof(buffer)) > 0) {
                    drive.id = probe::util::to_utf8(buffer);
                }
                break;
            }
            default: break;
            }
        }

        // trim
        STORAGE_PROPERTY_QUERY trim_query = { StorageDeviceTrimProperty, PropertyStandardQuery };
        DEVICE_TRIM_DESCRIPTOR trim{};
        if(::DeviceIoControl(handle, IOCTL_STORAGE_QUERY_PROPERTY, &trim_query,
                             sizeof(STORAGE_PROPERTY_QUERY), &trim, sizeof(DEVICE_TRIM_DESCRIPTOR), &bytes,
                             nullptr)) {
            drive.trim = (trim.Version == sizeof(DEVICE_TRIM_DESCRIPTOR)) && (trim.TrimEnabled == 1);
        }

        return drive;
    }

    // https://devblogs.microsoft.com/oldnewthing/20201019-00/?p=104380
    //
    // Physical Drive / Volume / Mount Point
    //
    // At the bottom of the storage hierarchy are physical drives. These are units of physical storage,
    // access to which is governed by a single disk controller.
    //
    // The next layer up is the volume. A volume is region of storage that is managed by a single file
    // system. The relationship between volumes and physical drives is typically one-to-one, but it doesn’t
    // have to be.
    //
    // You might take your physical drive and create multiple partitions, and then format each partition
    // separately. Each of those formatted partitions is its own volume.
    //
    // Or you might get really fancy and use a feature like spanned volumes or Storage Spaces to take
    // multiple physical drives and combine them into one giant volume.
    //
    // Mount points are places that volumes are inserted into the namespace and become paths. The most usual
    // place to see them is as a drive letter.
    //
    // Volumes don’t have to be mounted as drive letters, though. You can also mount them inside a
    // subdirectory of an existing volume, sort of like grafting one tree onto another.
    //
    // Note that a volume can be mounted in multiple places, or it might not be mounted at all.
    std::vector<drive_t> physical_drives()
    {
        std::vector<drive_t> drives{};

        HDEVINFO device_sets =
            ::SetupDiGetClassDevs(&DiskClassGuid, nullptr, nullptr, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

        if(INVALID_HANDLE_VALUE == device_sets) return {};
        defer(::SetupDiDestroyDeviceInfoList(device_sets));

        SP_DEVICE_INTERFACE_DATA idevice{ .cbSize = sizeof(SP_DEVICE_INTERFACE_DATA) };
        PSP_DEVICE_INTERFACE_DETAIL_DATA idevice_detail{};
        DWORD size{};

        for(DWORD idx = 0;
            ::SetupDiEnumDeviceInterfaces(device_sets, nullptr, &DiskClassGuid, idx, &idevice); ++idx) {

            ::SetupDiGetDeviceInterfaceDetail(device_sets, &idevice, nullptr, 0, &size, nullptr);
            if(!(ERROR_INSUFFICIENT_BUFFER == ::GetLastError())) continue;

            idevice_detail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(size);
            defer(free(idevice_detail); idevice_detail = nullptr);

            ::ZeroMemory(idevice_detail, size);
            idevice_detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

            if(!::SetupDiGetDeviceInterfaceDetail(device_sets, &idevice, idevice_detail, size, nullptr,
                                                  nullptr)) {
                continue;
            }

            drives.push_back(drive_info(idevice_detail->DevicePath));
        }

        return drives;
    }

    std::vector<partition_t> partitions(const drive_t& drive)
    {
        HANDLE handle =
            ::CreateFile(probe::util::to_utf16(drive.path).c_str(), GENERIC_READ,
                         FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
        if(INVALID_HANDLE_VALUE == handle) return {};
        defer(::CloseHandle(handle); handle = INVALID_HANDLE_VALUE);

        std::vector<partition_t> ret;
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
            if(0 == layout->PartitionEntry[i].PartitionNumber) continue;

            std::string part_id{};
            std::string type_id{};
            std::string name{};
            switch(layout->PartitionEntry[i].PartitionStyle) {
            case PARTITION_STYLE_MBR: {
                WCHAR buffer[256]{};
                if(StringFromGUID2(layout->PartitionEntry[i].Mbr.PartitionId, buffer, sizeof(buffer)) > 0) {
                    part_id = probe::util::to_utf8(buffer);
                }

                // type id
                type_id = std::format("{:X}", layout->PartitionEntry[i].Mbr.PartitionType);
                break;
            }
            case PARTITION_STYLE_GPT: {
                WCHAR buffer[256]{};

                // partition id
                if(StringFromGUID2(layout->PartitionEntry[i].Gpt.PartitionId, buffer, sizeof(buffer)) > 0) {
                    part_id = probe::util::to_utf8(buffer);
                }

                // name
                name = probe::util::to_utf8(layout->PartitionEntry[i].Gpt.Name);

                // type id
                if(StringFromGUID2(layout->PartitionEntry[i].Gpt.PartitionType, buffer, sizeof(buffer)) >
                   0) {
                    type_id = probe::util::to_utf8(buffer);
                }
                break;
            }
            case PARTITION_STYLE_RAW: break;
            }

            ret.push_back({
                .name    = name,
                .number  = layout->PartitionEntry[i].PartitionNumber,
                .style   = static_cast<partition_style_t>(layout->PartitionEntry[i].PartitionStyle),
                .type_id = type_id,
                .guid    = part_id,
                .offset  = static_cast<uint64_t>(layout->PartitionEntry[i].StartingOffset.QuadPart),
                .length  = static_cast<uint64_t>(layout->PartitionEntry[i].PartitionLength.QuadPart),
            });
        }

        return ret;
    }

    std::vector<volume_t> volumes()
    {
        std::vector<volume_t> ret;

        std::array<WCHAR, 512> path;

        auto handle = ::FindFirstVolume(path.data(), 512);
        bool sucess = true;
        for(; handle != INVALID_HANDLE_VALUE && sucess;
            sucess = ::FindNextVolume(handle, path.data(), 512)) {

            // info
            std::array<WCHAR, MAX_PATH> label;
            std::array<WCHAR, MAX_PATH> fsname;
            DWORD serial{};
            ::GetVolumeInformation(path.data(), label.data(), MAX_PATH, &serial, nullptr, nullptr,
                                   fsname.data(), MAX_PATH);

            // letter
            std::array<WCHAR, MAX_PATH> path_name{};
            DWORD returned_size;
            ::GetVolumePathNamesForVolumeName(path.data(), path_name.data(), MAX_PATH, &returned_size);

            // spaces
            ULARGE_INTEGER total{}, free{}, free_2_caller{};
            ::GetDiskFreeSpaceEx(path.data(), &free_2_caller, &total, &free);

            ret.push_back({
                .letter     = probe::util::to_utf8(path_name.data()),
                .label      = probe::util::to_utf8(label.data()),
                .serial     = std::format("{:X}", serial),
                .guid_path  = probe::util::to_utf8(path.data()),
                .filesystem = probe::util::to_utf8(fsname.data()),
                .capacity   = total.QuadPart,
                .free       = free.QuadPart,
            });
        }

        ::FindVolumeClose(handle);

        return ret;
    }

} // namespace probe::disk

#endif