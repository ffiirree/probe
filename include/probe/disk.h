#ifndef PROBE_DISK_H
#define PROBE_DISK_H

#include "probe/dllport.h"

#include <string>
#include <vector>

namespace probe
{
    namespace disk
    {
        enum class partition_style_t
        {
            MBR,
            GPT,
            RAW
        };

#ifdef _WIN32
        struct volume_t
        {
            std::string name;

            partition_style_t style;
        };

        struct drive_t
        {
            std::string name;
            std::string path;
            uint32_t number;
            // MEDIA_TYPE MediaType;
            uint64_t cylinders;
            uint32_t tracks_per_cylinder;
            uint32_t sectors_per_track;
            uint32_t bytes_per_sector;
        };

        PROBE_API std::vector<drive_t> physical_devices();
        PROBE_API std::vector<volume_t> volumes(const drive_t&);
#endif
    } // namespace disk
} // namespace probe

namespace probe
{
    PROBE_API std::string to_string(disk::partition_style_t);
} // namespace probe

#endif //! PROBE_DISK_H