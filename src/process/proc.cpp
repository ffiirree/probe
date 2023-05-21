#ifdef __linux__

#include "probe/defer.h"
#include "probe/process.h"
#include "probe/time.h"
#include "probe/util.h"

#include <fstream>
#include <map>
#include <regex>
#include <sstream>
#include <string.h>
#include <string>

namespace probe::process
{
    static std::string file_read(const std::string& file)
    {
        std::ifstream ifs(file);

        if (!ifs) return {};

        std::stringstream buffer;
        buffer << ifs.rdbuf();

        return buffer.str();
    }

    // /proc/uptime
    uint64_t uptime()
    {
        auto uptime_fd = ::fopen("/proc/uptime", "r");
        double uptime{};
        if (uptime_fd) {
            defer(::fclose(uptime_fd));

            if (::fscanf(uptime_fd, "%lf", &uptime) == 1) {
                return probe::time::system_time() - static_cast<uint64_t>(uptime * 1'000'000'000);
            }
        }
        return 0;
    }

    // /proc/[pid]/stat
    pstat_t parse_stat(uint64_t pid) { return parse_stat(std::to_string(pid)); }

    pstat_t parse_stat(const std::string& pid)
    {
        auto stat_fd = ::fopen(("/proc/" + pid + "/stat").c_str(), "r");
        pstat_t s{};
        char buffer[512]{};

        if (stat_fd) {
            defer(::fclose(stat_fd));

            // https://man7.org/linux/man-pages/man5/proc.5.html
            if (::fscanf(
                    stat_fd,
                    "%d %s "                   // pid, comm
                    "%c %d "                   // state, ppid
                    "%d %d %d %d "             // pgrp, session, tty_nr, tpgid
                    "%u %lu %lu %lu %lu "      // flags, minflt, cminflt, majflt, cmajflt
                    "%lu %lu %ld %ld "         // utime, stime, cutime, cstime
                    "%ld %ld "                 // priority, nice
                    "%ld "                     // num_threads
                    "%*s "                     // itrealvalue, not maintained
                    "%llu "                    // starttime
                    "%lu "                     // vsize
                    "%ld "                     // rss
                    "%lu %lu %lu %lu %lu %lu " // rsslim, startcode, endcode, startstack, kstkesp, kstkeip
                    "%*s %*s %*s %*s "         // signal, blocked, sigign, sigcatch, use /proc/[pid]/status
                    "%lu %*s %*s "             // wchan, (nswap, cnswap: not maintained)
                    "%d %d "                   // exit_signal, processor
                    "%u %u "                   // rt_priority, policy (sched)
                    "%llu %lu %ld"             // blkio_ticks, guest_time, cguest_time
                    ,
                    &s.pid, buffer, &s.state, &s.ppid, &s.pgrp, &s.session, &s.tty_nr, &s.tpgid, &s.flags,
                    &s.minflt, &s.cminflt, &s.majflt, &s.cmajflt, &s.utime, &s.stime, &s.cutime, &s.cstime,
                    &s.priority, &s.nice, &s.nb_threads, &s.starttime, &s.vsize, &s.rss, &s.rsslim,
                    &s.startcode, &s.endcode, &s.startstack, &s.kstkesp, &s.kstkeip, &s.wchan,
                    &s.exit_signal, &s.processor, &s.rt_priority, &s.policy, &s.blkio_ticks, &s.guest_time,
                    &s.cguest_time) == 0) {
                return {};
            }
        }

        std::string comm = buffer;
        auto fpos        = comm.find_first_of("(");
        auto lpos        = comm.find_last_of(")");
        if (fpos != std::string::npos && lpos != std::string::npos) {
            s.comm = comm.substr(fpos + 1, lpos - fpos - 1);
        }

        return s;
    }

    // /proc/[pid]/io
    pio_t parse_io(uint64_t pid) { return parse_io(std::to_string(pid)); }

    pio_t parse_io(const std::string& pid)
    {
        auto io_fd = ::fopen(("/proc/" + pid + "/io").c_str(), "r");
        pio_t io{};

        if (io_fd) {
            defer(::fclose(io_fd));

            if (::fscanf(io_fd, "%lu %lu %lu %lu %lu %lu %lu", &io.rchar, &io.wchar, &io.syscr, &io.syscw,
                         &io.read_bytes, &io.write_bytes, &io.cancelled_write_bytes) == 0) {
                return {};
            }
        }
        return io;
    }

    std::vector<unsigned long> parse_uids(const std::string& str)
    {
        std::smatch matchs;
        if (std::regex_match(str, matchs, std::regex("(\\d+)\t(\\d+)\t(\\d+)\t(\\d+)"))) {
            return {
                std::stoul(matchs[1].str()),
                std::stoul(matchs[2].str()),
                std::stoul(matchs[3].str()),
                std::stoul(matchs[4].str()),
            };
        }
        return { 0, 0, 0, 0 };
    }

    // /proc/[pid]/statm
    pstatm_t parse_statm(uint64_t pid) { return parse_statm(std::to_string(pid)); }

    pstatm_t parse_statm(const std::string& pid)
    {
        auto statm_fd = ::fopen(("/proc/" + pid + "/statm").c_str(), "r");
        pstatm_t m{};

        if (statm_fd) {
            defer(::fclose(statm_fd));

            if (::fscanf(statm_fd, "%lu %lu %lu %lu %lu", &m.size, &m.resident, &m.shared, &m.text,
                         &m.data) == 0) {
                return {};
            }
        }
        return m;
    }

    // /proc/[pid]/status
    pstatus_t parse_status(uint64_t pid) { return parse_status(std::to_string(pid)); }

    pstatus_t parse_status(const std::string& pid)
    {
        std::ifstream status_fd("/proc/" + pid + "/status");
        if (!status_fd.is_open() || !status_fd) return {};

        std::map<std::string, std::string> mapping;
        for (std::string line; std::getline(status_fd, line);) {
            auto pos = line.find_first_of(":");
            if (pos != std::string::npos) {
                mapping[line.substr(0, pos)] = line.substr(pos + 2); // skip ":\t"
            }
        }

#define MAPPING_INT(X, F, D) (mapping.contains(#X) ? std::sto##F(mapping[#X]) : D)
#define MAPPING_HEX(X, F, D) (mapping.contains(#X) ? std::sto##F(mapping[#X], nullptr, 16) : D)
#define MAPPING_STR(X, D)    (mapping.contains(#X) ? mapping[#X] : D)

        auto uids = parse_uids(MAPPING_STR(Uid, ""));
        auto gids = parse_uids(MAPPING_STR(Gid, ""));

        // clang-format off
        return pstatus_t {
            .name       = MAPPING_STR(Name, ""),

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 7, 0)
            .umask      = (mapping.contains("Umask") ? std::stoul(mapping["Umask"], nullptr, 8) : 0),
#endif

            .state      = static_cast<pstate_t>(MAPPING_STR(State, std::string("\0"))[0]),

            .tgid       = MAPPING_INT(Tgid, i, 0),
            .ngid       = MAPPING_INT(Ngid, i, 0),
            .pid        = MAPPING_INT(Pid, i, 0),
            .ppid       = MAPPING_INT(PPid, i, 0),

            .tracer_pid = MAPPING_INT(TracerPid, i, 0),

            .ruid       = static_cast<uid_t>(uids[0]),
            .euid       = static_cast<uid_t>(uids[1]),
            .suid       = static_cast<uid_t>(uids[2]),
            .fuid       = static_cast<uid_t>(uids[3]),

            .rgid       = static_cast<gid_t>(gids[0]),
            .egid       = static_cast<gid_t>(gids[1]),
            .sgid       = static_cast<gid_t>(gids[2]),
            .fgid       = static_cast<gid_t>(gids[3]),

            .fd_size    = MAPPING_INT(FDSize, l, 0),
            .groups     = MAPPING_STR(Groups, ""),

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0)
            .nstgid     = MAPPING_INT(NStgid, i, 0),
            .nspid      = MAPPING_INT(NSpid, i, 0),
            .nspgid     = MAPPING_INT(NSpgid, i, 0),
            .nssid      = MAPPING_INT(NSsid, i, 0),
#endif

            .vm_peak    = MAPPING_INT(VmPeak, ul, 0),
            .vm_size    = MAPPING_INT(VmSize, ul, 0),
            .vm_lck     = MAPPING_INT(VmLck, ul, 0),
            .vm_pin     = MAPPING_INT(VmPin, ul, 0),
            .vm_hwm     = MAPPING_INT(VmHWM, ul, 0),
            .vm_rss     = MAPPING_INT(VmRSS, ul, 0),

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0)
            .rss_anon   = MAPPING_INT(RssAnon, ul, 0),
            .rss_file   = MAPPING_INT(RssFile, ul, 0),
            .rss_shmem  = MAPPING_INT(RssShmem, ul, 0),
#endif
            .vm_data    = MAPPING_INT(VmData,ul, 0),
            .vm_stk     = MAPPING_INT(VmStk, ul, 0),
            .vm_exe     = MAPPING_INT(VmExe, ul, 0),
            .vm_lib     = MAPPING_INT(VmLib, ul, 0),
            .vm_pte     = MAPPING_INT(VmPTE, ul, 0),
            .vm_swap    = MAPPING_INT(VmSwap, ul, 0),

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0)
            .hugetlb_pages = MAPPING_INT(HugetlbPages, ul, 0),
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 15, 0)
            .core_dumping = MAPPING_INT(CoreDumping, i, 0),
#endif

            .threads    = MAPPING_INT(Threads, l, 0),

            .sigpnd     = MAPPING_HEX(SigPnd, ul, 0),
            .shdpnd     = MAPPING_HEX(ShdPnd, ul, 0),
            .sigblk     = MAPPING_HEX(SigBlk, ul, 0),
            .sigign     = MAPPING_HEX(SigIgn, ul, 0),
            .sigcgt     = MAPPING_HEX(SigCgt, ul, 0),

            .capinh     = MAPPING_HEX(CapInh, ul, 0),
            .capprm     = MAPPING_HEX(CapPrm, ul, 0),
            .capeff     = MAPPING_HEX(CapEff, ul, 0),
            .capbnd     = MAPPING_HEX(CapBnd, ul, 0),
            .capamb     = MAPPING_HEX(CapAmb, ul, 0),
        };
        // clang-format on

#undef MAPPING_HEX
#undef MAPPING_INT
#undef MAPPING_STR
    }

    // /proc/[pid]/environ
    std::string parse_environ(uint64_t pid) { return parse_environ(std::to_string(pid)); }

    std::string parse_environ(const std::string& pid) { return file_read("/proc/" + pid + "/environ"); }

    // /proc/[pid]/cmdline
    std::string parse_cmdline(uint64_t pid) { return parse_cmdline(std::to_string(pid)); }

    std::string parse_cmdline(const std::string& pid) { return file_read("/proc/" + pid + "/cmdline"); }

    // /proc/[pid]/comm
    std::string parse_comm(uint64_t pid) { return parse_comm(std::to_string(pid)); }

    std::string parse_comm(const std::string& pid)
    {
        return probe::util::trim(file_read("/proc/" + pid + "/comm"));
    }
} // namespace probe::process

#endif