#include "probe/system.h"

namespace probe
{
    std::string to_string(system::theme_t t) { return (t == system::theme_t::dark) ? "dark" : "light"; }

    std::string to_string(system::desktop_t de)
    {
        switch(de) {
        case system::desktop_t::Windows: return "Windows";
        case system::desktop_t::KDE: return "KDE";
        case system::desktop_t::GNOME: return "GNOME";
        case system::desktop_t::Unity: return "Unity";
        case system::desktop_t::MATE: return "MATE";
        case system::desktop_t::Cinnamon: return "Cinnamon";
        case system::desktop_t::Xfce: return "Xfce";
        case system::desktop_t::DeepinDE: return "DeepinDE";
        case system::desktop_t::Enlightenment: return "Enlightenment";
        case system::desktop_t::LXQT: return "LXQT";
        case system::desktop_t::Lumina: return "Lumina";

        case system::desktop_t::unknown:
        default: return "unknown";
        }
    }
} // namespace probe