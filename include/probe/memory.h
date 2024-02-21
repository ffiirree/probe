#ifndef PROBE_MEMORY_H
#define PROBE_MEMORY_H

#include "probe/dllport.h"

#include <cstdint>
#include <string>
#include <vector>

namespace probe::memory
{
    struct memory_status_t
    {
        uint64_t avail{}; // bytes
        uint64_t total{}; // bytes
    };

    PROBE_API memory_status_t status();

    enum class memory_type_t
    {
        Other                      = 0x01,
        Unknown                    = 0x02,
        DRAM                       = 0x03,
        EDRAM                      = 0x04,
        VRAM                       = 0x05,
        SRAM                       = 0x06,
        RAM                        = 0x07,
        ROM                        = 0x08,
        FLASH                      = 0x09,
        EEPROM                     = 0x0a,
        FEPROM                     = 0x0b,
        EPROM                      = 0x0c,
        CDRAM                      = 0x0d,
        _3DRAM                     = 0x0e,
        SDRAM                      = 0x0f,
        SGRAM                      = 0x10,
        RDRAM                      = 0x11,
        DDR                        = 0x12,
        DDR2                       = 0x13,
        DDR2_FB_DIMM               = 0x14,
        DDR3                       = 0x18,
        FBD2                       = 0x19,
        DDR4                       = 0x1a,
        LPDDR                      = 0x1b,
        LPDDR2                     = 0x1c,
        LPDDR3                     = 0x1d,
        LPDDR4                     = 0x1e,
        Logical_Non_VolatileDevice = 0x1f,
        HBM                        = 0x20,
        HBM2                       = 0x21,
        DDR5                       = 0x22,
        LPDDR5                     = 0x23,
        HBM3                       = 0x24,
    };

    struct PhysicalMemoryArray
    {
        uint16_t Handle{};
        uint8_t  Location{};
        uint8_t  Use{};
        uint8_t  MemoryErrorCorrection{};
        uint64_t Capacity{}; // kilobytes
        uint16_t MemoryErrorInfomationHandle{};
        uint16_t NumberOfMemoryDevices{};
    };

    // SMBIOS, Memory Device(Type 17)
    struct MemoryDevice
    {
        uint16_t      PhysicalMemeoryArrayHandle{};
        uint16_t      MemoryErrorInformationHandle{};
        uint16_t      TotalWidth{}; // bits
        uint16_t      DataWidth{};  // bits
        uint64_t      Size{};       // bytes
        uint8_t       FormFactor{};
        uint8_t       DeviceSet{};
        std::string   DeviceLocator{};
        std::string   BankLocator{};
        memory_type_t MemoryType{};
        uint16_t      TypeDetail{};
        uint32_t      Speed{}; // MT/s
        std::string   Manufacturer{};
        std::string   SerialNumber{};
        std::string   AssetTag{};
        std::string   PartNumber{};
        uint8_t       Attributes{};
        uint32_t      ConfiguredSpeed{};   // MT/s
        uint16_t      MinimumVoltage{};    // millivolts
        uint16_t      MaximumVoltage{};    // millivolts
        uint16_t      ConfiguredVoltage{}; // millivolts
        uint8_t       Technology{};
        uint16_t      OperatingModeCapability{};
        uint8_t       FirmwareVersion{};
        uint16_t      ModuleManufacturerID{};
        uint16_t      ModuleProductID{};
        uint16_t      SubsystemControllerManufacturerID{};
        uint16_t      SubsystemControllerProductID{};
        uint64_t      NonvolatileSize{}; // bytes
        uint64_t      VolatileSize{};    // bytes
        uint64_t      CacheSize{};       // bytes
        uint64_t      LogicalSize{};     // bytes
        uint16_t      PMIC0ManufacturerID{};
        uint16_t      PMIC0RevisionNumber{};
        uint16_t      RCDManufacturerID{};
        uint16_t      RCDRevisionNumber{};
    };

    PROBE_API std::vector<MemoryDevice> devices();

    PROBE_API std::vector<PhysicalMemoryArray> physical_device_arrays();
} // namespace probe::memory

#endif //! PROBE_MEMORY_H