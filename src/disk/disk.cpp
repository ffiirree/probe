#include "probe/disk.h"

namespace probe
{
    std::string to_string(disk::partition_style_t style)
    {
        switch(style) {
        case disk::partition_style_t::MBR: return "MBR";
        case disk::partition_style_t::GPT: return "GPT";
        case disk::partition_style_t::RAW: return "RAW";
        default: return "unknown";
        }
    }

    std::string to_string(disk::bus_type_t bus)
    {
        // clang-format off
        switch(bus) {
        case disk::bus_type_t::SCSI:        return "SCSI";
        case disk::bus_type_t::ATAPI:       return "ATAPI";
        case disk::bus_type_t::ATA:         return "ATA";
        case disk::bus_type_t::IEEE1394:    return "1394";
        case disk::bus_type_t::SSA:         return "SSA";
        case disk::bus_type_t::Fibre:       return "Fibre";
        case disk::bus_type_t::USB:         return "USB";
        case disk::bus_type_t::RAID:        return "RAID";
        case disk::bus_type_t::iSCSI:       return "iSCSI";
        case disk::bus_type_t::SAS:         return "SAS";
        case disk::bus_type_t::SATA:        return "SATA";
        case disk::bus_type_t::SD:          return "SD";
        case disk::bus_type_t::MMC:         return "MMC";
        case disk::bus_type_t::Virtual:     return "Virtual";
        case disk::bus_type_t::FileBackedVirtual: return "FileBackedVirtual";
        case disk::bus_type_t::Spaces:      return "Spaces";
        case disk::bus_type_t::NVMe:        return "NVMe";
        case disk::bus_type_t::SCM:         return "SCM";
        case disk::bus_type_t::UFS:         return "UFS";
        case disk::bus_type_t::MAX:         return "MAX";
        default: return "unknown";
        }
        // clang-format on
    }
} // namespace probe