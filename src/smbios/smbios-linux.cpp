#ifdef __linux__

#include "probe/smbios.h"

namespace probe::smbios
{
    smbios_t smbios()
    {
        return {};
    }
} // namespace probe::smbios

#endif // __linux__