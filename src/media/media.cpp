#include "probe/media.h"

namespace probe
{
    std::string to_string(media::device_type_t type)
    {
        std::string str{};

        switch (type & static_cast<media::device_type_t>(0x00f0)) {
        case media::device_type_t::audio: str += "Audio"; break;
        case media::device_type_t::video: str += "Video"; break;
        default:                          break;
        }

        switch (type & static_cast<media::device_type_t>(0x000f)) {
        case media::device_type_t::sink:   str += "Sink"; break;
        case media::device_type_t::source: str += "Source"; break;
        default:                           break;
        }

        switch (type & static_cast<media::device_type_t>(0x0f00)) {
        case media::device_type_t::monitor: str += "(Monitor)"; break;
        default:                            break;
        }

        if (str.empty()) str = "Unknown";

        return str;
    }

    std::string to_string(media::device_state_t state)
    {
        switch (state) {
        case media::device_state_t::active:     return "Active";
        case media::device_state_t::disabled:   return "Disabled";
        case media::device_state_t::notpresent: return "NotPresent";
        case media::device_state_t::unplugged:  return "Unplugged";

        case media::device_state_t::invalid:
        default:                                return "Invalid";
        }
    }
} // namespace probe
