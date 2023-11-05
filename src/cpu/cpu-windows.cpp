#ifdef _WIN32

#include "probe/cpu.h"
#include "probe/util.h"

#include <bitset>
#include <vector>
#include <Windows.h>

namespace probe::cpu
{
    static std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> processor_info()
    {
        std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> processors;

        DWORD byte_count{};
        ::GetLogicalProcessorInformation(nullptr, &byte_count);

        processors.resize(byte_count / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
        ::GetLogicalProcessorInformation(processors.data(), &byte_count);

        return processors;
    }

    architecture_t architecture()
    {
        SYSTEM_INFO system_info;
        ::GetNativeSystemInfo(&system_info);

        switch (system_info.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64: // x64 (AMD or Intel)
            return architecture_t::x64;

        case PROCESSOR_ARCHITECTURE_ARM:   // ARM
            return architecture_t::arm;

        case PROCESSOR_ARCHITECTURE_ARM64: // ARM64
            return architecture_t::arm64;

        case PROCESSOR_ARCHITECTURE_IA64:  // Intel Itanium-based
            return architecture_t::ia64;

        case PROCESSOR_ARCHITECTURE_INTEL: // x86
            return architecture_t::x86;

        default: return architecture_t::unknown;
        }
    }

    uint64_t frequency()
    {
        return probe::util::registry::read<DWORD>(
                   HKEY_LOCAL_MACHINE, R"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)", "~MHz")
                   .value_or(0) *
               1'000'000;
    }

    quantities_t quantities()
    {
        quantities_t q = { 0 };
        for (auto&& info : processor_info()) {
            switch (info.Relationship) {
            case RelationProcessorCore:
                q.physical++;
                q.logical += static_cast<std::uint32_t>(
                    std::bitset<sizeof(ULONG_PTR) * 8>(static_cast<uintptr_t>(info.ProcessorMask)).count());
                break;

            case RelationProcessorPackage: q.packages++; break;

            default:                       break;
            }
        }

        return q;
    }

    std::vector<cache_t> caches()
    {
        std::vector<cache_t> ret;

        for (const auto& info : processor_info()) {
            if (info.Relationship == RelationCache) {
                ret.emplace_back(info.Cache.Level, info.Cache.Associativity, info.Cache.LineSize,
                                 info.Cache.Size, static_cast<cache_type_t>(info.Cache.Type));
            }
        }
        return ret;
    }

    std::vector<cache_t> cache(int level, cache_type_t type)
    {
        std::vector<cache_t> ret;

        for (const auto& info : processor_info()) {
            if ((info.Relationship == RelationCache) && (info.Cache.Level == level) &&
                (info.Cache.Type == static_cast<PROCESSOR_CACHE_TYPE>(type))) {
                ret.emplace_back(info.Cache.Level, info.Cache.Associativity, info.Cache.LineSize,
                                 info.Cache.Size, static_cast<cache_type_t>(info.Cache.Type));
            }
        }
        return ret;
    }

    vendor_t vendor()
    {
        auto name =
            probe::util::registry::read<std::string>(
                HKEY_LOCAL_MACHINE, R"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)", "VendorIdentifier")
                .value_or("");
        return vendor_cast(name);
    }

    std::string name()
    {
        return probe::util::registry::read<std::string>(HKEY_LOCAL_MACHINE,
                                                        R"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)",
                                                        "ProcessorNameString")
            .value_or("");
    }

    cpu_info_t info()
    {
        return { name(), vendor(), architecture(), endianness(), frequency(), quantities() };
    }

    // This intrinsic stores the supported features and CPU information returned by the cpuid instruction in
    // cpuInfo, an array of four 32-bit integers that's filled with the values of the EAX, EBX, ECX, and EDX
    // registers (in that order). The information returned has a different meaning depending on the value
    // passed as the function_id parameter. The information returned with various values of function_id is
    // processor-dependent.
    //
    //  When the function_id argument is 0, cpuInfo[0] returns the highest available non-extended
    //  function_id value supported by the processor. The processor manufacturer is encoded in cpuInfo[1],
    //  cpuInfo[2], and cpuInfo[3].
    //
    //  Some processors support Extended Function CPUID information. When it's supported, function_id values
    //  from 0x80000000 might be used to return information. To determine the maximum meaningful value
    //  allowed, set function_id to 0x80000000. The maximum value of function_id supported for extended
    //  functions will be written to cpuInfo[0].
    void cpuid(int32_t (&info)[4], int32_t leaf, int32_t subleaf) { __cpuidex(info, leaf, subleaf); }
} // namespace probe::cpu

#endif // _WIN32