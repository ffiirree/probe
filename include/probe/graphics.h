#ifndef PROBE_GRAPHICS_H
#define PROBE_GRAPHICS_H

#include "probe/dllport.h"
#include "probe/types.h"

#include <cstdint>
#include <deque>
#include <string>
#include <vector>

#ifdef BUILD_WITH_QT
#include <QPoint>
#include <QRect>
#endif

// displays
namespace probe::graphics
{
    struct point_t
    {
        int32_t x;
        int32_t y;

#ifdef BUILD_WITH_QT
        PROBE_API operator QPoint() const { return QPoint{ x, y }; }
#endif
    };

    struct geometry_t
    {
        int32_t x;
        int32_t y;
        uint32_t width;
        uint32_t height;

        [[nodiscard]] PROBE_API bool contains(int32_t, int32_t, bool = false) const;
        [[nodiscard]] PROBE_API bool contains(const point_t&, bool = false) const;
        [[nodiscard]] PROBE_API bool contains(const geometry_t&, bool = false) const;
        [[nodiscard]] PROBE_API geometry_t intersected(const geometry_t&) const;

        [[nodiscard]] PROBE_API int32_t left() const { return x; }
        [[nodiscard]] PROBE_API int32_t top() const { return y; }
        [[nodiscard]] PROBE_API int32_t right() const { return x + static_cast<int32_t>(width) - 1; }
        [[nodiscard]] PROBE_API int32_t bottom() const { return y + static_cast<int32_t>(height) - 1; }

        [[nodiscard]] PROBE_API point_t centor() const
        {
            return { .x = x + static_cast<int>(width / 2), .y = y + static_cast<int>(height / 2) };
        }
#ifdef BUILD_WITH_QT
        PROBE_API operator QRect() const
        {
            return QRect{ x, y, static_cast<int>(width), static_cast<int>(height) };
        }
#endif
    };

    PROBE_API bool operator==(const geometry_t&, const geometry_t&);

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
        std::string id{};
        uint64_t handle;

#ifdef _WIN32
        std::string driver; // for registry
#endif

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

// edid of display monitor
namespace probe::graphics
{
    struct edid_t
    {
        std::string manufacturer;
        uint16_t product_code;
        uint32_t serial_number;

        uint16_t year;
        uint16_t week;

        uint8_t version;
        uint8_t revision;

        uint16_t hsize; // centimetres
        uint16_t vsize; // centimetres

        double gamma;
    };

    PROBE_API std::array<char, 256> edid_of(const std::string&, const std::string& = {});

    PROBE_API int parse_edid(const std::array<char, 256>&, edid_t *);
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