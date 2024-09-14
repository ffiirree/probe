#include "probe/serial-port.h"

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

    auto ports = probe::port::ports();

    std::cout << "Serial Ports: \n";
    for (const auto& port : ports) {
        std::cout << "  " << port.device << ":\n"
                  << "    Name                  : " << port.name << "\n"
                  << "    Friendly Name         : " << port.friendly_name << "\n"
                  << "    Device Instance ID    : " << port.instance_id << "\n"
                  << "    Description           : " << port.description << "\n"
                  << "    Manufacturer          : " << port.manufacturer << "\n"
                  << "    Driver                : " << port.driver << "\n\n";
    }

    return 0;
}