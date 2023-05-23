#include "probe/system.h"

#include "probe/memory.h"
#include "probe/util.h"

#include <iostream>

int main()
{
    auto os     = probe::system::os_info();
    auto kernel = probe::system::kernel_info();
    auto mem    = probe::memory::status();

    std::cout << "Operating System:\n"
              << "    Name             : " << os.name << '\n'
              << "    Version          : " << probe::to_string(os.version) << '\n'
              << "    Kernel           : " << kernel.name << '\n'
              << "    Kernel Version   : " << probe::to_string(kernel.version) << '\n'
              << "    Host Name        : " << probe::system::hostname() << '\n'
              << "    User Name        : " << probe::system::username() << '\n'
              << "    Theme            : " << probe::to_string(os.theme) << '\n'
              << "    Desktop ENV      : " << probe::to_string(probe::system::desktop()) << " ("
              << probe::to_string(probe::system::desktop_version()) << ")\n"
              << "    Window System    : " << probe::to_string(probe::system::window_system()) << '\n'
              << "    Memory           : " << probe::util::GB(mem.avail) << " / "
              << probe::util::GB(mem.total) << " GB\n";
    return 0;
}