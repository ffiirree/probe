#ifndef PROBE_SMBIOS_H
#define PROBE_SMBIOS_H

#include "probe/dllport.h"
#include "probe/types.h"

#include <cstdint>
#include <vector>

// System Management BIOS
namespace probe::smbios
{
    enum class type_t : uint8_t
    {
        // OEM
        BIOS                                       = 0x00, // Required
        System                                     = 0x01, // Required
        Baseboard                                  = 0x02,
        Chassis                                    = 0x03, // Required
        Processor                                  = 0x04, // Required
        MemoryController                           = 0x05,
        MemoryModule                               = 0x06,
        Cache                                      = 0x07, // Required
        PortConnector                              = 0x08,
        SystemSlots                                = 0x09, // Required
        OnBoardDevices                             = 0x0a,
        OEMStrings                                 = 0x0b,
        SystemConfigurationOptions                 = 0x0c,
        BIOSLanguage                               = 0x0d,
        GroupAssociations                          = 0x0e,
        SystemEventLog                             = 0x0f,
        PhysicalMemoryArray                        = 0x10, // Required
        MemoryDevice                               = 0x11, // Required
        MemoryError32Bit                           = 0x12,
        MemoryArrayMappedAddress                   = 0x13, // Required
        MemoryDeviceMappedAddress                  = 0x14,
        BuiltInPointingDevice                      = 0x15,
        PortableBattery                            = 0x16,
        SystemReset                                = 0x17,
        HardwareSecurity                           = 0x18,
        SystemPowerControls                        = 0x19,
        VoltageProbe                               = 0x1a,
        CoolingDevice                              = 0x1b,
        TemperatureProbe                           = 0x1c,
        ElectricalCurrentProbe                     = 0x1d,
        OutOfBandRemoteAccess                      = 0x1e,
        BootIntegrityServices                      = 0x1f,
        SystemBoot                                 = 0x20, // Required
        MemoryError64Bit                           = 0x21,
        ManagementDevice                           = 0x22,
        ManagementDeviceComponent                  = 0x23,
        ManagementDeviceThresholdData              = 0x24,
        MemoryChannel                              = 0x25,
        IPMIDevice                                 = 0x26,
        SystemPowerSupply                          = 0x27,
        Additional                                 = 0x28,
        OnboardDevicesExtended                     = 0x29,
        ManagementControllerHostInterface          = 0x2a,
        TPMDevice                                  = 0x2b,
        ProcessorAdditional                        = 0x2c,
        FirmwareInventory                          = 0x2d,
        StringProperty                             = 0x2e,
        Inactive                                   = 0x7e,
        EndOfTable                                 = 0x7f,
        // INTEL
        Intel_vPro                                 = 0x83,
        // LENOVO
        Lenovo_ThinkVantageTechnologiesFeatureBits = 0x83,
        Lenovo_DevicePresenceDetectionBits         = 0x87,
        Lenovo_ThinkPadEmbeddedControllerProgram   = 0x8c,
        // ACER
        AcerHotkeyFunction                         = 0xaa,
        // FUJITSU
        FSC_OMFBoardID                             = 0xb0,
        FSC_OMFBIOSID                              = 0xb1,
        FSC_SerialAndParallelPortConfiguration     = 0xb3,
        FSC_AdvancedPowerManagementConfiguration   = 0xb4,
        FSC_DisplayIdentification                  = 0xba,
        FSC_BIOSErrorLog                           = 0xbb,
        FSC_BIOSIdentification                     = 0xbc,
        FSC_HardwareControl                        = 0xbd,
        FSC_MultipurposeStaticDMIStructures        = 0xbe,
        // HP
        HP_SuperIoEnableDisableFeatures            = 0xc2,
        HP_CPUMicrocodePatch                       = 0xc7,
        HP_DeviceCorrelationRecord                 = 0xcb,
        HP_ProLiantSystemRackLocator               = 0xcc,
        HP_BIOS_PXE_NIC_PCI_MAC_209                = 0xd1,
        HP_64bitCRU                                = 0xd4,
        HP_VersionIndicatorRecord                  = 0xd8,
        HP_ProLiant                                = 0xdb,
        HP_BIOS_iSCSI_NIC_PCI_MAC                  = 0xdd,
        HP_TrustedModuleStatus                     = 0xe0,
        HP_PowerSupply                             = 0xe6,
        HP_BIOS_PXE_NIC_PCI_MAC_233                = 0xe9,
        HPE_ProLiantHDDBackplane                   = 0xec,
        HPE_DIMMVendorPartNumber                   = 0xed,
        HPE_USBPortConnectorCorrelationRecord      = 0xee,
        HPE_ProliantInventoryRecord                = 0xf0,
        HPE_HardDriveInventoryRecord               = 0xf2,
        // DELL
        DELL_RevisionsAndIDs                       = 0xd0,
        DELL_ParallelPort                          = 0xd1,
        DELL_SerialPort                            = 0xd2,
        DELL_IRPort                                = 0xd3,
    };

    struct header_t
    {
        type_t   type;
        uint8_t  length;
        uint16_t handle;
    };

    struct item_t
    {
        type_t                    type;
        uint8_t                   length;
        uint16_t                  handle;
        uint8_t                  *fields{};
        std::vector<const char *> strings{};
    };

    // System Management BIOS
    struct smbios_t
    {
        version_t            version{};
        std::vector<uint8_t> data{};
        std::vector<item_t>  table{};
    };

    PROBE_API smbios_t smbios();

#pragma pack(push, 1)

    struct PhysicalMemoryArray
    {
        uint8_t  Location{};                    // 2.1+
        uint8_t  Use{};                         // 2.1+
        uint8_t  MemoryErrorCorrection{};       // 2.1+
        uint32_t MaximumCapacity{};             // 2.1+
        uint16_t MemoryErrorInfomationHandle{}; // 2.1+
        uint16_t NumberOfMemoryDevices{};       // 2.1+
        uint64_t ExtendedMaximumCapacity{};     // 2.7+
    };

    struct MemoryDevice
    {
        uint16_t PhysicalMemeoryArrayHandle{};              // 2.1+
        uint16_t MemoryErrorInformationHandle{};            // 2.1+
        uint16_t TotalWidth{};                              // 2.1+, in bits
        uint16_t DataWidth{};                               // 2.1+, in bits
        uint16_t Size{};                                    // 2.1+,
        uint8_t  FormFactor{};                              // 2.1+,
        uint8_t  DeviceSet{};                               // 2.1+,
        uint8_t  DeviceLocator{};                           // 2.1+,
        uint8_t  BankLocator{};                             // 2.1+,
        uint8_t  MemoryType{};                              // 2.1+,
        uint16_t TypeDetail{};                              // 2.1+,
        uint16_t Speed{};                                   // 2.3+
        uint8_t  Manufacturer{};                            // 2.3+
        uint8_t  SerialNumber{};                            // 2.3+
        uint8_t  AssetTag{};                                // 2.3+
        uint8_t  PartNumber{};                              // 2.3+
        uint8_t  Attributes{};                              // 2.6+
        uint32_t ExtendedSize{};                            // 2.7+
        uint16_t ConfiguredMemeorySpeed{};                  // 2.7+
        uint16_t MinimumVoltage{};                          // 2.8+
        uint16_t MaximumVoltage{};                          // 2.8+
        uint16_t ConfiguredVoltage{};                       // 2.8+
        uint8_t  MemoryTechnology{};                        // 3.2+
        uint16_t MemoryOperatingModeCapability{};           // 3.2+
        uint8_t  FirmwareVersion{};                         // 3.2+
        uint16_t ModuleManufacturerID{};                    // 3.2+
        uint16_t ModuleProductID{};                         // 3.2+
        uint16_t MemorySubsystemControllerManufacturerID{}; // 3.2+
        uint16_t MemorySubsystemControllerProductID{};      // 3.2+
        uint64_t NonvolatileSize{};                         // 3.2+
        uint64_t VolatileSize{};                            // 3.2+
        uint64_t CacheSize{};                               // 3.2+
        uint64_t LogicalSize{};                             // 3.2+
        uint32_t ExtendedSpeed{};                           // 3.3+
        uint32_t ExtendedConfiguredMemeorySpeed{};          // 3.3+
        uint16_t PMIC0ManufacturerID{};                     // 3.7+
        uint16_t PMIC0RevisionNumber{};                     // 3.7+
        uint16_t RCDManufacturerID{};                       // 3.7+
        uint16_t RCDRevisionNumber{};                       // 3.7+
    };
#pragma pack(pop)
} // namespace probe::smbios

namespace probe
{
    PROBE_API std::string to_string(smbios::type_t);
}
#endif //! PROBE_SMBIOS_H