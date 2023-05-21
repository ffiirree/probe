#include "probe/power.h"

#include "probe/time.h"
#include "probe/util.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#endif

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IOFBF, 1'000);
#endif

    auto supplies = probe::power::supplies();

    std::cout << "Power Supply:\n";
    for (const auto& supply : supplies) {
        std::cout << "  " << supply.name << '\n'
                  << "    Type                  : " << probe::to_string(supply.type) << '\n'
                  << "    Status                : " << probe::to_string(supply.status) << '\n';

        switch (supply.type) {
        case probe::power::supply_type_t::Mains: break;
        case probe::power::supply_type_t::Battery:
            std::cout << "    Serial Number         : " << supply.serial << '\n'
                      << "    Manufacturer          : " << supply.manufacturer << '\n'
                      << "    Chemistry             : " << supply.chemistry << '\n'
                      << "    Designed Capacity     : " << supply.capacity << '\n'
                      << "    Full Charged Capacity : " << supply.full_charged_capacity << '\n'
                      << "    Cycle Count           : " << supply.cycle << '\n';
            break;
        default: break;
        }

        std::cout << "\n";
    }

    return 0;
}