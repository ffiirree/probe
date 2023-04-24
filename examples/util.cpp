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

    // listen the change event of the dark/light mode
    probe::util::registry::RegistryListener listener;
    listener.listen(
        std::pair<HKEY, std::string>{ HKEY_CURRENT_USER,
                                      R"(Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)" },
        [](auto) { std::cout << " -- [" << probe::util::thread_get_name() << "] mode changed\n"; });

    std::cout << "\nlistening the dark/light mode change event..\n";
    for(;listener.running();) {}

#elif defined(__linux__)

    // listen the theme change event
    probe::util::PipeListener listener;
    listener.listen(
        std::vector<const char *>{ "gsettings", "monitor", "org.gnome.desktop.interface", "gtk-theme" },
        [=](const std::any& theme) {
            std::cout << "-- [" << probe::util::thread_get_name() << "] "
                      << std::any_cast<std::string>(theme);
        });

    std::cout << "\nlistening the theme change event..\n";

    for(; listener.running();) {}

#endif
    return 0;
}