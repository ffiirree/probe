#include "probe/smbios.h"

namespace probe
{
    std::string to_string(const smbios::type_t type)
    {
        switch (type) {
        case smbios::type_t::BIOS:                                     return "BIOS Information";
        case smbios::type_t::System:                                   return "System Information";
        case smbios::type_t::Baseboard:                                return "Baseboard (or Module) Information";
        case smbios::type_t::Chassis:                                  return "System Enclosure or Chassis";
        case smbios::type_t::Processor:                                return "Processor Information";
        case smbios::type_t::MemoryController:                         return "Memory Controller Information";
        case smbios::type_t::MemoryModule:                             return "Memory Module Information";
        case smbios::type_t::Cache:                                    return "Cache Information";
        case smbios::type_t::PortConnector:                            return "Port Connector Information";
        case smbios::type_t::SystemSlots:                              return "System Slot";
        case smbios::type_t::OnBoardDevices:                           return "On Board Devices Information";
        case smbios::type_t::OEMStrings:                               return "OEM Strings";
        case smbios::type_t::SystemConfigurationOptions:               return "System Configuration Options";
        case smbios::type_t::BIOSLanguage:                             return "BIOS Language Information";
        case smbios::type_t::GroupAssociations:                        return "Group Associations";
        case smbios::type_t::SystemEventLog:                           return "System Event Log";
        case smbios::type_t::PhysicalMemoryArray:                      return "Physical Memory Array";
        case smbios::type_t::MemoryDevice:                             return "Memory Device";
        case smbios::type_t::MemoryError32Bit:                         return "32-Bit Memory Error Information";
        case smbios::type_t::MemoryArrayMappedAddress:                 return "Memory Array Mapped Address";
        case smbios::type_t::MemoryDeviceMappedAddress:                return "Memory Device Mapped Address";
        case smbios::type_t::BuiltInPointingDevice:                    return "Built-in Pointing Device";
        case smbios::type_t::PortableBattery:                          return "Portable Battery";
        case smbios::type_t::SystemReset:                              return "System Reset";
        case smbios::type_t::HardwareSecurity:                         return "Hardware Security";
        case smbios::type_t::SystemPowerControls:                      return "System Power Controls";
        case smbios::type_t::VoltageProbe:                             return "Voltage Probe";
        case smbios::type_t::CoolingDevice:                            return "Cooling Device";
        case smbios::type_t::TemperatureProbe:                         return "Temperature Probe";
        case smbios::type_t::ElectricalCurrentProbe:                   return "Electrical Current Probe";
        case smbios::type_t::OutOfBandRemoteAccess:                    return "Out-of-Band Remote Access";
        case smbios::type_t::BootIntegrityServices:                    return "Boot Integrity Services Entry Point";
        case smbios::type_t::SystemBoot:                               return "System Boot Information";
        case smbios::type_t::MemoryError64Bit:                         return "64-Bit Memory Error Information";
        case smbios::type_t::ManagementDevice:                         return "Management Device";
        case smbios::type_t::ManagementDeviceComponent:                return "Management Device Component";
        case smbios::type_t::ManagementDeviceThresholdData:            return "Management Device Threshold Data";
        case smbios::type_t::MemoryChannel:                            return "Memory Channel";
        case smbios::type_t::IPMIDevice:                               return "IPMI Device Information";
        case smbios::type_t::SystemPowerSupply:                        return "System Power Supply";
        case smbios::type_t::Additional:                               return "Additional Information";
        case smbios::type_t::OnboardDevicesExtended:                   return "Onboard Devices Extended Information";
        case smbios::type_t::ManagementControllerHostInterface:        return "Management Controller Host Interface";
        case smbios::type_t::TPMDevice:                                return "TPM Device";
        case smbios::type_t::ProcessorAdditional:                      return "Processor Additional Information";
        case smbios::type_t::FirmwareInventory:                        return "Firmware Inventory Information";
        case smbios::type_t::StringProperty:                           return "String Property";
        case smbios::type_t::Inactive:                                 return "Inactive";
        case smbios::type_t::EndOfTable:                               return "End-of-Table";
        case smbios::type_t::Intel_vPro:                               return "Intel vPro";
        case smbios::type_t::Lenovo_DevicePresenceDetectionBits:       return "Lenovo Device Presence Detection bits";
        case smbios::type_t::Lenovo_ThinkPadEmbeddedControllerProgram: return "Lenovo ThinkPad Embedded Controller Program";
        case smbios::type_t::AcerHotkeyFunction:                       return "Acer Hotkey Function";
        case smbios::type_t::FSC_OMFBoardID:                           return "FSC OMF Board ID";
        case smbios::type_t::FSC_OMFBIOSID:                            return "FSC OMF BIOS ID";
        case smbios::type_t::FSC_SerialAndParallelPortConfiguration:   return "FSC Serial and Parallel Port Configuration";
        case smbios::type_t::FSC_AdvancedPowerManagementConfiguration: return "FSC Advanced Power Management Configuration";
        case smbios::type_t::FSC_DisplayIdentification:                return "FSC Display Identification";
        case smbios::type_t::FSC_BIOSErrorLog:                         return "FSC BIOS Error-Log";
        case smbios::type_t::FSC_BIOSIdentification:                   return "FSC BIOS Identification";
        case smbios::type_t::FSC_HardwareControl:                      return "FSC Hardware Control";
        case smbios::type_t::FSC_MultipurposeStaticDMIStructures:      return "FSC Multipurpose static DMI structures";
        case smbios::type_t::HP_SuperIoEnableDisableFeatures:          return "HP Super IO Enable/Disable Features";
        case smbios::type_t::HP_CPUMicrocodePatch:                     return "HP CPU Microcode Patch";
        case smbios::type_t::HP_DeviceCorrelationRecord:               return "HP Device Correlation Record";
        case smbios::type_t::HP_ProLiantSystemRackLocator:             return "HP ProLiant System/Rack Locator";
        case smbios::type_t::HP_BIOS_PXE_NIC_PCI_MAC_209:              return "HP BIOS PXE NIC PCI and MAC Information";
        case smbios::type_t::HP_64bitCRU:                              return "HP 64-bit CRU Information";
        case smbios::type_t::HP_VersionIndicatorRecord:                return "HP Version Indicator Record";
        case smbios::type_t::HP_ProLiant:                              return "HP ProLiant Information";
        case smbios::type_t::HP_BIOS_iSCSI_NIC_PCI_MAC:                return "HP BIOS iSCSI NIC PCI and MAC Information";
        case smbios::type_t::HP_TrustedModuleStatus:                   return "HP Trusted Module (TPM or TCM) Status";
        case smbios::type_t::HP_PowerSupply:                           return "HP Power Supply Information";
        case smbios::type_t::HP_BIOS_PXE_NIC_PCI_MAC_233:              return "HP BIOS PXE NIC PCI and MAC Information";
        case smbios::type_t::HPE_ProLiantHDDBackplane:                 return "HPE ProLiant HDD Backplane";
        case smbios::type_t::HPE_DIMMVendorPartNumber:                 return "HPE DIMM Vendor Part Number Information";
        case smbios::type_t::HPE_USBPortConnectorCorrelationRecord:    return "HPE USB Port Connector Correlation Record";
        case smbios::type_t::HPE_ProliantInventoryRecord:              return "HPE Proliant Inventory Record";
        case smbios::type_t::HPE_HardDriveInventoryRecord:             return "HPE Hard Drive Inventory Record";
        case smbios::type_t::DELL_RevisionsAndIDs:                     return "DELL Revisions and IDs";
        // case DELL_ParallelPort:                        return "DELL Parallel Port";
        case smbios::type_t::DELL_SerialPort:                          return "DELL Serial Port";
        case smbios::type_t::DELL_IRPort:                              return "DELL IR Port";
        default:                                       return "Unknown";
        }
    }
} // namespace probe
