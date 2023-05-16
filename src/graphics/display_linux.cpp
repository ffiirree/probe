#ifdef __linux__

#include "probe/defer.h"
#include "probe/graphics.h"

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

namespace probe::graphics
{
    static double calculate_frequency(XRRScreenResources *res, RRMode mode_id)
    {
        for (auto k = 0; k < res->nmode; ++k) {
            auto mode = res->modes[k];
            if (mode.id == mode_id) {
                if (mode.hTotal != 0 && mode.vTotal != 0) {
                    return static_cast<double>((1000 * mode.dotClock) / (mode.hTotal * mode.vTotal)) /
                           1000.0;
                }
                break;
            }
        }

        return 0;
    }

    // The X Window System supports one or more screens containing overlapping windows or subwindows. A
    // screen is a physical monitor and hardware, which can be either color, grayscale, or monochrome. There
    // can be multiple screens for each display or workstation. A single X server can provide display
    // services for any number of screens. A set of screens for a single user with one keyboard and one
    // pointer (usually a mouse) is called a display.
    //
    // A slightly unusual feature is that a display is defined as a workstation consisting of a keyboard, a
    // pointing device such as a mouse, and one or more screens. Multiple screens can work together, with
    // mouse movement allowed to cross physical screen boundaries. As long as multiple screens are
    // controlled by a single user with a single keyboard and pointing device, they comprise only a single
    // display.
    //
    // ATTENTION: our 'display' concept is equal to the 'screen' concept in X.
    std::vector<display_t> displays()
    {
        std::vector<display_t> _displays;

        // display
        auto display = XOpenDisplay(nullptr);
        if (!display) return {};
        defer(XCloseDisplay(display));

        // display number
        int display_num = 0;
        auto monitors   = XRRGetMonitors(display, DefaultRootWindow(display), True, &display_num);
        defer(XRRFreeMonitors(monitors));

        auto screen_res = XRRGetScreenResources(display, XDefaultRootWindow(display));
        defer(XRRFreeScreenResources(screen_res));

        for (auto i = 0; i < display_num; ++i) {
            // monitors[i]->noutput == 1
            auto output_info = XRRGetOutputInfo(display, screen_res, monitors[i].outputs[0]);
            defer(XRRFreeOutputInfo(output_info));

            if (output_info->connection == RR_Disconnected || !output_info->crtc) {
                continue;
            }

            auto crtc_info = XRRGetCrtcInfo(display, screen_res, output_info->crtc);
            defer(XRRFreeCrtcInfo(crtc_info));

            //
            _displays.push_back({
                .name      = XGetAtomName(display, monitors[i].name),
                .id        = XGetAtomName(display, monitors[i].name),
                .handle    = reinterpret_cast<uint64_t>(display),
                .geometry  = geometry_t{ crtc_info->x, crtc_info->y, crtc_info->width, crtc_info->height },
                .frequency = calculate_frequency(screen_res, crtc_info->mode),
                .bpp       = static_cast<uint32_t>(DefaultDepth(display, 0)),   // global
                .dpi       = static_cast<uint32_t>((DisplayWidth(display, 0) * 25.4) /
                                             DisplayWidthMM(display, 0)), // global
                .orientation =
                    static_cast<orientation_t>((crtc_info->rotation & 0x000f) |
                                               static_cast<uint32_t>(!!(crtc_info->rotation & 0x00f0))),
                .primary = !!monitors[i].primary,
                .scale   = 1.0 // TODO:
            });
        }
        return _displays;
    }
    //
    // All the windows in an X server are arranged in strict hierarchies. At the top of each hierarchy is a
    // root window, which covers each of the display screens. Each root window is partially or completely
    // covered by child windows. All windows, except for root windows, have parents. There is usually at
    // least one window for each application program. Child windows may in turn have their own children. In
    // this way, an application program can create an arbitrarily deep tree on each screen. X provides
    // graphics, text, and raster operations for windows.
    std::deque<window_t> windows(window_filter_t flags)
    {
        std::deque<window_t> ret;

        auto display = ::XOpenDisplay(nullptr);
        if (!display) return {};

        auto root_window = DefaultRootWindow(display);

        Window root_return, parent_return;
        Window *child_windows  = nullptr;
        unsigned int child_num = 0;
        // The XQueryTree() function returns the root ID, the parent window ID, a pointer to the list of
        // children windows (NULL when there are no children), and the number of children in the list for
        // the specified window. The children are listed in current stacking order, from bottommost (first)
        // to topmost (last). XQueryTree() returns zero if it fails and nonzero if it succeeds. To free a
        // non-NULL children list when it is no longer needed, use XFree().
        ::XQueryTree(display, root_window, &root_return, &parent_return, &child_windows, &child_num);

        for (unsigned int i = 0; i < child_num; ++i) {
            XWindowAttributes attrs{};

            ::XGetWindowAttributes(display, child_windows[i], &attrs);

            if (any(flags & window_filter_t::visible) &&
                (attrs.map_state < 2 || (attrs.width * attrs.height < 4))) {
                continue;
            }

            // title
            std::string title{};
            char *name = nullptr;
            if (::XFetchName(display, child_windows[i], &name) > 0 && name != nullptr) {
                title = name;
                ::XFree(name);
            }
            else {
                XTextProperty wmname{};
                if (::XGetWMName(display, child_windows[i], &wmname) != 0 && wmname.value &&
                    wmname.format == 8) {
                    char **list = nullptr;
                    int num     = 0;
                    // COMPOUND_TEXT, STRING, UTF8_STRING or the encoding of the current locale is
                    // guaranteed
                    if (::Xutf8TextPropertyToTextList(display, &wmname, &list, &num) >= 0 && num > 0 &&
                        *list) {
                        title = *list;
                        ::XFreeStringList(list);
                    }
                }
                ::XFree(wmname.value);
            }

            ret.push_front({
                .name      = title,
                .classname = {},
                .rect =
                    geometry_t{
                        attrs.x,
                        attrs.y,
                        static_cast<uint32_t>(attrs.width),
                        static_cast<uint32_t>(attrs.height),
                    },
                .handle  = static_cast<uint64_t>(child_windows[i]),
                .visible = attrs.map_state >= 2,
            });
        }

        ::XFree(child_windows);
        ::XCloseDisplay(display);

        return ret;
    }
} // namespace probe::graphics

#endif // __linux__
