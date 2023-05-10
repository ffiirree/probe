#ifdef __linux__

#include "probe/process.h"
#include "probe/time.h"

#include <filesystem>
#include <inttypes.h>
#include <optional>
#include <unistd.h>

namespace probe::process
{
    //  pid : process id
    // ppid : pid of parent process
    // tgid : thread group id, the POSIX pid
    //  tid : task id, the POSIX thread id
    //
    // https://stackoverflow.com/a/9306150
    //
    /*
                          USER VIEW
                          vvvv vvvv
                  |
    <-- PID 43 -->|<----------------- PID 42 ----------------->
                  |                           |
                  |      +---------+          |
                  |      | process |          |
                  |     _| pid=42  |_         |
            ___(fork) _/ | tgid=42 | \_ (new thread) __
           /      |      +---------+          |        \
    +---------+   |                           |    +---------+
    | process |   |                           |    | process |
    | pid=43  |   |                           |    | pid=44  |
    | tgid=43 |   |                           |    | tgid=42 |
    +---------+   |                           |    +---------+
                  |                           |
    <-- PID 43 -->|<--------- PID 42 -------->|<--- PID 44 --->
                  |                           |
                            ^^^^^^ ^^^^
                            KERNEL VIEW
    */
    //
    // The proc filesystem is a pseudo-filesystem which provides an interface to kernel data structures. It
    // is commonly mounted at /proc.
    std::vector<process_t> processes()
    {
        std::vector<process_t> ret{};

        uint64_t sysuptime = uptime();

        for (const auto& entry : std::filesystem::directory_iterator{ "/proc" }) {
            std::string path = entry.path(); // /proc/<PID>
            std::string pids = entry.path().filename();
            // numerical subdirectory for each running process
            if (!entry.is_directory() || !(pids[0] > '0' && pids[0] <= '9')) continue;

            // /proc/<PID>/stat
            auto stat = parse_stat(pids);

            // /proc/<PID>/status
            auto status = parse_status(pids);

            // user
            auto pws = getpwuid(status.ruid);

            ret.emplace_back(process_t{
                .pid        = std::stoi(pids),
                .ppid       = stat.ppid,
                .state      = stat.state,
                .priority   = stat.priority,
                .name       = stat.comm,
                .cmdline    = parse_cmdline(pids),
                .starttime  = (stat.starttime / sysconf(_SC_CLK_TCK)) * 1000'000'000 + sysuptime,
                .nb_threads = static_cast<uint64_t>(stat.nb_threads),
                .user       = pws->pw_name,
            });
        }

        return ret;
    }

    std::vector<thread_t> threads(uint64_t) { return {}; }

    uint64_t memory(uint64_t pid)
    {
        // /proc/[pid]/statm
        auto statm = parse_statm(std::to_string(pid));

        return statm.size * sysconf(_SC_PAGESIZE);
    }
} // namespace probe::process

#endif