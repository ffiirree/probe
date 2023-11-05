#ifndef PROBE_SMBIOS_H
#define PROBE_SMBIOS_H

#include "probe/dllport.h"
#include "probe/types.h"

#include <cstdint>
#include <vector>

namespace probe::smbios
{
    enum class smbios_type_t : uint8_t
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

    struct smbios_header_t
    {
        smbios_type_t type{};
        uint8_t       length{};
        uint16_t      handle{};
    };

    // System Management BIOS
    struct smbios_t
    {
        version_t                      version{};
        std::vector<uint8_t>           data{};
        std::vector<smbios_header_t *> table{};
    };

    PROBE_API smbios_t smbios();
} // namespace probe::smbios

namespace probe
{
    PROBE_API std::string to_string(smbios::smbios_type_t);
}
#endif //! PROBE_SMBIOS_H