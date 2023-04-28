#include "probe/cpu.h"

#include "probe/types.h"

#include <bitset>
#include <regex>

namespace probe::cpu
{
    endianness_t endianness()
    {
        const uint16_t test = 0xFF00;
        const auto result   = *static_cast<const std::uint8_t *>(static_cast<const void *>(&test));

        return (result == 0xFF) ? endianness_t::big : endianness_t::little;
    }

    cache_type_t to_cache_type(const std::string& str)
    {
        if (std::regex_search(str, std::regex("unified", std::regex_constants::icase)))
            return cache_type_t::unified;

        if (std::regex_search(str, std::regex("inst", std::regex_constants::icase)))
            return cache_type_t::instruction;

        if (std::regex_search(str, std::regex("data", std::regex_constants::icase)))
            return cache_type_t::data;

        if (std::regex_search(str, std::regex("trace", std::regex_constants::icase)))
            return cache_type_t::trace;

        return static_cast<cache_type_t>(static_cast<uint32_t>(cache_type_t::trace) + 1);
    }

#define FEATURE_BIT_MASK(X, MASK, SHIFT)                                                                   \
    static_cast<int32_t>((static_cast<uint64_t>(X) & static_cast<uint64_t>(feature_t::MASK)) >> SHIFT)
    std::tuple<int32_t, int32_t, int32_t, int32_t> unpack(feature_t feature)
    {
        return std::tuple{
            FEATURE_BIT_MASK(feature, leaf_mask, 32),
            FEATURE_BIT_MASK(feature, subleaf_mask, 24),
            FEATURE_BIT_MASK(feature, register_mask, 16),
            FEATURE_BIT_MASK(feature, bit_mask, 0),
        };
    }
#undef FEATURE_BIT_MASK

    bool is_supported(feature_t feature)
    {
        int32_t info[4]{};

        cpuid(info, 0, 0);

        auto [leaf, subleaf, reg, bit] = unpack(feature);

        if (info[0] >= leaf) {
            cpuid(info, leaf, subleaf);

            return (info[reg] & (0x01 << bit));
        }
        return false;
    }

    // https://en.wikipedia.org/wiki/CPUID
    // https://learn.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex?view=msvc-170
    std::vector<feature_t> instruction_sets()
    {
        int32_t info[4]{};

        // Calling __cpuid with 0x0 as the function_id argument gets the number of the highest valid
        // function ID.
        cpuid(info, 0, 0);
        int32_t ids = info[0];

        // vendor string
        char vendor_name[0x13]{};
        *reinterpret_cast<int32_t *>(vendor_name)     = info[1];
        *reinterpret_cast<int32_t *>(vendor_name + 4) = info[3];
        *reinterpret_cast<int32_t *>(vendor_name + 8) = info[2];

        std::vector<feature_t> isets{};

        vendor_t vendor = vendor_cast(vendor_name);

        if (ids >= 1) {
            cpuid(info, 1, 0);

            std::bitset<32> ecx = info[2];
            std::bitset<32> edx = info[3];

            if (ecx[0]) isets.push_back(feature_t::sse3);
            if (ecx[1]) isets.push_back(feature_t::pclmulqdq);
            if (ecx[3]) isets.push_back(feature_t::monitor);
            if (ecx[9]) isets.push_back(feature_t::ssse3);
            if (ecx[12]) isets.push_back(feature_t::fma);
            if (ecx[13]) isets.push_back(feature_t::cx16);
            if (ecx[19]) isets.push_back(feature_t::sse4_1);
            if (ecx[20]) isets.push_back(feature_t::sse4_2);
            if (ecx[22]) isets.push_back(feature_t::movbe);
            if (ecx[23]) isets.push_back(feature_t::popcnt);
            if (ecx[25]) isets.push_back(feature_t::aes);
            if (ecx[26]) isets.push_back(feature_t::xsave);
            if (ecx[27]) isets.push_back(feature_t::osxsave);
            if (ecx[28]) isets.push_back(feature_t::avx);
            if (ecx[29]) isets.push_back(feature_t::f16c);
            if (ecx[30]) isets.push_back(feature_t::rdrnd);

            if (edx[5]) isets.push_back(feature_t::msr);
            if (edx[8]) isets.push_back(feature_t::cx8);
            if (edx[11]) isets.push_back(feature_t::sep);
            if (edx[15]) isets.push_back(feature_t::cmov);
            if (edx[19]) isets.push_back(feature_t::clfsh);
            if (edx[23]) isets.push_back(feature_t::mmx);
            if (edx[24]) isets.push_back(feature_t::fxsr);
            if (edx[25]) isets.push_back(feature_t::sse);
            if (edx[26]) isets.push_back(feature_t::sse2);
        }

        if (ids >= 7) {
            cpuid(info, 7, 0);

            std::bitset<32> ebx = info[1];
            std::bitset<32> ecx = info[2];

            if (ebx[0]) isets.push_back(feature_t::fsgsbase);
            if (ebx[3]) isets.push_back(feature_t::bmi1);
            if (ebx[4] && vendor == vendor_t::Intel) isets.push_back(feature_t::hle);
            if (ebx[5]) isets.push_back(feature_t::avx2);
            if (ebx[8]) isets.push_back(feature_t::bmi2);
            if (ebx[9]) isets.push_back(feature_t::erms);
            if (ebx[10]) isets.push_back(feature_t::invpcid);
            if (ebx[11] && vendor == vendor_t::Intel) isets.push_back(feature_t::rtm);
            if (ebx[16]) isets.push_back(feature_t::avx512_f);
            if (ebx[17]) isets.push_back(feature_t::avx512_dq);
            if (ebx[18]) isets.push_back(feature_t::rdseed);
            if (ebx[19]) isets.push_back(feature_t::adx);
            if (ebx[21]) isets.push_back(feature_t::avx512_ifma);
            if (ebx[26]) isets.push_back(feature_t::avx512_pf);
            if (ebx[27]) isets.push_back(feature_t::avx512_er);
            if (ebx[28]) isets.push_back(feature_t::avx512_cd);
            if (ebx[29]) isets.push_back(feature_t::sha);
            if (ebx[30]) isets.push_back(feature_t::avx512_bw);
            if (ebx[31]) isets.push_back(feature_t::avx512_vl);

            if (ecx[0]) isets.push_back(feature_t::prefetchwt1);
            if (ecx[1]) isets.push_back(feature_t::avx512_vbmi);
            if (ecx[6]) isets.push_back(feature_t::avx512_vbmi2);
        }

        // Calling __cpuid with 0x80000000 as the function_id argument gets the number of the highest valid
        // extended ID.
        cpuid(info, 0x80000000, 0);
        int32_t exids = info[0];

        if (exids >= static_cast<int32_t>(0x80000001)) {
            cpuid(info, 0x80000001, 0);

            std::bitset<32> ecx = info[2];
            std::bitset<32> edx = info[3];

            if (ecx[5] && vendor == vendor_t::Intel) isets.push_back(feature_t::lzcnt); // intel
            if (ecx[5] && vendor == vendor_t::AMD) isets.push_back(feature_t::abm);     // amd
            if (ecx[6] && vendor == vendor_t::AMD) isets.push_back(feature_t::sse4a);
            if (ecx[11] && vendor == vendor_t::AMD) isets.push_back(feature_t::xop);
            if (ecx[21] && vendor == vendor_t::AMD) isets.push_back(feature_t::tbm);

            if (edx[11] && vendor == vendor_t::Intel) isets.push_back(feature_t::syscall);
            if (edx[22] && vendor == vendor_t::AMD) isets.push_back(feature_t::mmxext);
            if (edx[27] && vendor == vendor_t::Intel) isets.push_back(feature_t::rdtscp);
            if (edx[30] && vendor == vendor_t::AMD) isets.push_back(feature_t::amd_3dnowext);
            if (edx[31] && vendor == vendor_t::AMD) isets.push_back(feature_t::amd_3dnow);
        }

        return isets;
    }
} // namespace probe::cpu

namespace probe
{
    std::string to_string(cpu::architecture_t arch)
    {
        // clang-format off
        switch(arch) {
        case cpu::architecture_t::x86:      return "x86";
        case cpu::architecture_t::ia64:     return "ia64";
        case cpu::architecture_t::x64:      return "x64";
        case cpu::architecture_t::arm:      return "arm";
        case cpu::architecture_t::arm64:    return "arm64";
        case cpu::architecture_t::unknown:
        default:                            return "unknown";
        }
        // clang-format on
    }

    std::string to_string(cpu::endianness_t e)
    {
        // clang-format off
        switch(e) {
        case cpu::endianness_t::little:     return "little";
        case cpu::endianness_t::big:        return "big";
        default:                            return "unknown";
        }
        // clang-format on
    }

    std::string to_string(cpu::cache_type_t type)
    {
        // clang-format off
        switch(type) {
        case cpu::cache_type_t::unified:    return "Unified";
        case cpu::cache_type_t::instruction:return "Instruction";
        case cpu::cache_type_t::data:       return "Data";
        case cpu::cache_type_t::trace:      return "Trace";

        default:                            return "unknown";
        }
        // clang-format on
    }

    std::string to_string(cpu::feature_t is)
    {
        // clang-format off
        switch(is) {
        case cpu::feature_t::amd_3dnow:     return "3dnow";
        case cpu::feature_t::amd_3dnowext:  return "3dnowext";
        case cpu::feature_t::abm:           return "abm";
        case cpu::feature_t::adx:           return "adx";
        case cpu::feature_t::aes:           return "aes";
        case cpu::feature_t::avx:           return "avx";
        case cpu::feature_t::avx2:          return "avx2";
        case cpu::feature_t::avx512_cd:     return "avx512_cd";
        case cpu::feature_t::avx512_bw:     return "avx512_bw";
        case cpu::feature_t::avx512_dq:     return "avx512_dq";
        case cpu::feature_t::avx512_er:     return "avx512_er";
        case cpu::feature_t::avx512_f:      return "avx512_f";
        case cpu::feature_t::avx512_ifma:   return "avx512_ifma";
        case cpu::feature_t::avx512_pf:     return "avx512_pf";
        case cpu::feature_t::avx512_vbmi:   return "avx512_vbmi";
        case cpu::feature_t::avx512_vbmi2:  return "avx512_vbmi2";
        case cpu::feature_t::avx512_vl:     return "avx512_vl";
        case cpu::feature_t::bmi1:          return "bmi1";
        case cpu::feature_t::bmi2:          return "bmi2";
        case cpu::feature_t::clfsh:         return "clfsh";
        case cpu::feature_t::cmov:          return "cmov";
        case cpu::feature_t::cx8:           return "cx8";
        case cpu::feature_t::cx16:          return "cx16";
        case cpu::feature_t::erms:          return "erms";
        case cpu::feature_t::f16c:          return "f16c";
        case cpu::feature_t::fma:           return "fma";
        case cpu::feature_t::fma4:          return "fma4";
        case cpu::feature_t::fsgsbase:      return "fsgsbase";
        case cpu::feature_t::fxsr:          return "fxsr";
        case cpu::feature_t::hle:           return "hle";
        case cpu::feature_t::invpcid:       return "invpcid";
        // case cpu::feature_t::lzcnt:         return "lzcnt";
        case cpu::feature_t::mmx:           return "mmx";
        case cpu::feature_t::mmxext:        return "mmxext";
        case cpu::feature_t::monitor:       return "monitor";
        case cpu::feature_t::movbe:         return "movbe";
        case cpu::feature_t::msr:           return "msr";
        case cpu::feature_t::osxsave:       return "osxsave";
        case cpu::feature_t::pclmulqdq:     return "pclmulqdq";
        case cpu::feature_t::popcnt:        return "popcnt";
        case cpu::feature_t::prefetchwt1:   return "prefetchwt1";
        case cpu::feature_t::rdrnd:         return "rdrnd";
        case cpu::feature_t::rdseed:        return "rdseed";
        case cpu::feature_t::rdtscp:        return "rdtscp";
        case cpu::feature_t::rtm:           return "rtm";
        case cpu::feature_t::sep:           return "sep";
        case cpu::feature_t::sha:           return "sha";
        case cpu::feature_t::sse:           return "sse";
        case cpu::feature_t::sse2:          return "sse2";
        case cpu::feature_t::sse3:          return "sse3";
        case cpu::feature_t::ssse3:         return "ssse3";
        case cpu::feature_t::sse4_1:        return "sse4_1";
        case cpu::feature_t::sse4_2:        return "sse4_2";
        case cpu::feature_t::sse4a:         return "sse4a";
        case cpu::feature_t::syscall:       return "syscall";
        case cpu::feature_t::tbm:           return "tbm";
        case cpu::feature_t::xop:           return "xop";
        case cpu::feature_t::xsave:         return "xsave";
        case cpu::feature_t::unknown:
        default:                            return "unknown";
        }
        // clang-format on
    }
} // namespace probe