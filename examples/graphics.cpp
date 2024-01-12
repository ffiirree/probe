#include "probe/graphics.h"

#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#endif

int main()
{
#ifdef _WIN32
    // https://stackoverflow.com/a/45622802
    // Set console code page to UTF-8 so console known how to interpret string data
    SetConsoleOutputCP(CP_UTF8);

    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    setvbuf(stdout, nullptr, _IOFBF, 1'000);
#endif

    auto displays = probe::graphics::displays();

    std::cout << "Displays: \n";
    std::cout << "    Virtual Screen   : " << probe::to_string(probe::graphics::virtual_screen_geometry())
              << "\n\n";
    for (const auto& display : displays) {
        std::cout << "    Name             : " << display.name << '\n'
#ifdef _WIN32
                  << "    ID               : " << display.id << '\n'
                  << "    Driver           : " << display.driver << '\n'
#endif
                  << "    Primary          : " << display.primary << '\n'
                  << "    Geometry         : " << probe::to_string(display.geometry) << '\n'
                  << "    Frequency        : " << display.frequency << " Hz\n"
                  << "    BPP              : " << display.bpp << '\n'
                  << "    Logical DPI      : " << display.dpi << " dpi\n"
                  << "    Scale            : " << std::lround(display.scale * 100) << " %\n"
                  << "    Orientation      : " << probe::to_string(display.orientation) << "\n";
#ifdef _WIN32
        probe::graphics::edid_t edid;
        if (!probe::graphics::parse_edid(probe::graphics::edid_of(display.name, display.driver), &edid)) {
            std::cout << "    Physical HSize   : " << edid.hsize << " cm\n"
                      << "    Physical VSize   : " << edid.vsize << " cm\n"
                      << "    Physical DPI     : " << (display.geometry.width * 2.54) / edid.hsize
                      << " dpi\n";
        }
#endif
        std::cout << "\n";
    }

    auto gpus = probe::graphics::info();

    std::cout << "GPUs: \n";
    for (const auto& gpu : gpus) {
        std::cout << "    Name             : " << gpu.name << '\n'
                  << "    Vendor           : " << probe::vendor_cast(gpu.vendor) << '\n'
                  << "    Dedicated Memory : " << gpu.dedicated_memory << " B\n"
                  << "    Shared Memory    : " << gpu.shared_memory << " B\n\n";
    }

    auto windows = probe::graphics::windows(probe::graphics::window_filter_t::visible |
                                            probe::graphics::window_filter_t::children);

    std::cout << "Windows: \n";
    for (const auto& win : windows) {
        std::cout << "    handle: " << std::setw(8) << std::hex << std::uppercase << win.handle
                  << ", visible: " << win.visible << ", rect: " << std::setw(28)
                  << probe::to_string(win.geometry) << ", parent: " << std::setw(8) << std::hex
                  << std::uppercase << win.parent << ", process: " << std::setw(24) << win.pname
                  << ", (class)name: '" << std::setw(48) << win.classname << "' - '" << win.name << "'\n";
    }
    return 0;
}