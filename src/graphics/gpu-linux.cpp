#ifdef __linux__

#include "probe/graphics.h"
#include "probe/sysfs.h"

namespace probe::graphics
{
    std::vector<gpu_info_t> info()
    {
        std::vector<gpu_info_t> ret;
        auto                    devices = probe::sys::pci_devices(0x03'0000);
        for (const auto& dev : devices) {
            ret.emplace_back(gpu_info_t{
                .name   = probe::product_name(dev.vendor_id, dev.product_id),
                .vendor = vendor_cast(dev.vendor_id),
            });
        }
        return ret;
    }
} // namespace probe::graphics

#endif // __linux__