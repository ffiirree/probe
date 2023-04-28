#include "probe/types.h"

#include <iostream>
#include <regex>

namespace probe
{
    version_t to_version(const std::string& str)
    {
        std::cout << str << "\n";
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
        switch(vendor) {
        case vendor_t::NVIDIA:          return "NVIDIA Corporation";
        case vendor_t::Intel:           return "Intel Corporation";
        case vendor_t::Microsoft:       return "Microsoft Corporation";
        case vendor_t::Qualcomm:        return "Qualcomm Technologies";
        case vendor_t::AMD:             return "Advanced Micro Devices, Inc.";
        case vendor_t::Apple:           return "Apple Inc.";
        case vendor_t::unknown:
        default:                        return "unknown";
        }
        // clang-format on
    }

    vendor_t vendor_cast(const std::string& name)
    {
        if (std::regex_search(name, std::regex("NVIDIA", std::regex_constants::icase)))
            return vendor_t::NVIDIA;

        if (std::regex_search(name, std::regex("Intel", std::regex_constants::icase)))
            return vendor_t::Intel;

        if (std::regex_search(name, std::regex("Microsoft", std::regex_constants::icase)))
            return vendor_t::Microsoft;

        if (std::regex_search(name, std::regex("Qualcomm", std::regex_constants::icase)))
            return vendor_t::Qualcomm;

        if (std::regex_search(name, std::regex("AMD", std::regex_constants::icase)) ||
            std::regex_search(name, std::regex("Advanced Micro Devices", std::regex_constants::icase)))
            return vendor_t::AMD;
            
        if (std::regex_search(name, std::regex("Apple", std::regex_constants::icase)))
            return vendor_t::Apple;

        return vendor_t::unknown;
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
} // namespace probe