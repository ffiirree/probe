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

            // path
            std::string proc_exe = "/proc/" + pids + "/exe", proc_path{};
            try {
                if (!::access(proc_exe.c_str(), R_OK) && std::filesystem::exists(proc_exe))
                    proc_path = std::filesystem::canonical(proc_exe);
            }
            catch (...) {
                proc_path = {};
            }

            ret.emplace_back(process_t{
                .pid        = std::stoi(pids),
                .ppid       = stat.ppid,
                .state      = stat.state,
                .priority   = stat.priority,
                .name       = stat.comm,
                .path       = proc_path,
                .cmdline    = parse_cmdline(pids),
                .starttime  = (stat.starttime / sysconf(_SC_CLK_TCK)) * 1'000'000'000 + sysuptime,
                .nb_threads = static_cast<uint64_t>(stat.nb_threads),
                .user       = pws->pw_name,
            });
        }

        return ret;
    }

    int64_t id() { return getpid(); }

    std::string name(uint64_t pid) { return parse_comm(pid); }

    std::string path(uint64_t pid)
    {
        auto proc_exe = "/proc/" + std::to_string(pid) + "/exe";
        if (std::filesystem::exists(proc_exe)) {
            return std::filesystem::canonical(proc_exe);
        }
        return {};
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