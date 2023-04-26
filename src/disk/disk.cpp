#include "probe/disk.h"

namespace probe
{
    std::string to_string(disk::partition_style_t style)
    {
        switch(style) {
        case disk::partition_style_t::MBR: return "MBR";
        case disk::partition_style_t::GPT: return "GPT";
        case disk::partition_style_t::RAW: return "RAW";
        default: return "unknown";
        }
    }
} // namespace probe