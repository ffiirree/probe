#ifndef PROBE_GRAPHICS_H
#define PROBE_GRAPHICS_H

#include "probe/dllport.h"
#include "probe/types.h"

#include <cstdint>
#include <deque>
#include <string>
#include <vector>

// displays
namespace probe::graphics
{
    struct geometry_t
    {
        int32_t x;
        int32_t y;
        uint32_t width;
        uint32_t height;

        [[nodiscard]] bool contains(int32_t, int32_t) const;
        [[nodiscard]] geometry_t intersected(const geometry_t&) const;

        [[nodiscard]] int32_t left() const { return x; }
        [[nodiscard]] int32_t top() const { return y; }
        [[nodiscard]] int32_t right() const { return x + static_cast<int32_t>(width) - 1; }
        [[nodiscard]] int32_t bottom() const { return y + static_cast<int32_t>(height) - 1; }
    };

    enum class orientation_t
    {
        landscape = 0x01,
        portrait  = 0x02,

        rotate_0   = landscape,
        rotate_90  = portrait,
        rotate_180 = 0x04,
        rotate_270 = 0x08,
#ifdef _WIN32
        landscape_flipped = rotate_180,
        portrait_flipped  = rotate_270
#elif __linux__
        flipped = 0x10,

        landscape_flipped = flipped,
        portrait_flipped  = rotate_90 | flipped
#endif
    };

    struct display_t
    {
        std::string name;
        std::string id;

        geometry_t geometry;
        double frequency; // Hz

        uint32_t bpp;
        uint32_t dpi{ 96 }; // logical dot per inch
        orientation_t orientation{ orientation_t::landscape };
        bool primary{ false };
        double scale{ 1.0 };
    };

    PROBE_API std::vector<display_t> displays();
} // namespace probe::graphics

// windows
namespace probe::graphics
{
    struct window_t
    {
        std::string name; // utf-8
        std::string classname;

        geometry_t rect;
        uint64_t handle;
        bool visible;
    };
    PROBE_API std::deque<window_t> windows(bool = true);

    PROBE_API display_t virtual_screen();

    PROBE_API geometry_t virtual_screen_geometry();
} // namespace probe::graphics

// graphics processing units
namespace probe::graphics
{
    struct gpu_info_t
    {
        std::string name;

        vendor_t vendor;

        size_t dedicated_memory; // B
        size_t shared_memory;    // B
        size_t frequency;
    };

    PROBE_API std::vector<gpu_info_t> info();
} // namespace probe::graphics

namespace probe
{
    PROBE_API std::string to_string(graphics::geometry_t);
    PROBE_API std::string to_string(graphics::orientation_t);
} // namespace probe

#endif //! PROBE_GRAPHICS_H