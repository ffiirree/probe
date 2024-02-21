#include "probe/system.h"

#include "probe/memory.h"
#include "probe/util.h"

#include <iostream>

int main()
{
    const auto mem = probe::memory::status();

    std::cout << "Operating System:\n"
              << "    System Name      : " << probe::system::name() << '\n'
              << "    System Version   : " << probe::to_string(probe::system::version()) << '\n'
              << "    Kernel Name      : " << probe::system::kernel::name() << '\n'
              << "    Kernel Version   : " << probe::to_string(probe::system::kernel::version()) << '\n'
              << "    Device Name      : " << probe::system::hostname() << '\n'
              << "    Theme            : " << probe::to_string(probe::system::theme()) << '\n'
              << "    Desktop ENV      : " << probe::to_string(probe::system::desktop_environment()) << " ("
              << probe::to_string(probe::system::desktop_environment_version()) << ")\n"
              << "    Windowing System : " << probe::to_string(probe::system::windowing_system()) << '\n'
              << "    Memory           : " << probe::util::GB(mem.avail) << " / "
              << probe::util::GB(mem.total) << " GB\n";
    return 0;
}