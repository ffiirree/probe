#include "probe/smbios.h"

namespace probe
{
    std::string to_string(const smbios::type_t type)
    {
        using enum smbios::type_t;
        switch (type) {
        case BIOS:                                     return "BIOS Information";
        case System:                                   return "System Information";
        case Baseboard:                                return "Baseboard (or Module) Information";
        case Chassis:                                  return "System Enclosure or Chassis";
        case Processor:                                return "Processor Information";
        case MemoryController:                         return "Memory Controller Information";
        case MemoryModule:                             return "Memory Module Information";
        case Cache:                                    return "Cache Information";
        case PortConnector:                            return "Port Connector Information";
        case SystemSlots:                              return "System Slot";
        case OnBoardDevices:                           return "On Board Devices Information";
        case OEMStrings:                               return "OEM Strings";
        case SystemConfigurationOptions:               return "System Configuration Options";
        case BIOSLanguage:                             return "BIOS Language Information";
        case GroupAssociations:                        return "Group Associations";
        case SystemEventLog:                           return "System Event Log";
        case PhysicalMemoryArray:                      return "Physical Memory Array";
        case MemoryDevice:                             return "Memory Device";
        case MemoryError32Bit:                         return "32-Bit Memory Error Information";
        case MemoryArrayMappedAddress:                 return "Memory Array Mapped Address";
        case MemoryDeviceMappedAddress:                return "Memory Device Mapped Address";
        case BuiltInPointingDevice:                    return "Built-in Pointing Device";
        case PortableBattery:                          return "Portable Battery";
        case SystemReset:                              return "System Reset";
        case HardwareSecurity:                         return "Hardware Security";
        case SystemPowerControls:                      return "System Power Controls";
        case VoltageProbe:                             return "Voltage Probe";
        case CoolingDevice:                            return "Cooling Device";
        case TemperatureProbe:                         return "Temperature Probe";
        case ElectricalCurrentProbe:                   return "Electrical Current Probe";
        case OutOfBandRemoteAccess:                    return "Out-of-Band Remote Access";
        case BootIntegrityServices:                    return "Boot Integrity Services Entry Point";
        case SystemBoot:                               return "System Boot Information";
        case MemoryError64Bit:                         return "64-Bit Memory Error Information";
        case ManagementDevice:                         return "Management Device";
        case ManagementDeviceComponent:                return "Management Device Component";
        case ManagementDeviceThresholdData:            return "Management Device Threshold Data";
        case MemoryChannel:                            return "Memory Channel";
        case IPMIDevice:                               return "IPMI Device Information";
        case SystemPowerSupply:                        return "System Power Supply";
        case Additional:                               return "Additional Information";
        case OnboardDevicesExtended:                   return "Onboard Devices Extended Information";
        case ManagementControllerHostInterface:        return "Management Controller Host Interface";
        case TPMDevice:                                return "TPM Device";
        case ProcessorAdditional:                      return "Processor Additional Information";
        case FirmwareInventory:                        return "Firmware Inventory Information";
        case StringProperty:                           return "String Property";
        case Inactive:                                 return "Inactive";
        case EndOfTable:                               return "End-of-Table";
        case Intel_vPro:                               return "Intel vPro";
        case Lenovo_DevicePresenceDetectionBits:       return "Lenovo Device Presence Detection bits";
        case Lenovo_ThinkPadEmbeddedControllerProgram: return "Lenovo ThinkPad Embedded Controller Program";
        case AcerHotkeyFunction:                       return "Acer Hotkey Function";
        case FSC_OMFBoardID:                           return "FSC OMF Board ID";
        case FSC_OMFBIOSID:                            return "FSC OMF BIOS ID";
        case FSC_SerialAndParallelPortConfiguration:   return "FSC Serial and Parallel Port Configuration";
        case FSC_AdvancedPowerManagementConfiguration: return "FSC Advanced Power Management Configuration";
        case FSC_DisplayIdentification:                return "FSC Display Identification";
        case FSC_BIOSErrorLog:                         return "FSC BIOS Error-Log";
        case FSC_BIOSIdentification:                   return "FSC BIOS Identification";
        case FSC_HardwareControl:                      return "FSC Hardware Control";
        case FSC_MultipurposeStaticDMIStructures:      return "FSC Multipurpose static DMI structures";
        case HP_SuperIoEnableDisableFeatures:          return "HP Super IO Enable/Disable Features";
        case HP_CPUMicrocodePatch:                     return "HP CPU Microcode Patch";
        case HP_DeviceCorrelationRecord:               return "HP Device Correlation Record";
        case HP_ProLiantSystemRackLocator:             return "HP ProLiant System/Rack Locator";
        case HP_BIOS_PXE_NIC_PCI_MAC_209:              return "HP BIOS PXE NIC PCI and MAC Information";
        case HP_64bitCRU:                              return "HP 64-bit CRU Information";
        case HP_VersionIndicatorRecord:                return "HP Version Indicator Record";
        case HP_ProLiant:                              return "HP ProLiant Information";
        case HP_BIOS_iSCSI_NIC_PCI_MAC:                return "HP BIOS iSCSI NIC PCI and MAC Information";
        case HP_TrustedModuleStatus:                   return "HP Trusted Module (TPM or TCM) Status";
        case HP_PowerSupply:                           return "HP Power Supply Information";
        case HP_BIOS_PXE_NIC_PCI_MAC_233:              return "HP BIOS PXE NIC PCI and MAC Information";
        case HPE_ProLiantHDDBackplane:                 return "HPE ProLiant HDD Backplane";
        case HPE_DIMMVendorPartNumber:                 return "HPE DIMM Vendor Part Number Information";
        case HPE_USBPortConnectorCorrelationRecord:    return "HPE USB Port Connector Correlation Record";
        case HPE_ProliantInventoryRecord:              return "HPE Proliant Inventory Record";
        case HPE_HardDriveInventoryRecord:             return "HPE Hard Drive Inventory Record";
        case DELL_RevisionsAndIDs:                     return "DELL Revisions and IDs";
        // case DELL_ParallelPort:                        return "DELL Parallel Port";
        case DELL_SerialPort:                          return "DELL Serial Port";
        case DELL_IRPort:                              return "DELL IR Port";
        default:                                       return "Unknown";
        }
    }
} // namespace probe
