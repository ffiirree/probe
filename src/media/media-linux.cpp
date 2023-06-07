#ifdef __linux__

#include "probe/media.h"

namespace probe::media
{
    std::vector<device_t> audio_sources() { return {}; }

    std::vector<device_t> audio_sinks() { return {}; }

    std::optional<device_t> audio_device_by_id(const std::string&) { return std::nullopt; }

    std::vector<device_t> cameras() { return {}; }
} // namespace probe::media

#endif