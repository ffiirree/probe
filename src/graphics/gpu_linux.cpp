#ifdef __linux__

#include "probe/graphics.h"

namespace probe::graphics
{
    std::vector<gpu_info_t> info() { return {}; }
} // namespace probe::graphics

#endif // __linux__