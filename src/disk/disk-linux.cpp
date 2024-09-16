#ifdef __linux__

#include "probe/disk.h"
#include "probe/sysfs.h"
#include "probe/util.h"

#include <bitset>
#include <filesystem>

namespace probe::disk
{
    std::vector<drive_t> physical_drives()
    {
        std::vector<drive_t> drives{};

        for (const auto& entry : std::filesystem::directory_iterator("/sys/block")) {
            if (!std::filesystem::exists(entry.path() / "device")) continue;

            auto name             = "/dev" / entry.path().filename();
            auto [device, driver] = probe::sys::device_by_class("block", entry.path().filename());
            auto bus              = probe::bus_cast(probe::sys::guess_bus(driver));

            std::string product{}, vendor{}, serial{};

            // PCI bus, vendor & product
            if (std::filesystem::exists(device / "serial")) {
                serial = probe::util::fread(device / "serial");
            }

            if (std::filesystem::exists(device / "model")) {
                product = probe::util::fread(device / "model");
            }

            if (bus == bus_type_t::PCI) {
                auto devpath = device;
                if (std::filesystem::is_directory(device / "device")) {
                    devpath /= "device";
                }
                if (std::filesystem::exists(devpath / "vendor")) {
                    auto vid = probe::util::to_32u(probe::util::fread(devpath / "vendor"), 16).value_or(0);
                    vendor   = vendor_cast(static_cast<vendor_t>(vid));

                    if (product.empty() && std::filesystem::exists(devpath / "device")) {
                        auto pid =
                            probe::util::to_32u(probe::util::fread(devpath / "device"), 16).value_or(0);
                        product = probe::product_name(vid, pid);
                    }
                }
            }
            // SCSI ...
            else {
                if (std::filesystem::exists(device / "vendor")) {
                    vendor = probe::util::fread(device / "vendor");
                }
            }

            drives.emplace_back(drive_t{
                .name    = name,
                .path    = device,
                .bus     = bus,
                .serial  = probe::util::trim(serial),
                .vendor  = probe::util::trim(vendor),
                .product = probe::util::trim(product),
            });
        }

        return drives;
    }

    std::vector<partition_t> partitions(const drive_t&)
    {
        std::vector<partition_t> ret;
        return ret;
    }

    std::vector<volume_t> volumes()
    {
        std::vector<volume_t> ret;

        return ret;
    }
} // namespace probe::disk

#endif