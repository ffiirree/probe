#include "probe/disk.h"

#include "probe/util.h"

#include <iostream>

int main()
{
#ifdef _WIN32
    auto drives = probe::disk::physical_drives();

    std::cout << "Disk Drives: \n";
    for(auto& drive : drives) {
        auto partitions = probe::disk::partitions(drive);

        std::cout << "  " << (drive.name.empty() ? "(N/A)" : drive.name.substr(4)) << "\n"
                  << "    Number              : " << drive.number << '\n'
                  << "    Name                : " << drive.name << '\n'
                  << "    Path                : " << drive.path << '\n'
                  << "    ID                  : " << drive.id << '\n'
                  << "    Serial Number       : " << drive.serial_number << '\n'
                  << "    Vendor  ID          : " << drive.vendor_id << '\n'
                  << "    Product ID          : " << drive.product_id << '\n'
                  << "    Bus                 : " << probe::to_string(drive.bus) << '\n'
                  << "    Removable           : " << drive.removable << '\n'
                  << "    Writable            : " << drive.writable << '\n'
                  << "    Trim                : " << drive.trim << '\n'
                  << "    Cylinders           : " << drive.cylinders << '\n'
                  << "    Tracks / Cylinders  : " << drive.tracks_per_cylinder << '\n'
                  << "    Sectors / Track     : " << drive.sectors_per_track << '\n'
                  << "    Bytes / Sector      : " << drive.bytes_per_sector << '\n'
                  << "    Total Size          : "
                  << (drive.cylinders * drive.tracks_per_cylinder * drive.sectors_per_track *
                      drive.bytes_per_sector) /
                         static_cast<double>((1024 * 1024 * 1024))
                  << " GB\n"
                  << "    Style               : " << probe::to_string(drive.style) << '\n'
                  << "    Partitions          : " << drive.partitions << '\n';

        for(const auto& part : partitions) {
            std::cout << "      Parition " << part.number << '\n'
                      << "        Name            : " << part.name << '\n'
                      << "        Style           : " << probe::to_string(part.style) << '\n'
                      << "        Type ID         : " << part.type_id << '\n'
                      << "        GUID            : " << part.guid << '\n'
                      << "        Offset          : " << part.offset << '\n'
                      << "        Length          : " << part.length << '\n';
        }
        std::cout << '\n';
    }

    auto volumes = probe::disk::volumes();

    std::cout << "Volumes: \n";
    for(const auto& volume : volumes) {
        std::cout << "  " << (volume.letter.empty() ? "(N/A)" : volume.letter) << '\n'
                  << "    Label               : " << (volume.label.empty() ? "(N/A)" : volume.label) << '\n'
                  << "    Filesystem          : " << volume.filesystem << '\n'
                  << "    Serial Number       : " << volume.serial << '\n'
                  << "    GUID Path           : " << volume.guid_path << '\n'
                  << "    Capacity            : " << probe::util::GB(volume.capacity) << " GB\n"
                  << "    Free Space          : " << probe::util::GB(volume.free) << " GB\n";
    }
#endif
    return 0;
}