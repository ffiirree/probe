#include "probe/util.h"

#include "probe/system.h"
#include "probe/thread.h"
#include "probe/time.h"

#include <iostream>

int main()
{
#ifdef _WIN32

    // example for registry::read
    if (probe::system::os_version() >= probe::WIN_10_1ST) {
        if (probe::util::registry::read<DWORD>(
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
        [](auto) { std::cout << " -- [" << probe::thread::name() << "] mode changed\n"; });

    std::cout << "\nlistening the dark/light mode change event..\n";

#elif defined(__linux__)

    std::cout << "gsettings contains 'org.gnome.desktop.interface': "
              << probe::util::gsettings::contains("org.gnome.desktop.interface") << "\n";
    std::cout << "gsettings contains 'org.gnome.desktop.interface' 'gtk-theme': "
              << probe::util::gsettings::contains("org.gnome.desktop.interface", "gtk-theme") << "\n";

    // listen the theme change event
    probe::util::PipeListener listener;
    listener.listen(
        std::vector<const char *>{ "gsettings", "monitor", "org.gnome.desktop.interface", "gtk-theme" },
        [=](const std::any& theme) {
            std::cout << "-- [" << probe::thread::name() << "] " << std::any_cast<std::string>(theme);
        });

    std::cout << "\nlistening the theme change event..\n";
#endif

    for (; listener.running();) {
        probe::time::msleep(100);
    }

    return 0;
}