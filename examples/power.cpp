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
    setvbuf(stdout, nullptr, _IOFBF, 1000);
#endif

    auto supplies = probe::power::status();

    std::cout << "Power Supply:\n";
    for (const auto& supply : supplies) {
        std::cout << "    Name             : " << supply.name << '\n'
                  << "    Type             : " << probe::to_string(supply.type) << '\n'
                  << "    Status           : " << probe::to_string(supply.status) << "\n\n";
    }

    return 0;
}