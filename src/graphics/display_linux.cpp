#ifdef __linux__

#include "probe/defer.h"
#include "probe/graphics.h"
#include "probe/process.h"

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <cstring>

// TODO:
//   X11: Xlib or XCB
//   Wayland
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
                .id        = "DISPLAY:" + std::to_string(i),
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

    template<typename T>
    static std::vector<T> xget_property(Display *display, Window win, Atom type, const char *name)
    {
        auto xa_name = ::XInternAtom(display, name, False);

        Atom real_type{};
        int format{};
        unsigned long nb_items{};
        unsigned long bytes{};
        T *data{};
        if (::XGetWindowProperty(display, win, xa_name, 0, ~0L, False, type, &real_type, &format, &nb_items,
                                 &bytes, reinterpret_cast<unsigned char **>(&data)) != Success) {
            return {};
        }

        if (type != real_type) return {};

        std::vector<T> buffer;
        for (size_t i = 0; i < nb_items; ++i) {
            buffer.push_back(data[i]);
        }

        return buffer;
    }

    // name, classname
    static std::pair<std::string, std::string> xget_window_name(Display *display, Window window)
    {
        // title
        std::string title{};
        char *name = nullptr;
        if (::XFetchName(display, window, &name) > 0 && name != nullptr) {
            title = name;
            ::XFree(name);
        }
        else {
            XTextProperty wmname{};
            if (::XGetWMName(display, window, &wmname) != 0 && wmname.value && wmname.format == 8) {
                char **list = nullptr;
                int num     = 0;
                // COMPOUND_TEXT, STRING, UTF8_STRING or the encoding of the current locale is
                // guaranteed
                if (::Xutf8TextPropertyToTextList(display, &wmname, &list, &num) >= 0 && num > 0 && *list) {
                    title = *list;
                    ::XFreeStringList(list);
                }
            }
            ::XFree(wmname.value);
        }

        // class name
        XClassHint hint{};
        ::XGetClassHint(display, window, &hint);

        return { title, hint.res_name ? hint.res_name : "" };
    }

    // process
    static std::pair<uint64_t, std::string> xget_window_process(Display *display, Window window)
    {
        auto buffer = xget_property<unsigned long>(display, window, XA_CARDINAL, "_NET_WM_PID");
        auto pid    = buffer.empty() ? 0 : buffer[0];
        return { pid, probe::process::name(pid) };
    }

    // geometry
    static geometry_t xget_window_geometry(Display *display, Window window)
    {
        unsigned int border, depth;
        Window root;
        geometry_t g{};

        XGetGeometry(display, window, &root, &g.x, &g.y, &g.width, &g.height, &border, &depth);
        XTranslateCoordinates(display, window, DefaultRootWindow(display), 0, 0, &g.x, &g.y, &root);

        return g;
    }

    static Window xget_window_parent(Display *display, Window window)
    {
        Window root, parent;
        Window *children    = nullptr;
        unsigned int number = 0;
        std::vector<window_t> ret;

        ::XQueryTree(display, window, &root, &parent, &children, &number);
        defer(::XFree(children));

        return (DefaultRootWindow(display) == parent) ? 0 : parent;
    }

    static window_t xget_window_info(Display *display, Window window)
    {
        auto [name, cname] = xget_window_name(display, window);
        auto [pid, pname]  = xget_window_process(display, window);

        XWindowAttributes attrs{};
        ::XGetWindowAttributes(display, window, &attrs);

        auto desktop = xget_property<unsigned long>(display, window, XA_CARDINAL, "_NET_WM_DESKTOP");

        return {
            .name      = name,
            .classname = cname,
            .geometry  = xget_window_geometry(display, window),
            .visible   = attrs.map_state >= IsViewable && !attrs.override_redirect,
            .handle    = static_cast<uint64_t>(window),
            .parent    = xget_window_parent(display, window),
            .pid       = pid,
            .pname     = pname,
            .desktop   = desktop.empty() ? 0 : desktop[0],
        };
    }

    static std::vector<window_t> xget_window_children(Display *display, Window window)
    {
        Window root, parent;
        Window *children    = nullptr;
        unsigned int number = 0;
        std::vector<window_t> ret;

        ::XQueryTree(display, window, &root, &parent, &children, &number);

        for (unsigned int i = 0; i < number; ++i) {
            auto child = xget_window_info(display, children[i]);
            if (child.geometry.width * child.geometry.height >= 16) {
                ret.emplace_back(child);
            }
        }

        ::XFree(children);

        return ret;
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
        defer(::XCloseDisplay(display));

        Window root_return, parent_return;
        Window *top_windows = nullptr;
        unsigned int number = 0;
        // The XQueryTree() function returns the root ID, the parent window ID, a pointer to the list of
        // children windows (NULL when there are no children), and the number of children in the list for
        // the specified window. The children are listed in current stacking order, from bottommost (first)
        // to topmost (last). XQueryTree() returns zero if it fails and nonzero if it succeeds. To free a
        // non-NULL children list when it is no longer needed, use XFree().
        //
        // Top windows
        ::XQueryTree(display, DefaultRootWindow(display), &root_return, &parent_return, &top_windows,
                     &number);
        defer(::XFree(top_windows));

        // current virtual desktop
        auto buffer      = xget_property<unsigned long>(display, DefaultRootWindow(display), XA_CARDINAL,
                                                   "_NET_CURRENT_DESKTOP");
        auto desktop_idx = buffer.empty() ? 0 : buffer[0];

        for (unsigned int i = 0; i < number; ++i) {
            auto window = xget_window_info(display, top_windows[i]);

            if (any(flags & window_filter_t::visible) &&
                (!window.visible || (window.geometry.width * window.geometry.height < 16))) {
                continue;
            }

            if (window.desktop != desktop_idx) continue;

            ret.emplace_front(window);

            // children windows
            auto children = xget_window_children(display, top_windows[i]); // top->bottom
            geometry_t last_geometry{};
            std::for_each(children.rbegin(), children.rend(), [&](const auto& subwind) {
                // ignore the children which completely cover their parent
                if (!subwind.geometry.contains(window.geometry)) {
                    // keep the last one of children with same rect
                    if (last_geometry != geometry_t{} && subwind.geometry == last_geometry) {
                        ret.pop_back();
                    }

                    last_geometry = subwind.geometry;
                    ret.emplace_front(subwind);
                }
            });
        }

        return ret;
    }

    std::optional<window_t> active_window()
    {
        auto display = ::XOpenDisplay(nullptr);
        if (!display) return {};
        defer(::XCloseDisplay(display));

        auto buffer =
            xget_property<Window>(display, DefaultRootWindow(display), XA_WINDOW, "_NET_ACTIVE_WINDOW");
        if (buffer.empty()) return std::nullopt;

        return xget_window_info(display, buffer[0]);
    }

    window_t virtual_screen()
    {
        Window handle{};

        auto display = ::XOpenDisplay(nullptr);
        if (display) {
            handle = DefaultRootWindow(display);
            ::XCloseDisplay(display);
        }

        return window_t{
            .name      = "~VIRTUAL-SCREEN",
            .classname = "",
            .geometry  = virtual_screen_geometry(),
            .visible   = true,
            .handle    = reinterpret_cast<uint64_t>(handle),
        };
    }
} // namespace probe::graphics

#endif // __linux__
