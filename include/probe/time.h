#ifndef PROBE_TIME_H
#define PROBE_TIME_H

#include "probe/dllport.h"

#include <chrono>
#include <functional>
#include <thread>

#ifdef _WIN32
#include <winrt/base.h>
#endif

using namespace std::chrono_literals;

namespace probe::time
{
    // ns
    PROBE_API inline uint64_t system_time()
    {
        using namespace std::chrono;
        return duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();
    }

    /**
     * Get the current time in nanoseconds since some unspecified starting point.
     * On platforms that support it, the time comes from a monotonic clock
     * This property makes this time source ideal for measuring relative time.
     * The returned values may not be monotonic on platforms where a monotonic
     * clock is not available.
     */
    PROBE_API inline uint64_t relative_time()
    {
        using namespace std::chrono;
        return duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();
    }

    // sleep t s
    PROBE_API inline void sleep(uint64_t t) { std::this_thread::sleep_for(std::chrono::seconds(t)); }

    // sleep t ms
    PROBE_API inline void msleep(uint64_t t) { std::this_thread::sleep_for(std::chrono::milliseconds(t)); }

    // sleep t us
    PROBE_API inline void usleep(uint64_t t) { std::this_thread::sleep_for(std::chrono::microseconds(t)); }

#ifdef _WIN32
    // ns, could be used with system_time()
    PROBE_API inline uint64_t from_filetime(const winrt::file_time& ft)
    {
        using namespace std::chrono;
        // FILETIME <--> DataTime (time_point<winrt::clock>) <--> time_point<system_clock> <--> time_t
        auto datatime = winrt::clock::from_file_time(ft);
        auto sys_time = winrt::clock::to_sys(datatime).time_since_epoch(); // in 100ns
        return duration_cast<nanoseconds>(sys_time).count();
    }
#endif
} // namespace probe::time

namespace probe::time
{
    // timer
    struct timer
    {
        template<class Rep, class Period>
        explicit timer(const std::chrono::duration<Rep, Period>& interval,
                       const std::function<void()>& callback, bool single = false)
        {
            running_ = !single;
            thread_  = std::thread([=, this]() {
                do {
                    std::this_thread::sleep_for(interval);
                    callback();
                } while (running_);
            });
        }

        timer(const timer&)            = delete;
        timer(timer&&)                 = delete;
        timer& operator=(const timer&) = delete;
        timer& operator=(timer&&)      = delete;

        ~timer() { stop(); }

        void stop()
        {
            running_ = false;
            if (thread_.joinable()) thread_.join();
        }

    private:
        std::thread thread_{};
        std::atomic<bool> running_{ false };
    };
} // namespace probe::time

#endif //! PROBE_TIME_H