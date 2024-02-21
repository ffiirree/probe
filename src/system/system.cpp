#include "probe/system.h"

namespace probe
{
    std::string to_string(const system::theme_t t)
    {
        return (t == system::theme_t::dark) ? "dark" : "light";
    }

    std::string to_string(const system::desktop_environment_t de)
    {
        switch (de) {
        case system::desktop_environment_t::WindowsShell:  return "Windows Shell";
        case system::desktop_environment_t::KDE:           return "KDE";
        case system::desktop_environment_t::GNOME:         return "GNOME";
        case system::desktop_environment_t::Unity:         return "Unity";
        case system::desktop_environment_t::MATE:          return "MATE";
        case system::desktop_environment_t::Cinnamon:      return "Cinnamon";
        case system::desktop_environment_t::Xfce:          return "Xfce";
        case system::desktop_environment_t::DeepinDE:      return "DeepinDE";
        case system::desktop_environment_t::Enlightenment: return "Enlightenment";
        case system::desktop_environment_t::LXQT:          return "LXQT";
        case system::desktop_environment_t::Lumina:        return "Lumina";

        default:                                           return "Unknown";
        }
    }

    std::string to_string(const system::windowing_system_t ws)
    {
        switch (ws) {
        case system::windowing_system_t::DesktopWindowManager: return "Desktop Window Manager";
        case system::windowing_system_t::X11:                  return "X11";
        case system::windowing_system_t::Wayland:              return "Wayland";
        default:                                               return "Unknown";
        }
    }
} // namespace probe