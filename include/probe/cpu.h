#ifndef PROBE_CPU_H
#define PROBE_CPU_H

#include "probe/dllport.h"

#include <cstdint>
#include <string>

namespace probe::cpu
{
    enum class endianness_t
    {
        little,
        big
    };

    enum class architecture_t
    {
        unknown,
        x86,
        i386 = x86,
        ia64,
        itanium = ia64,
        x64,
        amd64  = x64,
        x86_64 = x64,
        arm,
        arm64
    };

    struct quantities_t
    {
        // Hyperthreads / Logical processors.
        uint32_t logical;
        // Physical "Cores".
        uint32_t physical;
        // Physical CPU units/packages/sockets.
        uint32_t packages;
    };

    struct cpu_info_t
    {
        std::string name;
        std::string vendor;
        architecture_t arch;

        endianness_t endianness;
        // base speed
        uint64_t frequency;
        quantities_t quantities;
    };

    PROBE_API architecture_t architecture();

    PROBE_API endianness_t endianness();

    PROBE_API quantities_t quantities();

    PROBE_API uint64_t frequency();

    PROBE_API std::string vendor();
    PROBE_API std::string name();

    PROBE_API cpu_info_t info();
} // namespace probe::cpu

namespace probe
{
    PROBE_API std::string to_string(cpu::architecture_t);
    PROBE_API std::string to_string(cpu::endianness_t);
} // namespace probe

#endif //! PROBE_CPU_H