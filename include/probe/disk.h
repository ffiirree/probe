#ifndef PROBE_DISK_H
#define PROBE_DISK_H

#include "probe/dllport.h"
#include "probe/types.h"

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
        struct drive_t
        {
            std::string name;
            std::string path;
            uint32_t number;
            std::string id; // GPT: GUID; MBR: Signature
            bus_type_t bus;
            bool removable;
            bool writable;
            bool trim;
            uint32_t partitions;
            partition_style_t style;
            std::string serial_number;
            std::string vendor_id;
            std::string product_id;
            uint64_t cylinders;
            uint32_t tracks_per_cylinder;
            uint32_t sectors_per_track;
            uint32_t bytes_per_sector;
        };

        struct partition_t
        {
            std::string name;
            uint32_t number;
            partition_style_t style;
            std::string type_id;
            std::string guid;
            uint64_t offset;
            uint64_t length;
        };

        struct volume_t
        {
            std::string letter;     // C:, D:, ...
            std::string label;      // volume label
            std::string serial;
            std::string path;
            std::string guid_path;  // "\\\\?\\Volume{803b42f7-bbee-4d30-ad22-2d0fe90072b6}\\"
            std::string filesystem; // NTFS, exFAT, ...
            uint64_t capacity;
            uint64_t free;
        };

        PROBE_API std::vector<drive_t> physical_drives();
        PROBE_API std::vector<partition_t> partitions(const drive_t&);
        PROBE_API std::vector<volume_t> volumes();
#endif
    } // namespace disk
} // namespace probe

namespace probe
{
    PROBE_API std::string to_string(disk::partition_style_t);
} // namespace probe

#endif //! PROBE_DISK_H