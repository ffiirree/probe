#if defined(__linux__) && !defined(PROBE_WITH_X11) && !defined(PROBE_WITH_WAYLAND)

#include "probe/graphics.h"

namespace probe::graphics
{
    std::vector<display_t> displays() { return {}; }

    std::deque<window_t> windows(window_filter_t flags) { return {}; }

    std::optional<window_t> active_window() { return std::nullopt; }

    window_t virtual_screen()
    {
        return window_t{
            .name      = "~VIRTUAL-SCREEN",
            .classname = "",
            .geometry  = virtual_screen_geometry(),
            .visible   = true,
        };
    }
} // namespace probe::graphics

#endif // __linux__
