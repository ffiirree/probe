#ifndef PROBE_PROCESS_H
#define PROBE_PROCESS_H

#include "probe/dllport.h"

#include <cstdint>
#include <string>
#include <vector>

#ifdef __linux__
#include <linux/version.h>
#include <pwd.h>
#endif

namespace probe::process
{
    // LINUX_VERSION_CODE > KERNEL_VERSION(3, 13, 0)
    enum class pstate_t
    {
        Running     = 'R', // Running
        Sleeping    = 'S', // Sleeping in an interruptible wait
        DiskSleep   = 'D', // Waiting in uninterruptible disk sleep
        Zombie      = 'Z', // Zombie
        Stopped     = 'T', // Stopped (on a signal) or (before Linux 2.6.33) trace stopped
        TracingStop = 't', // Tracing stop (Linux 2.6.33 onward)
        Dead        = 'X', // Dead (from Linux 2.6.0 onward)
    };

    struct process_t
    {
        int64_t pid{};
        int64_t ppid{}; // pid of parent process

        int state{};    // linux only
        long priority{};

        std::string name{};
        std::string path{};
        std::string cmdline{}; // linux only
        uint64_t starttime{};  // ns
        uint64_t nb_threads{};
        std::string user{};
    };

    struct thread_t
    {
        uint64_t tid{};
        std::string name{};
    };

    // probe all process
    PROBE_API std::vector<process_t> processes();

    // get pid of current process
    PROBE_API int64_t id();

    // get name by pid
    PROBE_API std::string name(uint64_t);

    // get path of the process
    PROBE_API std::string path(uint64_t);

    // get all threads of the process
    PROBE_API std::vector<thread_t> threads(uint64_t);

    // bytes
    PROBE_API uint64_t memory(uint64_t);
} // namespace probe::process

namespace probe::process
{
#ifdef __linux__
    struct pstat_t
    {
        int pid;          // The process ID.
        std::string comm; // The filename of the executable, in parentheses.
        char state;
        int ppid;         // The PID of the parent of this process.
        int pgrp;         // The process group ID of the process.
        int session;      // The session ID of the process.
        int tty_nr;       // The controlling terminal of the process.
        int tpgid; // The ID of the foreground process group of the controlling terminal of the process.
        unsigned int flags;   // The kernel flags word of the process.
        unsigned long minflt; // The number of minor faults the process has made which have not required
                              // loading a memory page from disk.
        unsigned long
            cminflt; // The number of minor faults that the process's waited-for children have made.
        unsigned long majflt; // The number of major faults the process has made which have required loading
                              // a memory page from disk.
        unsigned long
            cmajflt;         // The number of major faults that the process's waited-for children have made.
        unsigned long utime; // Amount of time that this process has been scheduled in user mode.
        unsigned long stime; // Amount of time that this process has been scheduled in kernel mode.
        long cutime; // Amount of time that this process's waited-for children have been scheduled in user
                     // mode
        long cstime; // Amount of time that this process's waited-for children have been scheduled in kernel
                     // mode
        long priority;
        long nice;   // The nice value, a value in the range 19 (low priority) to -20 (high priority).
        long nb_threads;
        unsigned long long starttime; // The time the process started after system boot.
        unsigned long vsize;          // Virtual memory size in bytes.
        long rss;                     // Resident Set Size: number of pages the process has in real memory.
        unsigned long rsslim;         // Current soft limit in bytes on the rss of the process;
        unsigned long startcode;      // The address above which program text can run.
        unsigned long endcode;        // The address below which program text can run.
        unsigned long startstack;     // The address of the start (i.e., bottom) of the stack.
        unsigned long kstkesp;    // The current value of ESP (stack pointer), as found in the kernel stack
                                  // page for the process.
        unsigned long kstkeip;    // The current EIP (instruction pointer).
        unsigned long wchan;      // This is the "channel" in which the process is waiting
        int exit_signal;          // Signal to be sent to parent when we die.
        int processor;            //  CPU number last executed on.
        unsigned int rt_priority; // Real-time scheduling priority
        unsigned int policy;      // Scheduling policy
        unsigned long long blkio_ticks; // Aggregated block I/O delays, measured in clock ticks
        unsigned long guest_time;       // Guest time of the process.
        long cguest_time;
    };

    struct pio_t
    {
        unsigned long rchar;
        unsigned long wchar;
        unsigned long syscr;
        unsigned long syscw;
        unsigned long read_bytes;
        unsigned long write_bytes;
        unsigned long cancelled_write_bytes;
    };

    // Provides information about memory usage,
    // measured in pages.
    struct pstatm_t
    {
        unsigned long size;     // total program size, (same as VmSize in /proc/[pid]/status)
        unsigned long resident; // resident set size (inaccurate; same as VmRSS in /proc/[pid]/status)
        unsigned long shared;   // number of resident shared pages  (inaccurate; same as RssFile+RssShmem in
                                // /proc/[pid]/status)
        unsigned long text;
        unsigned long data;     // data + stack
    };

    struct pstatus_t
    {
        std::string name;    // Command run by this process.
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 7, 0)
        unsigned long umask; // Process umask, expressed in octal with a leading zero;
#endif
        pstate_t state;
        int tgid;            // Thread group ID
        int ngid;            // NUMA group ID
        int pid;             // Thread ID
        int ppid;            // PID of parent process
        int tracer_pid;      // PID of process tracing this process

        uid_t ruid;          // Real, effective, saved set, and filesystem UIDs
        uid_t euid;
        uid_t suid;
        uid_t fuid;

        gid_t rgid;
        gid_t egid;
        gid_t sgid;
        gid_t fgid;

        long fd_size;       //  Number of file descriptor slots currently allocated.
        std::string groups; // Supplementary group list.

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0)
        int nstgid; // Thread group ID (i.e., PID) in each of the PID namespaces of which [pid] is a member
        int nspid;  // Thread ID in each of the PID namespaces of which [pid] is a member.
        int nspgid; // Process group ID in each of the PID namespaces of which [pid] is a member.
        int nssid;  // descendant namespace session ID hierarchy Session ID in each of the PID namespaces of
                    // which [pid] is a member.
#endif
        // KB
        unsigned long vm_peak; // Peak virtual memory size.
        unsigned long vm_size; // Virtual memory size.
        unsigned long vm_lck;  // Locked memory size
        unsigned long vm_pin;  // Pinned memory size
        unsigned long
            vm_hwm; // Peak resident set size. This value is inaccurate; see /proc/[pid]/statm above.
        unsigned long
            vm_rss; // Resident set size.  Note that the value here is the sum of RssAnon, RssFile, and
                    // RssShmem.  This value is inaccurate; see /proc/[pid]/statm above.
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0)
        unsigned long rss_anon;  // Size of resident anonymous memory.This value is inaccurate; see
                                 // /proc/[pid]/statm above.
        unsigned long rss_file;  // Size of resident file mappings. This value is inaccurate; see
                                 // /proc/[pid]/statm above.
        unsigned long rss_shmem; // Size of resident shared memory (includes System V shared memory,
                                 // mappings from tmpfs(5), and shared anonymous mappings).
#endif
        // Size of data, stack, and text segments.
        // This value is inaccurate; see /proc/[pid]/statm above.
        unsigned long vm_data;
        unsigned long vm_stk;
        unsigned long vm_exe;

        unsigned long vm_lib; // Shared library code size.
        unsigned long vm_pte; // Page table entries size
        // unsigned long vm_pwd;
        unsigned long
            vm_swap; // Swapped-out virtual memory size by anonymous private pages; shmem swap usage is not
                     // included. This value is inaccurate; see /proc/[pid]/statm above.
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0)
        unsigned long hugetlb_pages; // Size of hugetlb memory portions
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 15, 0)
        int core_dumping; // Contains the value 1 if the process is currently dumping core, and 0 if it is
                          // not. This information can be used by a monitoring process to avoid killing a
                          // process that is currently dumping core, which could result in a corrupted core
                          // dump file.
#endif
        long threads;

        uint64_t sigpnd; // Mask (hexadecimal) of signals pending for thread and for process as a whole
        uint64_t shdpnd; //
        uint64_t sigblk; // Masks (hexadecimal) indicating signals being blocked, ignored, and caught.
        uint64_t sigign; //
        uint64_t sigcgt; //

        uint64_t capinh;
        uint64_t capprm;
        uint64_t capeff;
        uint64_t capbnd;
        uint64_t capamb;
    };

    // proc/uptime
    // ns
    // return zero if failed
    PROBE_API uint64_t uptime();

    // /proc/[pid]/stat
    PROBE_API pstat_t parse_stat(uint64_t);
    PROBE_API pstat_t parse_stat(const std::string&);

    // /proc/[pid]/io
    PROBE_API pio_t parse_io(uint64_t);
    PROBE_API pio_t parse_io(const std::string&);

    // /proc/[pid]/statm
    PROBE_API pstatm_t parse_statm(uint64_t);
    PROBE_API pstatm_t parse_statm(const std::string&);

    // /proc/[pid]/status
    PROBE_API pstatus_t parse_status(uint64_t);
    PROBE_API pstatus_t parse_status(const std::string&);

    // /proc/[pid]/environ
    PROBE_API std::string parse_environ(uint64_t);
    PROBE_API std::string parse_environ(const std::string&);

    // /proc/[pid]/cmdline
    PROBE_API std::string parse_cmdline(uint64_t);
    PROBE_API std::string parse_cmdline(const std::string&);

    // /proc/[pid]/comm
    //  - This file exposes the process's comm value—that is, the command name associated with the process.
    PROBE_API std::string parse_comm(uint64_t);
    PROBE_API std::string parse_comm(const std::string&);
#endif
} // namespace probe::process

namespace probe
{
    PROBE_API std::string to_string(process::pstate_t);
    PROBE_API char to_char(process::pstate_t);
} // namespace probe

#endif //! PROBE_PROCESS_H