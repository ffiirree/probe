#include "probe/system.h"

#include <iostream>

int main()
{
    auto os     = probe::system::os_info();
    auto kernel = probe::system::kernel_info();

    std::cout << "Operating System:\n"
              << "    Name             : " << os.name << '\n'
              << "    Version          : " << probe::to_string(os.version) << '\n'
              << "    Kernel           : " << kernel.name << '\n'
              << "    Kernel Version   : " << probe::to_string(kernel.version) << '\n'
              << "    Theme            : " << probe::to_string(os.theme) << '\n'
              << "    Desktop ENV      : " << probe::to_string(probe::system::desktop()) << '\n';

    return 0;
}