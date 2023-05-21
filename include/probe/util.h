#ifndef PROBE_UTIL_H
#define PROBE_UTIL_H

#include <atomic>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "probe/dllport.h"
#include "probe/types.h"

namespace probe::util
{
    PROBE_API std::string to_utf8(const wchar_t *, size_t = 0);
    PROBE_API std::string to_utf8(const std::wstring&);

    // must has a null-terminating-character if the size_t == 0
    PROBE_API std::string to_utf8(const char *, size_t = 0);
    PROBE_API std::string to_utf8(const std::string&);

    PROBE_API std::wstring to_utf16(const std::string&);
    PROBE_API std::wstring to_utf16(const char *, size_t = 0);

    // trim a string
    inline constexpr auto whitespace = " \f\n\r\t\v";
    PROBE_API std::string trim(const std::string&);

    PROBE_API inline double KB(uint64_t v) { return (static_cast<double>(v) / (1'024)); }

    PROBE_API inline double MB(uint64_t v) { return (static_cast<double>(v) / (1'024 * 1'024)); }

    PROBE_API inline double GB(uint64_t v) { return (static_cast<double>(v) / (1'024 * 1'024 * 1'024)); }
} // namespace probe::util

// windows registry
#ifdef _WIN32
namespace probe::util
{
    namespace registry
    {
        template<typename T> std::optional<T> read(HKEY key, const std::string&, const std::string&);

        template<>
        PROBE_API std::optional<std::string> read<std::string>(HKEY, const std::string&,
                                                               const std::string&);

        template<> PROBE_API std::optional<DWORD> read<DWORD>(HKEY, const std::string&, const std::string&);

        class RegistryListener : public Listener
        {
        public:
            PROBE_API ~RegistryListener() override { stop(); }

            PROBE_API int listen(const std::any&, const std::function<void(const std::any&)>&) override;
            PROBE_API void stop() override;

            PROBE_API bool running() override { return running_; }

        private:
            HKEY key_;
            HANDLE STOP_EVENT{ nullptr };
            HANDLE NOTIFY_EVENT{ nullptr };
            std::thread thread_;
            std::atomic<bool> running_{ false };
        };
    } // namespace registry
} // namespace probe::util
#endif

// linux
#ifdef __linux__
namespace probe::util
{
    PROBE_API std::pair<FILE *, pid_t> pipe_open(std::vector<const char *>);
    PROBE_API void pipe_close(std::pair<FILE *, pid_t>);

    PROBE_API std::vector<std::string> exec_sync(const std::vector<const char *>&);

    PROBE_API void exec_sync(const std::vector<const char *>&,
                             const std::function<bool(const std::string&)>&);

    class PipeListener : public Listener
    {
    public:
        PROBE_API ~PipeListener() override { stop(); }

        PROBE_API int listen(const std::any&, const std::function<void(const std::any&)>&) override;
        PROBE_API void stop() override;

        PROBE_API bool running() override { return running_; }

    private:
        std::pair<FILE *, pid_t> pipe_{};
        std::thread thread_;
        std::atomic<bool> running_{ false };
    };
} // namespace probe::util

// GNOME: gsettings
namespace probe::util::gsettings
{
    PROBE_API version_t version();

    // list installed schemas
    PROBE_API std::vector<std::string> list_schemas();

    // list keys in a schema
    PROBE_API std::vector<std::string> list_keys(const std::string&);

    // wheather contains a schema
    PROBE_API bool contains(const std::string&);

    // wheather contains a key
    PROBE_API bool contains(const std::string&, const std::string&);
} // namespace probe::util::gsettings
#endif
#endif //! PROBE_UTIL_H