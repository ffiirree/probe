#include "probe/util.h"

#include "probe/system.h"

#include <iostream>

int main()
{
#ifdef _WIN32
    // example for registry::read
    if(probe::system::os_version() >= probe::WIN_10_1ST) {
        if(probe::util::registry::read<DWORD>(
               HKEY_CURRENT_USER, R"(Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)",
               "AppsUseLightTheme")
               .value_or(1) == 0) {
            std::cout << "current mode: dark\n";
        }
        else {
            std::cout << "current mode: light mode\n";
        }
    }
    // clang-format off
    // listen the change event of the dark/light mode
    probe::util::registry::RegistryListener listener;
    if(listener.listen(
        std::pair<HKEY, std::string>{
            HKEY_CURRENT_USER, 
            R"(Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)" 
        },
        [](auto) { 
            std::cout << " -- [" << probe::util::thread_get_name() << "] mode changed\n"; 
        }
    ) >= 0) {
        std::cout << "\nlistening the dark/light mode change event..\n";
        for(;;) {}
    }
    // clang-format on
#endif
    return 0;
}