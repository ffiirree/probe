#ifndef PROBE_MEDIA_H
#define PROBE_MEDIA_H

#include "probe/dllport.h"
#include "probe/enum.h"

#include <optional>
#include <string>
#include <vector>

namespace probe
{
    namespace media
    {
        enum class device_type_t
        {
            none    = 0x0000,
            unknown = none,
            source  = 0x0001,
            sink    = 0x0002,
            audio   = 0x0010,
            video   = 0x0020,
            monitor = 0x0100,

            data_flow_mask = 0x0f,

            ENABLE_BITMASK_OPERATORS()
        };

        enum class device_state_t
        {
            invalid    = 0x00,
            active     = 0x01,
            disabled   = 0x02,
            notpresent = 0x04,
            unplugged  = 0x08,
        };

        struct device_t
        {
            std::string name{};        // utf-8, linux: like 'UVC Camera (046d:081b)'
            std::string id{};          // utf-8, linux: like '/dev/video0'
            std::string description{}; // utf-8
            device_type_t type{};
            device_state_t state{};
        };

        PROBE_API std::vector<device_t> audio_sources();
        PROBE_API std::vector<device_t> audio_sinks();

        // id
        PROBE_API std::optional<device_t> audio_device_by_id(const std::string& id);

    } // namespace media
} // namespace probe

namespace probe
{
    PROBE_API std::string to_string(media::device_type_t);
    PROBE_API std::string to_string(media::device_state_t);
} // namespace probe

#endif //! PROBE_MEDIA_H