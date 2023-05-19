#include "probe/types.h"

#include "probe/pciids.h"

#include <map>
#include <regex>

namespace probe
{
    version_t to_version(const std::string& str)
    {
        std::smatch matchs;
        if (std::regex_match(str, matchs, std::regex(verion_regex))) {
            return {
                matchs[1].str().empty() ? 0 : static_cast<uint32_t>(std::stoul(matchs[1].str())),
                matchs[2].str().empty() ? 0 : static_cast<uint32_t>(std::stoul(matchs[2].str())),
                matchs[3].str().empty() ? 0 : static_cast<uint32_t>(std::stoul(matchs[3].str())),
                matchs[4].str().empty() ? 0 : static_cast<uint32_t>(std::stoul(matchs[4].str())),
            };
        }
        return {};
    }

    static uint64_t numeric_combined(uint32_t h, uint32_t l) { return static_cast<uint64_t>(h) << 32 | l; }

    bool operator>=(const version_t& l, const version_t& r)
    {
        if (numeric_combined(l.major, l.minor) > numeric_combined(r.major, r.minor)) return true;

        if (numeric_combined(l.major, l.minor) < numeric_combined(r.major, r.minor)) return false;

        return numeric_combined(l.patch, l.build) >= numeric_combined(r.patch, r.build);
    }

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

    std::string to_string(version_t ver)
    {
        std::string vstr = std::to_string(ver.major) + "." + std::to_string(ver.minor) + "." +
                           std::to_string(ver.patch) + "." + std::to_string(ver.build);
        if (!ver.codename.empty()) {
            vstr += " (" + ver.codename + ")";
        }
        return vstr;
    }

    bus_type_t bus_cast(const std::string& str)
    {
        using enum bus_type_t;
#define SEARCH_IF(REGEX, RES)                                                                              \
    if (std::regex_search(                                                                                 \
            str, std::regex(REGEX, std::regex_constants::ECMAScript | std::regex_constants::icase)))       \
    return RES

        SEARCH_IF("\\bAC97\\b", AC97);
        SEARCH_IF("\\bACPI\\b|Advanced[ _-]Configuration[ _-]and[ _-]Power[ _-]Interface", ACPI);
        SEARCH_IF("\\bAuxiliary\\b", Auxiliary);
        SEARCH_IF("\\bATA\\b|Advanced[ _-]?Technology[ _-]?Attachment", ATA);
        SEARCH_IF("\\bCAN\\b|Controller[ _-]?Area[ _-]?Network", CAN);
        SEARCH_IF("\\bCEC\\b|Consumer[ _-]?Electronics[ _-]?Control", CEC);
        SEARCH_IF("\\bCPU\\b", CPU);
        SEARCH_IF("\\bEISA\\b|Extended[ _-]?ISA|Extended[ _-]?Industry[ _-]?Standard[ _-]?Architecture",
                  EISA);
        SEARCH_IF("\\bFibre\\b", Fibre);
        SEARCH_IF("File[ _-]?Backed[ _-]?Virtual", FileBackedVirtual);
        SEARCH_IF("\\bGPIO\\b|General[ _-]?Purpose[ _-]?Input/Output", GPIO);
        SEARCH_IF("HD[ _-]?Audio", HDAudio);
        SEARCH_IF("\\bID\\b|Human[ _-]?Interface[ _-]?Device", HID);
        SEARCH_IF("IEEE[ _-]?1394", IEEE1394);
        SEARCH_IF("\\bI2C\\b|IIC|Inter[ _-]?Integrated[ _-]?Circuit", I2C);
        SEARCH_IF("\\bIDE\\b", IDE);
        SEARCH_IF("\\bISA\\b|Industry[ _-]?Standard[ _-]?Architecture", ISA);
        SEARCH_IF("\\biSCSI\\b|Internet[ _-]?Small[ _-]?Computer[ _-]?Systems[ _-]?Interface", iSCSI);
        SEARCH_IF("\\bMAX\\b", MAX);
        SEARCH_IF("\\bMDIO\\b|Management[ _-]?Data[ _-]?Input/Output", MDIO); // also known as SMI/MIIM
        SEARCH_IF("\\bSMI\\b|Serial[ _-]?Management[ _-]?Interface", MDIO);
        SEARCH_IF("\\bMIIM\\b|Media[ _-]?Independent[ _-]?Interface Management", MDIO);
        SEARCH_IF("\\bMMC\\b|Multi[ _-]Media[ _-]Card", MMC);
        SEARCH_IF("\\bNVMEM\\b", NVMEM);
        SEARCH_IF("\\bNVMe\\b|NVMHCIS|NVM[ _-]?Express", NVMe);
        SEARCH_IF("\\bPCI\\b|Peripheral[ _-]?Component[ _-]?Interconnect", PCI);
        SEARCH_IF(
            "\\bPCI\\b[ _-]?e|PCI[ _-]?Express|Peripheral[ _-]?Component[ _-]?Interconnect[ _-]?Express",
            PCIe);
        SEARCH_IF("\\bPnP\\b|Plug[ _-]?and[ _-]?Play", PnP);
        SEARCH_IF("\\bRAID\\b", RAID);
        SEARCH_IF("\\bSAS\\b|Serial[ _-]?Attached[ _-]?SCSI", SAS);
        SEARCH_IF("\\bSATA\\b|Serial[ _-]?ATA|Serial[ _-]?AT[ _-]?Attachment", SATA);
        SEARCH_IF("\\bSCM\\b", SCM);
        SEARCH_IF("\\bSDIO\\b|\\bSD\\b", SDIO);
        SEARCH_IF("\\bSpaces\\b", Spaces);
        SEARCH_IF("\\bSPI\\b|Serial[ _-]Peripheral[ _-]Interface", SPI);
        SEARCH_IF("\\bSSA\\b|Serial[ _-]Storage[ _-]Architecture", SSA);
        SEARCH_IF("\\bUFS\\b|Universal[ _-]Flash[ _-]Storage", UFS);
        SEARCH_IF("\\bUSB\\b|Universal[ _-]Serial[ _-]Bus", USB);
        SEARCH_IF("\\bVirtio\\b", Virtio);
        SEARCH_IF("\\bVirtual\\b", Virtual);
        SEARCH_IF("\\bVME\\b|Versa[ _-]Module[ _-]Eurocard", VME);
        SEARCH_IF("\\bXen\\b", Xen);
#undef SEARCH_IF
        return Unknown;
    }

    std::string bus_cast(bus_type_t bus)
    {
        using enum bus_type_t;
        // clang-format off
        switch(bus) {
        case AC97:          return "AC97";
        case ACPI:          return "ACPI";
        case Auxiliary:     return "Auxiliary";
        case ATA:           return "ATA";
        case ATAPI:         return "ATAPI";
        case CAN:           return "CAN";
        case CEC:           return "CEC";
        case CPU:           return "CPU";
        case EISA:          return "EISA";
        case Fibre:         return "Fibre";
        case FileBackedVirtual: return "FileBackedVirtual";
        case GPIO:          return "GPIO";
        case HDAudio:       return "HDAudio";
        case HID:           return "HID";
        case IEEE1394:      return "1394";
        case I2C:           return "I2C";
        case IDE:           return "IDE";
        case ISA:           return "ISA";
        case iSCSI:         return "iSCSI";
        case MAX:           return "MAX";
        case MDIO:          return "MDIO";
        case MMC:           return "MMC";
        case NVMe:          return "NVMe";
        case NVMEM:         return "NVMEM";
        case PCI:           return "PCI";
        case PCIe:          return "PCIe";
        case PnP:           return "PnP";
        case RAID:          return "RAID";
        case SAS:           return "SAS";
        case SATA:          return "SATA";
        case SCM:           return "SCM";
        case SCSI:          return "SCSI";
        case SDIO:          return "SDIO";
        case Spaces:        return "Spaces";
        case SPI:           return "SPI";
        case SSA:           return "SSA";
        case UFS:           return "UFS";
        case USB:           return "USB";
        case Virtio:        return "Virtio";
        case Virtual:       return "Virtual";
        case VME:           return "VME";
        case Xen:           return "Xen";

        default: return "Unknown";
        }
        // clang-format on
    }
} // namespace probe