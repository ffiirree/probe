#ifdef _WIN32

#include "probe/memory.h"
#include "probe/smbios.h"

#include <Windows.h>

namespace probe::memory
{
    memory_status_t status()
    {
        MEMORYSTATUSEX statex{ .dwLength = sizeof(MEMORYSTATUSEX) };

        if (!::GlobalMemoryStatusEx(&statex)) return {};

        return {
            .avail = statex.ullAvailPhys,
            .total = statex.ullTotalPhys,
        };
    }

    std::vector<PhysicalMemoryArray> physical_device_arrays()
    {
        std::vector<PhysicalMemoryArray> list{};

        const auto smbios = smbios::smbios();
        for (const auto& item : smbios.table) {
            if (item.type == smbios::type_t::PhysicalMemoryArray) {

                smbios::PhysicalMemoryArray arr{};
                std::memcpy(&arr, item.fields, std::min<size_t>(sizeof(arr), item.length));

                list.push_back({
                    .Handle                = item.handle,
                    .Location              = arr.Location,
                    .Use                   = arr.Use,
                    .MemoryErrorCorrection = arr.MemoryErrorCorrection,
                    .Capacity = (arr.MaximumCapacity == 0x8000'0000) ? arr.ExtendedMaximumCapacity
                                                                     : arr.MaximumCapacity,
                    .MemoryErrorInfomationHandle = arr.MemoryErrorInfomationHandle,
                    .NumberOfMemoryDevices       = arr.NumberOfMemoryDevices,
                });
            }
        }

        return list;
    }

    std::vector<MemoryDevice> devices()
    {
        std::vector<MemoryDevice> list{};

        const auto smbios = smbios::smbios();
        for (const auto& item : smbios.table) {
            if (item.type == smbios::type_t::MemoryDevice) {
                smbios::MemoryDevice dev{};
                std::memcpy(&dev, item.fields, std::min<size_t>(sizeof(dev), item.length));
                uint64_t size = 0;
                if (dev.Size != 0 && dev.Size != 0xffff) {
                    size = (dev.Size == 0x7fff)
                               ? (dev.ExtendedSize * 1'024ull * 1'024ull)
                               : (dev.Size * ((dev.Size & 0x8000) ? 1'024ull : (1'024ull * 1'024ull)));
                }

                list.push_back({
                    .PhysicalMemeoryArrayHandle   = dev.PhysicalMemeoryArrayHandle,
                    .MemoryErrorInformationHandle = dev.MemoryErrorInformationHandle,
                    .TotalWidth                   = dev.TotalWidth,
                    .DataWidth                    = dev.DataWidth,
                    .Size                         = size,
                    .FormFactor                   = dev.FormFactor,
                    .DeviceSet                    = dev.DeviceSet,
                    .DeviceLocator =
                        dev.DeviceLocator <= item.strings.size() ? item.strings[dev.DeviceLocator - 1] : "",
                    .BankLocator =
                        dev.BankLocator <= item.strings.size() ? item.strings[dev.BankLocator - 1] : "",
                    .MemoryType = static_cast<memory_type_t>(dev.MemoryType),
                    .TypeDetail = dev.TypeDetail,
                    .Speed      = dev.Speed == 0xF'FFFF ? dev.ExtendedSpeed : dev.Speed,
                    .Manufacturer =
                        dev.Manufacturer <= item.strings.size() ? item.strings[dev.Manufacturer - 1] : "",
                    .SerialNumber =
                        dev.SerialNumber <= item.strings.size() ? item.strings[dev.SerialNumber - 1] : "",
                    .AssetTag = dev.AssetTag <= item.strings.size() ? item.strings[dev.AssetTag - 1] : "",
                    .PartNumber =
                        dev.PartNumber <= item.strings.size() ? item.strings[dev.PartNumber - 1] : "",
                    .Attributes                        = dev.Attributes,
                    .ConfiguredSpeed                   = dev.ConfiguredMemeorySpeed == 0xFFFF
                                                             ? dev.ExtendedConfiguredMemeorySpeed
                                                             : dev.ConfiguredMemeorySpeed,
                    .MinimumVoltage                    = dev.MinimumVoltage,
                    .MaximumVoltage                    = dev.MaximumVoltage,
                    .ConfiguredVoltage                 = dev.ConfiguredVoltage,
                    .Technology                        = dev.MemoryTechnology,
                    .OperatingModeCapability           = dev.MemoryOperatingModeCapability,
                    .FirmwareVersion                   = dev.FirmwareVersion,
                    .ModuleManufacturerID              = dev.ModuleManufacturerID,
                    .ModuleProductID                   = dev.ModuleProductID,
                    .SubsystemControllerManufacturerID = dev.MemorySubsystemControllerManufacturerID,
                    .SubsystemControllerProductID      = dev.MemorySubsystemControllerProductID,
                    .NonvolatileSize                   = dev.NonvolatileSize,
                    .VolatileSize                      = dev.VolatileSize,
                    .CacheSize                         = dev.CacheSize,
                    .LogicalSize                       = dev.LogicalSize,
                    .PMIC0ManufacturerID               = dev.PMIC0ManufacturerID,
                    .PMIC0RevisionNumber               = dev.PMIC0RevisionNumber,
                    .RCDManufacturerID                 = dev.RCDManufacturerID,
                    .RCDRevisionNumber                 = dev.RCDRevisionNumber,
                });
            }
        }

        return list;
    }
} // namespace probe::memory

#endif