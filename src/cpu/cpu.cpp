#include "probe/cpu.h"

namespace probe::cpu
{
    endianness_t endianness()
    {
        const uint16_t test = 0xFF00;
        const auto result   = *static_cast<const std::uint8_t *>(static_cast<const void *>(&test));

        if(result == 0xFF)
            return endianness_t::big;
        else
            return endianness_t::little;
    }
} // namespace probe::cpu

namespace probe
{
    std::string to_string(cpu::architecture_t arch)
    {
        switch(arch) {
        case cpu::architecture_t::x86: return "x86";
        case cpu::architecture_t::ia64: return "ia64";
        case cpu::architecture_t::x64: return "x64";
        case cpu::architecture_t::arm: return "arm";
        case cpu::architecture_t::arm64: return "arm64";
        case cpu::architecture_t::unknown:
        default: return "unknown";
        }
    }

    std::string to_string(cpu::endianness_t e)
    {
        switch(e) {
        case cpu::endianness_t::little: return "little";
        case cpu::endianness_t::big: return "big";
        default: return "unknown";
        }
    }
} // namespace probe