#include "probe/types.h"

namespace probe
{
    static uint64_t numeric_combined(uint32_t h, uint32_t l) { return static_cast<uint64_t>(h) << 32 | l; }

    bool operator>=(const version_t& l, const version_t& r)
    {
        if(numeric_combined(l.major, l.minor) > numeric_combined(r.major, r.minor)) return true;

        if(numeric_combined(l.major, l.minor) < numeric_combined(r.major, r.minor)) return false;

        return numeric_combined(l.patch, l.build) >= numeric_combined(r.patch, r.build);
    }

    bool operator<=(const version_t& l, const version_t& r)
    {
        if(numeric_combined(l.major, l.minor) > numeric_combined(r.major, r.minor)) return false;

        if(numeric_combined(l.major, l.minor) < numeric_combined(r.major, r.minor)) return true;

        return numeric_combined(l.patch, l.build) <= numeric_combined(r.patch, r.build);
    }

    bool operator==(const version_t& l, const version_t& r)
    {
        return l.major == r.major && l.minor == r.minor && l.patch == r.patch && l.build == r.build;
    }

    template<> vendor_t vendor_cast(uint32_t id)
    {
        switch(id) {
        case static_cast<uint32_t>(vendor_t::NVIDIA):
        case static_cast<uint32_t>(vendor_t::Intel):
        case static_cast<uint32_t>(vendor_t::Microsoft):
        case static_cast<uint32_t>(vendor_t::Qualcomm):
        case static_cast<uint32_t>(vendor_t::AMD):
        case static_cast<uint32_t>(vendor_t::Apple): return static_cast<vendor_t>(id);

        default: return vendor_t::unknown;
        }
    }

    template<> std::string vendor_cast(vendor_t vendor)
    {
        switch(vendor) {
        case vendor_t::NVIDIA: return "NVIDIA Corporation";
        case vendor_t::Intel: return "Intel Corporation";
        case vendor_t::Microsoft: return "Microsoft Corporation";
        case vendor_t::Qualcomm: return "Qualcomm Technologies";
        case vendor_t::AMD: return "Advanced Micro Devices, Inc.";
        case vendor_t::Apple: return "Apple Inc.";
        case vendor_t::unknown:
        default: return "unknown";
        }
    }

    template<> vendor_t vendor_cast(std::string_view name)
    {
        if(name.find("NVIDIA") != std::string::npos) return vendor_t::NVIDIA;
        if(name.find("Intel") != std::string::npos) return vendor_t::Intel;
        if(name.find("Microsoft") != std::string::npos) return vendor_t::Microsoft;
        if(name.find("Qualcomm") != std::string::npos) return vendor_t::Qualcomm;
        if(name.find("AMD") != std::string::npos ||
           name.find("Advanced Micro Devices") != std::string::npos)
            return vendor_t::AMD;
        if(name.find("Apple") != std::string::npos) return vendor_t::Apple;

        return vendor_t::unknown;
    }

    std::string to_string(version_t ver)
    {

        std::string vstr = std::to_string(ver.major) + "." + std::to_string(ver.minor) + "." +
                           std::to_string(ver.patch) + "." + std::to_string(ver.build);
        if(!ver.codename.empty()) {
            vstr += "(" + ver.codename + ")";
        }
        return vstr;
    }
} // namespace probe