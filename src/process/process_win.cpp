#ifdef _WIN32

#include "probe/defer.h"
#include "probe/process.h"
#include "probe/time.h"
#include "probe/util.h"

// clang-format off
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
// clang-format on

namespace probe::process
{
    std::vector<process_t> processes()
    {
        std::vector<process_t> ret;

        HANDLE snap{};
        if ((snap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) == INVALID_HANDLE_VALUE) return ret;
        defer(::CloseHandle(snap));

        PROCESSENTRY32 entry{ .dwSize = sizeof(PROCESSENTRY32) };

        if (!::Process32First(snap, &entry)) return ret;

        do {
            auto phandle =
                ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, entry.th32ProcessID);

            wchar_t path[MAX_PATH]{};
            uint64_t time{};
            std::string user{};
            if (phandle != nullptr) {
                // path
                if (::GetModuleFileNameEx(phandle, nullptr, path, MAX_PATH) == 0) {
                    path[0] = 0;
                }

                // times
                FILETIME ct{}, et{}, kt{}, ut{};
                if (::GetProcessTimes(phandle, &ct, &et, &kt, &ut) != 0) {
                    time = probe::time::to_time(ct);
                }

                // user
                HANDLE token{};
                if (::OpenProcessToken(phandle, TOKEN_QUERY, &token)) {
                    PTOKEN_USER ptu{};
                    DWORD size{};
                    ::GetTokenInformation(token, TokenUser, nullptr, 0, &size);
                    ptu = reinterpret_cast<PTOKEN_USER>(malloc(size));
                    defer(free(ptu));

                    if (::GetTokenInformation(token, TokenUser, ptu, size, &size)) {
                        SID_NAME_USE sidtype;
                        DWORD usize{}, dsize{};
                        ::LookupAccountSid(nullptr, ptu->User.Sid, nullptr, &usize, nullptr, &dsize,
                                           &sidtype);

                        auto uname = new wchar_t[usize];
                        auto dname = new wchar_t[dsize];
                        defer(delete[] uname; delete[] dname);

                        if (::LookupAccountSid(nullptr, ptu->User.Sid, uname, &usize, dname, &dsize,
                                               &sidtype)) {
                            user = probe::util::to_utf8(uname);
                        }
                    }
                }

                ::CloseHandle(phandle);
            }

            ret.emplace_back(process_t{
                .pid        = entry.th32ProcessID,
                .ppid       = entry.th32ParentProcessID,
                .priority   = entry.pcPriClassBase,
                .name       = probe::util::to_utf8(entry.szExeFile),
                .path       = probe::util::to_utf8(path),
                .starttime  = time,
                .nb_threads = entry.cntThreads,
                .user       = user,
            });
        } while (::Process32Next(snap, &entry));

        return ret;
    }

    std::vector<thread_t> threads(uint64_t pid)
    {
        HANDLE snap = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, static_cast<DWORD>(pid));
        if (snap == INVALID_HANDLE_VALUE) return {};
        defer(::CloseHandle(snap));

        THREADENTRY32 entry{ .dwSize = sizeof(THREADENTRY32) };

        if (!::Thread32First(snap, &entry)) return {};

        std::vector<thread_t> ret{};

        do {
            if (pid == entry.th32OwnerProcessID) {
                ret.emplace_back(thread_t{
                    .tid  = entry.th32ThreadID,
                    .name = probe::util::thread_get_name(entry.th32ThreadID),
                });
            }
        } while (::Thread32Next(snap, &entry));

        return ret;
    }

    uint64_t memory(uint64_t pid)
    {
        PROCESS_MEMORY_COUNTERS mem{};

        auto phandle =
            ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, static_cast<DWORD>(pid));
        if (nullptr == phandle) return 0;

        if (::GetProcessMemoryInfo(phandle, &mem, sizeof(mem))) {
            return mem.WorkingSetSize;
        }

        ::CloseHandle(phandle);
        return 0;
    }

} // namespace probe::process

#endif