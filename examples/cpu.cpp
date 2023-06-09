#include "probe/cpu.h"

#include "probe/util.h"

#include <iomanip>
#include <iostream>

int main()
{
    auto info = probe::cpu::info();

    // info
    std::cout << "CPU Information: \n"
              << "    Name             : " << info.name << '\n'
              << "    Vendor           : " << probe::to_string(info.vendor) << '\n'
              << "    Architecture     : " << probe::to_string(info.arch) << '\n'
              << "    Endianness       : " << probe::to_string(info.endianness) << '\n'
              << "    Frequency        : " << info.frequency / 1000000000.0 << " GHz\n"
              << "    Sockets          : " << info.quantities.packages << '\n'
              << "    Cores            : " << info.quantities.physical << '\n'
              << "    Logical CPUs     : " << info.quantities.logical << '\n'
              << "    Instruction Sets : ";

    for (const auto& i : probe::cpu::instruction_sets()) {
        std::cout << probe::to_string(i) << " ";
    }
    std::cout << "\n\n";

    // caches
    std::cout << "CPU Caches: \n";
    auto l1_data = probe::cpu::cache(1, probe::cpu::cache_type_t::data);
    auto l1_inst = probe::cpu::cache(1, probe::cpu::cache_type_t::instruction);
    auto l2      = probe::cpu::cache(2, probe::cpu::cache_type_t::unified);
    auto l3      = probe::cpu::cache(3, probe::cpu::cache_type_t::unified);

    // ATTENTION: cache size may be not the same at each level
    if (!l1_data.empty())
        std::cout << "    L1 Data          : " << std::setw(2) << l1_data.size() << " x " << std::setw(4)
                  << probe::util::KB(l1_data[0].size) << " KB\n";

    if (!l1_inst.empty())
        std::cout << "    L1 Inst.         : " << std::setw(2) << l1_inst.size() << " x " << std::setw(4)
                  << probe::util::KB(l1_inst[0].size) << " KB\n";

    if (!l2.empty())
        std::cout << "    L2               : " << std::setw(2) << l2.size() << " x " << std::setw(4)
                  << probe::util::KB(l2[0].size) << " KB\n";

    if (!l3.empty())
        std::cout << "    L3               : " << std::setw(2) << l3.size() << " x " << std::setw(4)
                  << probe::util::MB(l3[0].size) << " MB\n";

    return 0;
}