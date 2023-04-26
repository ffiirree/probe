#include "probe/cpu.h"

#include <iostream>

int main()
{
    auto info = probe::cpu::info();

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

    for(const auto& i : probe::cpu::instruction_sets()) {
        std::cout << probe::to_string(i) << " ";
    }
    std::cout << "\n";

    return 0;
}