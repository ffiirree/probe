#ifndef PROBE_SYSTEM_H
#define PROBE_SYSTEM_H

#include "probe/dllport.h"
#include "probe/types.h"

#include <string>

namespace probe::system
{
    enum class theme_t
    {
        dark,
        light // default mode
    };

    PROBE_API std::string name();
    PROBE_API theme_t     theme();
    PROBE_API version_t   version();

    // hostname
    PROBE_API std::string hostname();

    // current username
    PROBE_API std::string username();

    namespace kernel
    {
        PROBE_API std::string name();
        PROBE_API version_t   version();
    } // namespace kernel
} // namespace probe::system

// desktop environment
namespace probe::system
{
    enum class desktop_environment_t
    {
        Unknown,
        WindowsShell, // Windows
        KDE,          // K Desktop Environment, based on Qt
        GNOME,        // GNU Network Object Model Environment
        Unity,        // based on GNOME
        MATE,         // forked from GNOME 2
        Cinnamon,     // forked from GNOME 3
        Xfce,
        DeepinDE,     // based on Qt
        Enlightenment,
        LXQT,
        Lumina,
    };

    PROBE_API desktop_environment_t desktop_environment();

    PROBE_API version_t desktop_environment_version();
} // namespace probe::system

// window system
namespace probe::system
{
    enum class windowing_system_t
    {
        Unknown              = 0x00,
        DesktopWindowManager = 0x01, // Desktop Window Manager (DWM) in Microsoft Windows (Vista and later)
        X11                  = 0x02, // X Window System, X11, X
        Wayland              = 0x04,
    };

    PROBE_API windowing_system_t windowing_system();
} // namespace probe::system

namespace probe
{
    PROBE_API std::string to_string(system::theme_t);
    PROBE_API std::string to_string(system::desktop_environment_t);
    PROBE_API std::string to_string(system::windowing_system_t);
} // namespace probe

#endif //! PROBE_SYSTEM_H