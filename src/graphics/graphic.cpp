#include "probe/graphics.h"

#include <limits>

namespace probe
{
    bool geometry_t::operator==(const geometry_t& r) const
    {
        return x == r.x && y == r.y && width == r.width && height == r.height;
    }

    bool geometry_t::contains(int32_t _x, int32_t _y, bool proper) const
    {
        if (proper)
            return (_x > left() && _x < right()) && (_y > top() && _y < bottom());
        else
            return (_x >= left() && _x <= right()) && (_y >= top() && _y <= bottom());
    }

    bool geometry_t::contains(const point_t& point, bool proper) const
    {
        return contains(point.x, point.y, proper);
    }

    bool geometry_t::contains(const geometry_t& r, bool proper) const
    {
        if (proper)
            return left() < r.left() && right() > r.right() && top() < r.top() && bottom() > r.bottom();
        else
            return left() <= r.left() && right() >= r.right() && top() <= r.top() && bottom() >= r.bottom();
    }

    void geometry_t::translate(const point_t& offset)
    {
        x += offset.x;
        y += offset.y;
    }

    void geometry_t::translate(int32_t dx, int32_t dy)
    {
        x += dx;
        y += dy;
    }

    geometry_t geometry_t::translated(const point_t& offset) const
    {
        return { x + offset.x, y + offset.y, width, height };
    }

    geometry_t geometry_t::translated(int32_t dx, int32_t dy) const
    {
        return { x + dx, y + dy, width, height };
    }

    geometry_t geometry_t::intersected(const geometry_t& otr) const
    {
        int32_t l = std::max(left(), otr.left());
        int32_t t = std::max(top(), otr.top());

        int32_t r = std::min(right(), otr.right());
        int32_t b = std::min(bottom(), otr.bottom());

        if (r <= l || b <= t) {
            return {};
        }

        return { l, t, static_cast<uint32_t>(r - l + 1), static_cast<uint32_t>(b - t + 1) };
    }

    geometry_t geometry_t::united(const geometry_t& otr) const
    {
        int32_t l = std::min(left(), otr.left());
        int32_t t = std::min(top(), otr.top());

        int32_t r = std::max(right(), otr.right());
        int32_t b = std::max(bottom(), otr.bottom());

        if (r <= l || b <= t) {
            return {};
        }

        return { l, t, static_cast<uint32_t>(r - l + 1), static_cast<uint32_t>(b - t + 1) };
    }

} // namespace probe

namespace probe::graphics
{
    geometry_t virtual_screen_geometry()
    {
        int32_t l = std::numeric_limits<int32_t>::max(), t = std::numeric_limits<int32_t>::max();
        int32_t r = std::numeric_limits<int32_t>::min(), b = std::numeric_limits<int32_t>::min();

        for (const auto& display : displays()) {
            if (l > display.geometry.x) l = display.geometry.x;

            if (t > display.geometry.y) t = display.geometry.y;

            int32_t this_r = display.geometry.x + display.geometry.width - 1;
            if (r < this_r) r = this_r;

            int32_t this_b = display.geometry.y + display.geometry.height - 1;
            if (b < this_b) b = this_b;
        }

        return geometry_t{
            l,
            t,
            static_cast<uint32_t>(r - l + 1),
            static_cast<uint32_t>(b - t + 1),
        };
    }
} // namespace probe::graphics

namespace probe
{
    std::string to_string(const geometry_t& g)
    {
        return "<<" + std::to_string(g.x) + ", " + std::to_string(g.y) + ">, " + std::to_string(g.width) +
               "x" + std::to_string(g.height) + ">";
    }

    std::string to_string(graphics::orientation_t o)
    {
        switch (o) {
        case graphics::orientation_t::rotate_0:   return "Landscape";
        case graphics::orientation_t::rotate_90:  return "Portrait";
        case graphics::orientation_t::rotate_180: return "Landscape (flipped)";
        case graphics::orientation_t::rotate_270: return "Portrait (flipped)";
        default:                                  return "Unknown";
        }
    }
} // namespace probe