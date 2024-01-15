#ifndef PROBE_GRAPHICS_H
#define PROBE_GRAPHICS_H

#include "probe/dllport.h"
#include "probe/enum.h"
#include "probe/types.h"

#include <cstdint>
#include <deque>
#include <optional>
#include <string>
#include <vector>

#ifdef BUILD_WITH_QT
#include <QPoint>
#include <QRect>
#endif

namespace probe
{
    struct point_t
    {
        int32_t x{};
        int32_t y{};

#ifdef BUILD_WITH_QT
        operator QPoint() const { return QPoint{ x, y }; }
#endif
    };

    struct PROBE_API geometry_t
    {
        int32_t  x{};
        int32_t  y{};
        uint32_t width{};
        uint32_t height{};

        bool operator==(const geometry_t&) const;

        bool operator!=(const geometry_t& r) const { return !(*this == r); }

        [[nodiscard]] bool contains(int32_t, int32_t, bool = false) const;
        [[nodiscard]] bool contains(const point_t&, bool = false) const;
        [[nodiscard]] bool contains(const geometry_t&, bool = false) const;

        [[nodiscard]] geometry_t intersected(const geometry_t&) const;
        [[nodiscard]] geometry_t united(const geometry_t&) const;

        void translate(const point_t&);
        void translate(int32_t dx, int32_t dy);

        [[nodiscard]] geometry_t translated(const point_t&) const;
        [[nodiscard]] geometry_t translated(int32_t dx, int32_t dy) const;

        [[nodiscard]] int32_t left() const { return x; }

        [[nodiscard]] int32_t top() const { return y; }

        // the region includes both the left and right pixels.
        [[nodiscard]] int32_t right() const { return x + static_cast<int32_t>(width) - 1; }

        [[nodiscard]] int32_t bottom() const { return y + static_cast<int32_t>(height) - 1; }

        [[nodiscard]] point_t center() const
        {
            return { .x = x + static_cast<int>(width / 2), .y = y + static_cast<int>(height / 2) };
        }

        [[nodiscard]] bool valid() const { return width > 0 && height > 0; };

#ifdef BUILD_WITH_QT
        operator QRect() const { return QRect{ x, y, static_cast<int>(width), static_cast<int>(height) }; }
#endif
    };
} // namespace probe

// displays
namespace probe::graphics
{
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

    struct display_mode_t
    {
        std::string name{};
        bool        actived{};
        uint32_t    width{};
        uint32_t    height{};
        double      frequency{}; // Hz
    };

    struct display_t
    {
        std::string name{};
        std::string id{};
        uint64_t    handle{};

#ifdef _WIN32
        std::string driver{}; // for registry
#endif

        // virtual
        //  Windows: == physical resolution
        //  Linux X: ==  render  resolution
        geometry_t geometry{};
        double     frequency{};  // Hz

        uint32_t      bpp{};
        uint32_t      dpi{ 96 }; // logical dot per inch
        orientation_t orientation{ orientation_t::landscape };
        bool          primary{ false };
        double        scale{ 1.0 };
    };

    PROBE_API std::vector<display_t> displays();

#ifdef _WIN32
    uint32_t retrieve_dpi_for_window(uint64_t wid);

    // probe the display information with handle mid
    PROBE_API std::optional<display_t> display_info_of(uint64_t mid);

    // the display that contains this window
    PROBE_API std::optional<display_t> display_contains(uint64_t wid);

    // the display that contains this point
    PROBE_API std::optional<display_t> display_contains(const point_t& point);

    PROBE_API inline std::optional<display_t> display_contains(int32_t x, int32_t y)
    {
        return display_contains(point_t{ x, y });
    }

#ifdef BUILD_WITH_QT
    PROBE_API inline std::optional<display_t> display_contains(const QPoint& point)
    {
        return display_contains(point.x(), point.y());
    }
#endif

    // the display that contains this rect
    PROBE_API std::optional<display_t> display_contains(const geometry_t& rect);
#endif
} // namespace probe::graphics

// edid of display monitor
namespace probe::graphics
{
    struct edid_t
    {
        std::string manufacturer{};
        uint16_t    product_code{};
        uint32_t    serial_number{};

        uint16_t year{};
        uint16_t week{};

        uint8_t version{};
        uint8_t revision{};

        uint16_t hsize{}; // centimetres
        uint16_t vsize{}; // centimetres

        double gamma{};
    };

    PROBE_API std::array<char, 256> edid_of(const std::string&, const std::string& = {});

    PROBE_API int parse_edid(const std::array<char, 256>&, edid_t *);
} // namespace probe::graphics

// windows
namespace probe::graphics
{
    enum class window_filter_t
    {
        visible    = 0x01,
        capturable = 0x04, // Windows Graphics Capture
        children   = 0x08,

        ENABLE_BITMASK_OPERATORS()
    };

    struct window_t
    {
        std::string name{};      // utf-8
        std::string classname{}; // utf-8

        geometry_t geometry{};   // absolute coordinate
        bool       visible{};    //

        uint64_t handle{};       // window id, Linux: Window, Windows: HWND
        uint64_t parent{};       // handle of parent window

        uint64_t    pid{};       // process id
        std::string pname{};     // process name

        uint64_t desktop{};      // index of the desktop, not used on Windows
    };

    PROBE_API std::deque<window_t> windows(window_filter_t = window_filter_t::visible);

    PROBE_API std::optional<window_t> active_window();

    PROBE_API window_t virtual_screen();

    PROBE_API geometry_t virtual_screen_geometry();
} // namespace probe::graphics

// graphics processing units
namespace probe::graphics
{
    struct gpu_info_t
    {
        std::string name{};

        vendor_t vendor{};

        size_t dedicated_memory{}; // B
        size_t shared_memory{};    // B
        size_t frequency{};
    };

    PROBE_API std::vector<gpu_info_t> info();
} // namespace probe::graphics

namespace probe
{
    PROBE_API std::string to_string(const geometry_t&);
    PROBE_API std::string to_string(graphics::orientation_t);
} // namespace probe

#endif //! PROBE_GRAPHICS_H