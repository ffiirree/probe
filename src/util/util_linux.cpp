#ifdef __linux__

#include "probe/util.h"

#include <chrono>
#include <pthread.h>

using namespace std::chrono_literals;

namespace probe::util
{
    std::optional<std::string> exec(const char *cmd)
    {
        char buffer[128];
        std::string result{};

        FILE *pipe = popen(cmd, "r");

        if(!pipe) return std::nullopt;

        while(fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }

        pclose(pipe);
        return result;
    }
} // namespace probe::util

namespace probe::util
{
    int thread_set_name(const std::string& name)
    {
        return pthread_setname_np(pthread_self(), name.substr(0, 15).c_str());
    }

    std::string thread_get_name()
    {
        char buffer[128];
        pthread_getname_np(pthread_self(), buffer, 128);
        return buffer;
    }
} // namespace probe::util

#endif // __linux__