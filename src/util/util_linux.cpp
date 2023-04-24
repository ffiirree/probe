#ifdef __linux__

#include "probe/util.h"

#include <chrono>
#include <cstring>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

using namespace std::chrono_literals;

namespace probe::util
{
    std::pair<FILE *, pid_t> pipe_open(std::vector<const char *> cmd)
    {
        int pipefd[2]; // "r"+"w"

        if(::pipe(pipefd) < 0) {
            return { nullptr, -1 };
        }

        auto pid = ::fork();
        switch(pid) {
        case -1: return { nullptr, -1 };

        case 0: // child process
            // close read endpoint
            ::close(pipefd[0]);

            // redirect standrd output to the pipe
            ::dup2(pipefd[1], 1);

            // it has been duplicated, close the original descriptor we got from pipe()
            ::close(pipefd[1]);

            // execute command via shell
            // this will replace current process
            cmd.emplace_back(nullptr);
            ::execvp(cmd.at(0), const_cast<char *const *>(&cmd.at(0)));

            return { nullptr, 0 };

        default: // parent process
            // close write endpoint
            ::close(pipefd[1]);

            return { ::fdopen(pipefd[0], "r"), pid };
        }
    }

    void pipe_close(std::pair<FILE *, pid_t> pp)
    {
        if(pp.second > 0) {
            ::kill(pp.second, SIGTERM);
        }

        if(pp.first) {
            ::fclose(pp.first);
        }
    }

    std::vector<std::string> exec_sync(const std::vector<const char *>& cmd)
    {
        char buffer[4096];
        std::vector<std::string> ret{};

        auto pp = pipe_open(cmd);

        if(!pp.first) return ret;

        while(::fgets(buffer, sizeof(buffer), pp.first) != nullptr) {
            if(buffer[std::strlen(buffer) - 1] == '\n') buffer[std::strlen(buffer) - 1] = '\0';
            ret.emplace_back(buffer);
        }

        pipe_close(pp);
        return ret;
    }

    void exec_sync(const std::vector<const char *>& args,
                   const std::function<bool(const std::string&)>& callback)
    {
        auto pp = pipe_open(args);
        if(!pp.first) return;

        char buffer[4096];
        while(::fgets(buffer, sizeof(buffer), pp.first)) {
            if(buffer[std::strlen(buffer) - 1] == '\n') buffer[std::strlen(buffer) - 1] = '\0';
            if(!callback(buffer)) break;
        }

        pipe_close(pp);
    }
} // namespace probe::util

// PipeListener
namespace probe::util
{
    int PipeListener::listen(const std::any& obj, const std::function<void(const std::any&)>& callback)
    {
        auto cmd = std::any_cast<std::vector<const char *>>(obj);

        running_ = true;
        thread_  = std::thread([=, this]() {
            thread_set_name(std::string{ "listen-" } + cmd[0]);

            pipe_ = pipe_open(cmd);

            char buffer[256]{};
            while(running_ && ::fgets(buffer, sizeof(buffer), pipe_.first)) {
                callback(std::string{ buffer });
            }

            running_ = false;
        });

        return 0;
    }

    void PipeListener::stop()
    {
        running_ = false;
        pipe_close(pipe_);

        if(thread_.joinable()) thread_.join();
    }
} // namespace probe::util

// thread
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