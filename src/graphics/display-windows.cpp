#ifdef _WIN32

#include "probe/defer.h"
#include "probe/graphics.h"
#include "probe/process.h"
#include "probe/util.h"

#include <dwmapi.h>
#include <ExDisp.h>
#include <map>
#include <regex>
#include <ShellScalingApi.h>
#include <ShlGuid.h>
#include <ShlObj_core.h>
#include <Shlwapi.h>

namespace probe::graphics
{
    // { classname|name - process }
    std::map<std::string, std::string> blacklist = {
        { "WorkerW|", ".*" },                      //
        { "PseudoConsoleWindow|", ".*" },          // Windows Pseudo Console?
        { "YNoteShadowWnd|", "\\bYoudaoDict\\b" }, // Shadow Window of Youdao Dict
        { "popupshadow|", "\\bWeChat\\b" },        // Shadow Window of  WeChat
        { "QMShadowWndClass|", "\\bQQMusic\\b" },  // Shadow Window of QQMusic
        // children
        { "Windows.UI.Composition.DesktopWindowContentBridge|DesktopWindowXamlSource", ".*" },
        { "DRAG_BAR_WINDOW_CLASS|", ".*" },              // windows terminal
        { "TITLE_BAR_SCAFFOLDING_WINDOW_CLASS|", ".*" }, // explorer.exe
        { "Intermediate D3D Window|", ".*" },
        { "AVL_AVView|AVTopbarResizeView", "\\bAcrobat\\b" },
        { "ScrollBar|", "\\bAcrobat\\b" },
    };

    //  { classname:name - process }
    std::map<std::string, std::string> uncapturable_windows = {
        { "Shell_TrayWnd|", ".*" },                                                   // taskbar
        { "Progman|Program Manager", ".*" },                                          //
        { "TopLevelWindowForOverflowXamlIsland|System tray overflow window.", ".*" }, // System Tray
        { "YdGenericListWnd|YdGenericListWnd", ".*" }, // System Tray Menu of YoudaoDict
        { "Qt5153QWindowToolSaveBits|Microsoft OneDrive", "\\bOneDrive\\b" }
    };

    static std::pair<std::string, std::string> display_name_and_driver_of(const WCHAR *device)
    {
        DISPLAY_DEVICE device_info{};
        device_info.cb = sizeof(DISPLAY_DEVICE);

        int monitor_idx = 0;
        while (::EnumDisplayDevices(device, monitor_idx, &device_info, 0)) {
            //
            if (device_info.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) {

                // example: L"MONITOR\\AOC2790\\{4d36e96e-e325-11ce-bfc1-08002be10318}\\0001"
                //          - name:     AOC2790
                //          - driver:   {4d36e96e-e325-11ce-bfc1-08002be10318}\\0001
                const auto device_id = util::to_utf8(device_info.DeviceID);

                const auto _1 = device_id.find_first_of('\\');
                const auto _2 = device_id.find_first_of('\\', _1 + 1);

                return std::pair{ device_id.substr(_1 + 1, _2 - _1 - 1), device_id.substr(_2 + 1) };
            }
            ++monitor_idx;
        }

        return {};
    }

    // https://learn.microsoft.com/en-us/windows/win32/hidpi/setting-the-default-dpi-awareness-for-a-process
    // <  Windows 8.1       : GetDeviceCaps(hdc, LOGPIXELSX)
    // >= Windows 8.1       : GetDpiForMonitor(, MDT_EFFECTIVE_DPI, ,) with
    // SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE), but can lead to unexpected application
    // behavior.
    // >= Windows 10, 1607  : GetDpiForMonitor(, MDT_EFFECTIVE_DPI, ,) with
    // SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)
    //
    // Just for >= 1607
    static UINT retrieve_dpi_for_monitor(HMONITOR monitor)
    {
        const auto PRE_DAC = ::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        defer(::SetThreadDpiAwarenessContext(PRE_DAC));

        UINT dpi = 96, _;
        ::GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpi, &_);

        return dpi;
    }

    // >= Windows 10, 1607
    uint32_t retrieve_dpi_for_window(uint64_t hwnd)
    {
        const auto PRE_DAC = ::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        defer(::SetThreadDpiAwarenessContext(PRE_DAC));

        return ::GetDpiForWindow(reinterpret_cast<HWND>(hwnd));
    }

    std::optional<display_t> display_info_of(uint64_t mid)
    {
        // prevent the GetMonitorInfo from being affected by the process dpi awareness
        const auto PRE_DAC = ::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);
        defer(::SetThreadDpiAwarenessContext(PRE_DAC));

        MONITORINFOEX info = { sizeof(MONITORINFOEX) };
        if (::GetMonitorInfo(reinterpret_cast<HMONITOR>(mid), &info)) {

            DEVMODE settings = {};
            settings.dmSize  = sizeof(DEVMODE);

            if (::EnumDisplaySettings(info.szDevice, ENUM_CURRENT_SETTINGS, &settings)) {

                const auto [name, driver] = display_name_and_driver_of(info.szDevice);
                return display_t{
                    .name   = name,
                    .id     = util::to_utf8(info.szDevice),
                    .handle = mid,
                    .driver = driver,
                    .geometry =
                        geometry_t{
                            settings.dmPosition.x,
                            settings.dmPosition.y,
                            settings.dmPelsWidth,
                            settings.dmPelsHeight,
                        },
                    .frequency   = static_cast<double>(settings.dmDisplayFrequency),
                    .bpp         = settings.dmBitsPerPel,
                    .dpi         = retrieve_dpi_for_monitor(reinterpret_cast<HMONITOR>(mid)),
                    .orientation = static_cast<orientation_t>(0x01 << settings.dmDisplayOrientation),
                    .primary     = (settings.dmPosition.x == 0) && (settings.dmPosition.y == 0),
                    .scale       = static_cast<float>(settings.dmPelsWidth) /
                             (info.rcMonitor.right - info.rcMonitor.left),
                };
            }
        }

        return std::nullopt;
    }

    // https://learn.microsoft.com/en-us/windows/win32/gdi/multiple-display-monitors
    //
    // The bounding rectangle of all the monitors is the `virtual screen`.
    // The `desktop` covers the virtual screen instead of a single monitor.
    // The `primary monitor` contains the origin (0,0) for compatibility.
    //
    // Each `physical display` is represented by a monitor handle of type HMONITOR.
    // A valid HMONITOR is guaranteed to be non-NULL.
    // A physical display has the same HMONITOR as long as it is part of the desktop.
    // When a WM_DISPLAYCHANGE message is sent, any monitor may be removed from the desktop
    // and thus its HMONITOR becomes invalid or has its settings changed.
    // Therefore, an application should check whether all HMONITORS are valid when this message is sent.
    //
    // Any function that returns a display device context(DC) normally returns a DC for the primary monitor.
    // To obtain the DC for another monitor, use the EnumDisplayMonitors function.
    // Or, you can use the device name from the GetMonitorInfo function to create a DC with CreateDC.
    // However, if the function, such as GetWindowDC or BeginPaint, gets a DC for a window that spans more
    // than one display, the DC will also span the two displays.
    //
    // To enumerate all the devices on the computer, call the EnumDisplayDevices function.
    // The information that is returned also indicates which monitor is part of the desktop.
    //
    // To enumerate the devices on the desktop that intersect a clipping region, call EnumDisplayMonitors.
    // This returns the HMONITOR handle to each monitor, which is used with GetMonitorInfo.
    // To enumerate all the devices in the virtual screen, use EnumDisplayMonitors.
    //
    // When using multiple monitors as independent displays, the desktop contains one display or set of
    // displays.
    std::vector<display_t> displays()
    {
        std::vector<display_t> ret{};

        // retrieve all monitors
        ::EnumDisplayMonitors(
            nullptr, nullptr,
            [](auto monitor, auto, auto, auto userdata) -> BOOL {
                const auto ret = reinterpret_cast<std::vector<display_t> *>(userdata);

                if (const auto display = display_info_of(reinterpret_cast<uint64_t>(monitor));
                    display.has_value()) {
                    ret->push_back(display.value());
                }

                return TRUE;
            },
            reinterpret_cast<LPARAM>(&ret));

        return ret;
    }

    std::optional<display_t> display_contains(uint64_t wid)
    {
        HMONITOR hmonitor = ::MonitorFromWindow(reinterpret_cast<HWND>(wid), MONITOR_DEFAULTTONEAREST);

        return display_info_of(reinterpret_cast<uint64_t>(hmonitor));
    }

    std::optional<display_t> display_contains(const point_t& point)
    {
        HMONITOR hmonitor = ::MonitorFromPoint(POINT{ point.x, point.y }, MONITOR_DEFAULTTONEAREST);

        return display_info_of(reinterpret_cast<uint64_t>(hmonitor));
    }

    std::optional<display_t> display_contains(const geometry_t& r)
    {
        const auto rect = RECT{
            static_cast<LONG>(r.left()),
            static_cast<LONG>(r.top()),
            static_cast<LONG>(r.right()),
            static_cast<LONG>(r.bottom()),
        };

        HMONITOR hmonitor = ::MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);

        return display_info_of(reinterpret_cast<uint64_t>(hmonitor));
    }

    static std::pair<std::string, std::string> window_name_of(HWND hwnd)
    {
        // title
        std::wstring name;
        auto         name_len = ::GetWindowTextLength(hwnd);
        if (name_len > 0) {
            name.resize(name_len + 1, {});

            name_len = ::GetWindowText(hwnd, name.data(), name_len + 1);
        }

        // classname
        std::wstring classname(256, {});
        const auto   cn_len = ::GetClassName(hwnd, classname.data(), 256);

        const auto u8name = util::trim(util::to_utf8(name.c_str(), name_len));
        return {
            u8name.substr(0, u8name.find_first_of("\n\r")),
            util::to_utf8(classname.c_str(), cn_len),
        };
    }

    static bool IsWindowCloaked(HWND hwnd)
    {
        BOOL is_cloaked = false;

        if (SUCCEEDED(::DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &is_cloaked, sizeof(is_cloaked)))) {
            return is_cloaked;
        }

        return false;
    }

    static bool IsWindowBlocked(const window_t& win)
    {
        const auto key = win.classname + "|" + win.name;

        return blacklist.contains(key) &&
               std::regex_search(win.pname, std::regex(blacklist[key], std::regex_constants::icase));
    }

    static bool IsWindowCapturable(const window_t& win)
    {
        const auto key = win.classname + "|" + win.name;

        return !(uncapturable_windows.contains(key) &&
                 std::regex_search(win.pname,
                                   std::regex(uncapturable_windows[key], std::regex_constants::icase)));
    }

    static auto process_of(HWND hwnd)
    {
        // process
        DWORD       pid = 0;
        std::string pname{};
        if (::GetWindowThreadProcessId(hwnd, &pid) != 0) {
            if (pid != 0 && ::GetCurrentProcessId() != pid) {
                pname = process::name(pid);
            }
        }

        return std::pair{ pid, pname };
    }

    static window_t window_info_of(HWND hwnd)
    {
        auto [name, classname] = window_name_of(hwnd);

        // rect
        RECT rect{};
        ::DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));

        // visible
        const bool visible = ::IsWindowVisible(hwnd) && !IsWindowCloaked(hwnd);

        // process
        auto [pid, pname] = process_of(hwnd);

        return {
            .name      = name,
            .classname = classname,
            .geometry =
                geometry_t{
                    rect.left,
                    rect.top,
                    static_cast<uint32_t>(rect.right - rect.left),
                    static_cast<uint32_t>(rect.bottom - rect.top),
                },
            .visible = visible,
            .handle  = reinterpret_cast<uint64_t>(hwnd),
            .pid     = pid,
            .pname   = pname,
        };
    }

    static std::vector<window_t> children_of(HWND hwnd)
    {
        std::pair<std::vector<window_t>, HWND> bundle{ {}, hwnd };

        ::EnumChildWindows(
            hwnd,
            [](HWND chwnd, LPARAM userdata) -> BOOL {
                // invisible
                if (!::IsWindowVisible(chwnd) || IsWindowCloaked(chwnd)) return TRUE;

                const auto bundle = reinterpret_cast<std::pair<std::vector<window_t>, HWND> *>(userdata);

                // classname - name
                auto [name, cname] = window_name_of(chwnd);

                //
                RECT rect{};
                ::GetWindowRect(chwnd, &rect);

                // process
                auto [pid, pname] = process_of(chwnd);

                auto subwin = window_t{
                    .name      = name,
                    .classname = cname,
                    .geometry =
                        geometry_t{
                            rect.left,
                            rect.top,
                            static_cast<uint32_t>(rect.right - rect.left),
                            static_cast<uint32_t>(rect.bottom - rect.top),
                        },
                    .visible = true,
                    .handle  = reinterpret_cast<uint64_t>(chwnd),
                    .parent  = reinterpret_cast<uint64_t>(bundle->second),
                    .pid     = pid,
                    .pname   = pname,
                };

                // ignored
                if (subwin.geometry.width * subwin.geometry.height < 16 || IsWindowBlocked(subwin))
                    return TRUE;

                bundle->first.emplace_back(subwin);

                return TRUE;
            },
            reinterpret_cast<LPARAM>(&bundle));

        return bundle.first;
    }

    std::deque<window_t> windows(window_filter_t flags)
    {
        const auto desktop_geo = virtual_screen_geometry();

        std::deque<window_t> ret;

        // Z-index: up to down
        // only including the windows in current virtual desktop (WIN + TAB: virtual desktops)
        for (auto hwnd = ::GetTopWindow(nullptr); hwnd != nullptr;
             hwnd      = ::GetNextWindow(hwnd, GW_HWNDNEXT)) {
            auto window = window_info_of(hwnd);

            // invisible
            if (any(flags & window_filter_t::visible)) {
                // invisible
                if (!window.visible) continue;

                // out of sight
                const auto winrect = desktop_geo.intersected(window.geometry);
                if (winrect.width * winrect.height < 16) continue;

                // ignored
                if (IsWindowBlocked(window)) continue;
            }

            // wgc capturable
            if (any(flags & window_filter_t::capturable)) {
                // not children
                if (::GetAncestor(hwnd, GA_ROOT) != hwnd) continue;

                //
                if (!IsWindowCapturable(window)) continue;
            }

            // children
            if (any(flags & window_filter_t::children)) {
                auto       children = children_of(hwnd);
                geometry_t last_rect{};
                std::for_each(children.rbegin(), children.rend(), [&](const auto& subwind) {
                    // ignore the children which completely cover their parent
                    if (!subwind.geometry.contains(window.geometry)) {
                        // keep the last one of children with same rect
                        if (last_rect != geometry_t{} && subwind.geometry == last_rect) {
                            ret.pop_back();
                        }

                        last_rect = subwind.geometry;
                        ret.emplace_back(subwind);
                    }
                });
            }

            ret.emplace_back(window);
        }

        return ret;
    }

    std::optional<window_t> active_window()
    {
        if (const auto hwnd = ::GetForegroundWindow()) {
            return window_info_of(hwnd);
        }
        return std::nullopt;
    }

    window_t virtual_screen()
    {
        auto handle = ::GetDesktopWindow();

        auto [_, cname] = window_name_of(handle);

        return window_t{
            .name      = "~VIRTUAL-SCREEN",
            .classname = cname,
            .geometry  = virtual_screen_geometry(),
            .handle    = reinterpret_cast<uint64_t>(handle),
        };
    }
} // namespace probe::graphics

namespace probe::graphics
{
    std::string explorer_focused(uint64_t wid)
    {
        TCHAR path_buf[MAX_PATH]{};
        TCHAR item_buf[MAX_PATH]{};

        try {
            winrt::com_ptr<IShellWindows> shell_windows{};
            winrt::check_hresult(::CoCreateInstance(winrt::guid_of<ShellWindows>(), nullptr, CLSCTX_ALL,
                                                    winrt::guid_of<IShellWindows>(),
                                                    shell_windows.put_void()));

            VARIANT i{};
            V_VT(&i) = VT_I4;
            winrt::com_ptr<IDispatch>      dispatch{};
            winrt::com_ptr<IWebBrowserApp> wba{};

            for (V_I4(&i) = 0; shell_windows->Item(i, dispatch.put()) == S_OK; V_I4(&i)++) {

                auto cur = dispatch.as<IWebBrowserApp>();

                HWND hwnd{};
                if (SUCCEEDED(cur->get_HWND(reinterpret_cast<LONG_PTR *>(&hwnd))) && hwnd == (HWND)(wid)) {
                    wba = cur;
                    break;
                }
            }

            if (!wba) return {};

            auto provider = wba.as<IServiceProvider>();

            winrt::com_ptr<IShellBrowser> browser{};
            winrt::check_hresult(
                provider->QueryService(SID_STopLevelBrowser, IID_IShellBrowser, browser.put_void()));

            winrt::com_ptr<IShellView> sv{};
            winrt::check_hresult(browser->QueryActiveShellView(sv.put()));

            auto                            fv = sv.as<IFolderView>();
            winrt::com_ptr<IPersistFolder2> pf{};
            pf.capture(fv, &IFolderView::GetFolder);

            LPITEMIDLIST pidf{};
            defer(CoTaskMemFree(pidf));
            winrt::check_hresult(pf->GetCurFolder(&pidf));

            if (!SHGetPathFromIDList(pidf, path_buf)) return {};

            int idx{};
            winrt::check_hresult(fv->GetFocusedItem(&idx));

            LPITEMIDLIST item{};
            defer(CoTaskMemFree(item));
            winrt::check_hresult(fv->Item(idx, &item));

            auto   sf = pf.as<IShellFolder>();
            STRRET str;
            winrt::check_hresult(sf->GetDisplayNameOf(item, SHGDN_INFOLDER, &str));

            StrRetToBuf(&str, item, item_buf, MAX_PATH);
        }
        catch (const winrt::hresult_error& e) {
            return {};
        }

        return probe::util::to_utf8(path_buf) + "\\" + probe::util::to_utf8(item_buf);
    }

} // namespace probe::graphics

#endif // _WIN32
