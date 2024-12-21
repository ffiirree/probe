#include "probe/types.h"

#include "probe/pciids.h"

#include <regex>

namespace probe
{
    version_t to_version(const std::string& str)
    {
        if (std::regex_match(str, std::regex("\\d+"))) {
            return { static_cast<uint32_t>(std::stoul(str)) };
        }

        std::smatch matches;
        if (std::regex_match(str, matches, std::regex(version_regex))) {
            return {
                matches[1].str().empty() ? 0 : static_cast<uint32_t>(std::stoul(matches[1].str())),
                matches[2].str().empty() ? 0 : static_cast<uint32_t>(std::stoul(matches[2].str())),
                matches[3].str().empty() ? 0 : static_cast<uint32_t>(std::stoul(matches[3].str())),
                matches[4].str().empty() ? 0 : static_cast<uint32_t>(std::stoul(matches[4].str())),
                matches[5],
            };
        }
        return {};
    }

    static uint64_t numeric_combined(uint32_t h, uint32_t l) { return static_cast<uint64_t>(h) << 32 | l; }

    bool operator>(const version_t& l, const version_t& r) { return !(l <= r); }

    bool operator>=(const version_t& l, const version_t& r)
    {
        if (numeric_combined(l.major, l.minor) > numeric_combined(r.major, r.minor)) return true;

        if (numeric_combined(l.major, l.minor) < numeric_combined(r.major, r.minor)) return false;

        return numeric_combined(l.patch, l.build) >= numeric_combined(r.patch, r.build);
    }

    bool operator<(const version_t& l, const version_t& r) { return !(l >= r); }

    bool operator<=(const version_t& l, const version_t& r)
    {
        if (numeric_combined(l.major, l.minor) > numeric_combined(r.major, r.minor)) return false;

        if (numeric_combined(l.major, l.minor) < numeric_combined(r.major, r.minor)) return true;

        return numeric_combined(l.patch, l.build) <= numeric_combined(r.patch, r.build);
    }

    bool operator==(const version_t& l, const version_t& r)
    {
        return l.major == r.major && l.minor == r.minor && l.patch == r.patch && l.build == r.build;
    }

    bool operator!=(const version_t& l, const version_t& r) { return !(l == r); }

    bool strict_equal(const version_t& l, const version_t& r) { return l == r && l.codename == r.codename; }

    std::string vendor_cast(vendor_t vendor)
    {
        // clang-format off
        switch (static_cast<uint32_t>(vendor)) {
#define V(ID, STR) case ID: return STR;
        PCIIDS_VENDORS
#undef V
        default: return "Unknown";
        }
        // clang-format on
    }

    vendor_t vendor_cast(const std::string& name)
    {
        if (std::regex_search(name, std::regex("\\bGenuineIntel\\b", std::regex_constants::icase)))
            return vendor_t::Intel;

#define V(ID, STR)                                                                                         \
    if (std::regex_match(STR, std::regex(name, std::regex_constants::icase)))                              \
        return static_cast<vendor_t>(ID);
        PCIIDS_VENDORS
#undef V

        return vendor_t::Unknown;
    }

    std::string product_name(uint32_t vid, uint32_t pid)
    {
        uint32_t key = (vid << 16) | pid;
        // clang-format off
        switch (key) {
#define P(ID, STR) case ID: return STR;
            PCIIDS_DEVICES
#undef P
        default: return {};
        }
        // clang-format on
    }

    std::string to_string(const version_t& ver)
    {
        std::string vstr =
            std::to_string(ver.major) + '.' + std::to_string(ver.minor) + '.' + std::to_string(ver.patch);
        if (ver.build) vstr += '-' + std::to_string(ver.build);

        if (!ver.codename.empty()) {
            vstr += " (" + ver.codename + ")";
        }
        return vstr;
    }

    bus_type_t bus_cast(const std::string& str)
    {
#define SEARCH_IF(REGEX, RES)                                                                              \
    if (std::regex_search(                                                                                 \
            str, std::regex(REGEX, std::regex_constants::ECMAScript | std::regex_constants::icase)))       \
    return RES

        SEARCH_IF("\\bAC97\\b", bus_type_t::AC97);
        SEARCH_IF("\\bACPI\\b|Advanced[ _-]Configuration[ _-]and[ _-]Power[ _-]Interface", bus_type_t::ACPI);
        SEARCH_IF("\\bAuxiliary\\b", bus_type_t::Auxiliary);
        SEARCH_IF("\\bATA\\b|Advanced[ _-]?Technology[ _-]?Attachment", bus_type_t::ATA);
        SEARCH_IF("\\bCAN\\b|Controller[ _-]?Area[ _-]?Network", bus_type_t::CAN);
        SEARCH_IF("\\bCEC\\b|Consumer[ _-]?Electronics[ _-]?Control", bus_type_t::CEC);
        SEARCH_IF("\\bCPU\\b", bus_type_t::CPU);
        SEARCH_IF("\\bEISA\\b|Extended[ _-]?ISA|Extended[ _-]?Industry[ _-]?Standard[ _-]?Architecture",
                  bus_type_t::EISA);
        SEARCH_IF("\\bFibre\\b", bus_type_t::Fibre);
        SEARCH_IF("File[ _-]?Backed[ _-]?Virtual", bus_type_t::FileBackedVirtual);
        SEARCH_IF("\\bGPIO\\b|General[ _-]?Purpose[ _-]?Input/Output", bus_type_t::GPIO);
        SEARCH_IF("HD[ _-]?Audio", bus_type_t::HDAudio);
        SEARCH_IF("\\bID\\b|Human[ _-]?Interface[ _-]?Device", bus_type_t::HID);
        SEARCH_IF("IEEE[ _-]?1394", bus_type_t::IEEE1394);
        SEARCH_IF("\\bI2C\\b|IIC|Inter[ _-]?Integrated[ _-]?Circuit", bus_type_t::I2C);
        SEARCH_IF("\\bIDE\\b", bus_type_t::IDE);
        SEARCH_IF("\\bISA\\b|Industry[ _-]?Standard[ _-]?Architecture", bus_type_t::ISA);
        SEARCH_IF("\\biSCSI\\b|Internet[ _-]?Small[ _-]?Computer[ _-]?Systems[ _-]?Interface", bus_type_t::iSCSI);
        SEARCH_IF("\\bMAX\\b", bus_type_t::MAX);
        SEARCH_IF("\\bMDIO\\b|Management[ _-]?Data[ _-]?Input/Output", bus_type_t::MDIO); // also known as SMI/MIIM
        SEARCH_IF("\\bSMI\\b|Serial[ _-]?Management[ _-]?Interface", bus_type_t::MDIO);
        SEARCH_IF("\\bMIIM\\b|Media[ _-]?Independent[ _-]?Interface Management", bus_type_t::MDIO);
        SEARCH_IF("\\bMMC\\b|Multi[ _-]Media[ _-]Card", bus_type_t::MMC);
        SEARCH_IF("\\bNVMEM\\b", bus_type_t::NVMEM);
        SEARCH_IF("\\bNVMe\\b|NVMHCIS|NVM[ _-]?Express", bus_type_t::NVMe);
        SEARCH_IF("\\bPCI\\b|Peripheral[ _-]?Component[ _-]?Interconnect", bus_type_t::PCI);
        SEARCH_IF(
            "\\bPCI\\b[ _-]?e|PCI[ _-]?Express|Peripheral[ _-]?Component[ _-]?Interconnect[ _-]?Express",
            bus_type_t::PCIe);
        SEARCH_IF("\\bPnP\\b|Plug[ _-]?and[ _-]?Play", bus_type_t::PnP);
        SEARCH_IF("\\bRAID\\b", bus_type_t::RAID);
        SEARCH_IF("\\bSAS\\b|Serial[ _-]?Attached[ _-]?SCSI", bus_type_t::SAS);
        SEARCH_IF("\\bSATA\\b|Serial[ _-]?ATA|Serial[ _-]?AT[ _-]?Attachment", bus_type_t::SATA);
        SEARCH_IF("\\bSCM\\b", bus_type_t::SCM);
        SEARCH_IF("\\bSCSI\\b", bus_type_t::SCSI);
        SEARCH_IF("\\bSDIO\\b|\\bSD\\b", bus_type_t::SDIO);
        SEARCH_IF("\\bSpaces\\b", bus_type_t::Spaces);
        SEARCH_IF("\\bSPI\\b|Serial[ _-]Peripheral[ _-]Interface", bus_type_t::SPI);
        SEARCH_IF("\\bSSA\\b|Serial[ _-]Storage[ _-]Architecture", bus_type_t::SSA);
        SEARCH_IF("\\bUFS\\b|Universal[ _-]Flash[ _-]Storage", bus_type_t::UFS);
        SEARCH_IF("\\bUSB\\b|Universal[ _-]Serial[ _-]Bus", bus_type_t::USB);
        SEARCH_IF("\\bVirtio\\b", bus_type_t::Virtio);
        SEARCH_IF("\\bVirtual\\b", bus_type_t::Virtual);
        SEARCH_IF("\\bVME\\b|Versa[ _-]Module[ _-]Eurocard", bus_type_t::VME);
        SEARCH_IF("\\bXen\\b", bus_type_t::Xen);
#undef SEARCH_IF
        return bus_type_t::Unknown;
    }

    std::string bus_cast(bus_type_t bus)
    {
        switch (bus) {
        case bus_type_t::AC97:              return "AC97";
        case bus_type_t::ACPI:              return "ACPI";
        case bus_type_t::Auxiliary:         return "Auxiliary";
        case bus_type_t::ATA:               return "ATA";
        case bus_type_t::ATAPI:             return "ATAPI";
        case bus_type_t::CAN:               return "CAN";
        case bus_type_t::CEC:               return "CEC";
        case bus_type_t::CPU:               return "CPU";
        case bus_type_t::EISA:              return "EISA";
        case bus_type_t::Fibre:             return "Fibre";
        case bus_type_t::FileBackedVirtual: return "FileBackedVirtual";
        case bus_type_t::GPIO:              return "GPIO";
        case bus_type_t::HDAudio:           return "HDAudio";
        case bus_type_t::HID:               return "HID";
        case bus_type_t::IEEE1394:          return "1394";
        case bus_type_t::I2C:               return "I2C";
        case bus_type_t::IDE:               return "IDE";
        case bus_type_t::ISA:               return "ISA";
        case bus_type_t::iSCSI:             return "iSCSI";
        case bus_type_t::MAX:               return "MAX";
        case bus_type_t::MDIO:              return "MDIO";
        case bus_type_t::MMC:               return "MMC";
        case bus_type_t::NVMe:              return "NVMe";
        case bus_type_t::NVMEM:             return "NVMEM";
        case bus_type_t::PCI:               return "PCI";
        case bus_type_t::PCIe:              return "PCIe";
        case bus_type_t::PnP:               return "PnP";
        case bus_type_t::RAID:              return "RAID";
        case bus_type_t::SAS:               return "SAS";
        case bus_type_t::SATA:              return "SATA";
        case bus_type_t::SCM:               return "SCM";
        case bus_type_t::SCSI:              return "SCSI";
        case bus_type_t::SDIO:              return "SDIO";
        case bus_type_t::Spaces:            return "Spaces";
        case bus_type_t::SPI:               return "SPI";
        case bus_type_t::SSA:               return "SSA";
        case bus_type_t::UFS:               return "UFS";
        case bus_type_t::USB:               return "USB";
        case bus_type_t::Virtio:            return "Virtio";
        case bus_type_t::Virtual:           return "Virtual";
        case bus_type_t::VME:               return "VME";
        case bus_type_t::Xen:               return "Xen";

        default:                return "Unknown";
        }
    }
} // namespace probe
