#include "probe/disk.h"

#include "probe/util.h"

#include <iostream>

int main()
{
#ifdef _WIN32
    auto drives = probe::disk::physical_devices();

    std::cout << "Disk Drives: \n";
    for(auto& drive : drives) {
        auto volumes = probe::disk::volumes(drive);

        std::cout << "    Number              : " << drive.number << '\n'
                  << "    Name                : " << drive.name << '\n'
                  << "    Path                : " << drive.path << '\n'
                  << "    Serial Number       : " << drive.serial_number << '\n'
                  << "    VendorID            : " << drive.vendor_id << '\n'
                  << "    ProductID           : " << drive.product_id << '\n'
                  << "    Cylinders           : " << drive.cylinders << '\n'
                  << "    Tracks / Cylinders  : " << drive.tracks_per_cylinder << '\n'
                  << "    Sectors / Track     : " << drive.sectors_per_track << '\n'
                  << "    Bytes / Sector      : " << drive.bytes_per_sector << '\n'
                  << "    Size                : "
                  << (drive.cylinders * drive.tracks_per_cylinder * drive.sectors_per_track *
                      drive.bytes_per_sector) /
                         static_cast<double>((1024 * 1024 * 1024))
                  << " GB\n"
                  << "    Partitions         : " << '\n';

        for(const auto& volume : volumes) {
            std::cout << "       - Name : " << volume.name << ", Style : " << probe::to_string(volume.style)
                      << "\n";
        }
        std::cout << '\n';
    }
#endif
    return 0;
}