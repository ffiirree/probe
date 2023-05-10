#include "probe/process.h"

namespace probe
{
    std::string to_string(process::pstate_t state)
    {
        switch (state) {
        case process::pstate_t::Running: return "Running";
        case process::pstate_t::Sleeping: return "Sleeping";
        case process::pstate_t::DiskSleep: return "DiskSleep";
        case process::pstate_t::Zombie: return "Zombie";
        case process::pstate_t::Stopped: return "Stopped";
        case process::pstate_t::TracingStop: return "TracingStop";
        case process::pstate_t::Dead: return "Dead";

        default: return "Unknown";
        }
    }
    char to_char(process::pstate_t state)
    {
        switch (state) {
        case process::pstate_t::Running: return 'R';
        case process::pstate_t::Sleeping: return 'S';
        case process::pstate_t::DiskSleep: return 'D';
        case process::pstate_t::Zombie: return 'Z';
        case process::pstate_t::Stopped: return 'T';
        case process::pstate_t::TracingStop: return 't';
        case process::pstate_t::Dead: return 'X';

        default: return '\0';
        }
    }
} // namespace probe