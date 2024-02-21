#include "probe/util.h"

#include "probe/system.h"
#include "probe/thread.h"
#include "probe/time.h"

#include <iostream>

int main()
{
#ifdef _WIN32
    const auto theme_key = R"(Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)";

    // example for registry::read,
    auto theme = probe::system::theme_t::light;
    if (probe::system::os_version() >= probe::WIN_10) {
        if (!probe::util::registry::read<DWORD>(HKEY_CURRENT_USER, theme_key, "AppsUseLightTheme")
                 .value_or(1)) {
            theme = probe::system::theme_t::dark;
        }
    }
    std::cout << "current mode: " << probe::to_string(theme) << std::endl;

    // listen the change event of the dark/light mode
    probe::util::registry::RegistryListener listener;
    // triggered by AppsUseLightTheme & SystemUsesLightTheme
    listener.listen(std::pair<HKEY, std::string>{ HKEY_CURRENT_USER, theme_key }, [](auto) {
        std::cout << " -- [" << probe::thread::name() << "] " << probe::to_string(probe::system::theme())
                  << std::endl;
    });

#elif defined(__linux__)

    std::cout << "gsettings contains 'org.gnome.desktop.interface' 'gtk-theme': "
              << probe::util::gsettings::contains("org.gnome.desktop.interface", "gtk-theme") << "\n";

    // listen the theme change event
    probe::util::PipeListener listener;
    listener.listen(
        std::vector<const char *>{ "gsettings", "monitor", "org.gnome.desktop.interface", "gtk-theme" },
        [=](const std::any& theme) {
            std::cout << "-- [" << probe::thread::name() << "] " << std::any_cast<std::string>(theme);
        });
#endif

    std::cout << "\nlistening the dark/light mode change event..\n";

    while (listener.running()) {
        std::this_thread::sleep_for(100ms);
    }

    return 0;
}