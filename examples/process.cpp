#include "probe/process.h"

#include "probe/time.h"
#include "probe/util.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <ranges>
#ifdef _WIN32
#include <Windows.h>
#endif

static std::string running_time(uint64_t st)
{
    auto now      = probe::time::system_time();
    auto duration = (now - (!st ? now : st)) / 1000'000'000; // ns -> s
    auto s        = static_cast<int>(duration % 60);
    auto m        = static_cast<int>((duration / 60) % 60);
    auto h        = static_cast<int>((duration / 3600) % 24);
    char buffer[16]{};
    std::sprintf(buffer, "%02d:%02d:%02d", h, m, s);
    return buffer;
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IOFBF, 1000);
#endif

    auto processes = probe::process::processes();

    std::cout << "Processes: \n";
    std::cout << "       USER          PID   Memory(KB)     Time   Threads Path\n";
    for (const auto& process : processes) {
        std::cout << std::setw(16) << process.user.substr(0, 16) << ' ' << std::setw(7) << process.pid
                  << ' ' << std::setw(12) << probe::process::memory(process.pid) / 1024 << ' '
                  << std::setw(10) << running_time(process.starttime) << ' ' << std::setw(7)
                  << process.nb_threads << ' '
#ifdef __linux__
                  << (process.cmdline.empty() ? ("[" + process.name + "]") : process.cmdline.substr(0, 64))
                  << '\n';
#elif _WIN32
                  << process.path << "\n";
#endif
    }
    return 0;
}