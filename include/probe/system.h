#ifndef PROBE_SYSTEM_H
#define PROBE_SYSTEM_H

#include "probe/dllport.h"
#include "probe/types.h"

#include <optional>
#include <string>

namespace probe::system
{
    enum class theme_t
    {
        dark,
        light // default mode
    };

    struct os_info_t
    {
        std::string name;
        theme_t theme;

        version_t version;
    };

    struct kernel_info_t
    {
        std::string name;
        version_t version;
    };

    PROBE_API theme_t theme();

    PROBE_API std::string os_name();
    PROBE_API std::string kernel_name();

    PROBE_API version_t os_version();
    PROBE_API version_t kernel_version();

    PROBE_API os_info_t os_info();
    PROBE_API kernel_info_t kernel_info();

    // hostname
    PROBE_API std::string hostname();

    // current username
    PROBE_API std::string username();
} // namespace probe::system

// desktop environment
namespace probe::system
{
    enum class desktop_t
    {
        Unknown,
        Windows,  // Windows
        KDE,      // K Desktop Environment, based on Qt
        GNOME,    // GNU Network Object Model Environment
        Unity,    // based on GNOME
        MATE,     // forked from GNOME 2
        Cinnamon, // forked from GNOME 3
        Xfce,
        DeepinDE, // based on Qt
        Enlightenment,
        LXQT,
        Lumina,
    };

    PROBE_API desktop_t desktop();
    PROBE_API version_t desktop_version();
} // namespace probe::system

// window system
namespace probe::system
{
    enum class window_system_t
    {
        Unknown = 0x00,
        Windows = 0x01,
        X11     = 0x02,
        Wayland = 0x04,
    };

    PROBE_API window_system_t window_system();
} // namespace probe::system

namespace probe
{
    PROBE_API std::string to_string(system::theme_t);
    PROBE_API std::string to_string(system::desktop_t);
    PROBE_API std::string to_string(system::window_system_t);
} // namespace probe

#endif //! PROBE_SYSTEM_H