#include "probe/graphics.h"

#include <array>
#include <cstring>
#include <numeric>

namespace probe::graphics
{
    // VESA E-EDID Standard Release A2 (EDID 1.4)
    // https://en.wikipedia.org/wiki/Extended_Display_Identification_Data
    // https://glenwing.github.io/docs/VESA-EEDID-A2.pdf
    int parse_edid(const std::array<char, 256>& arr, edid_t *edid)
    {
        const char *data = arr.data();

        // Checksum. Sum of all 128 bytes should equal 0 (mod 256).
        if(std::accumulate(data, data + 128, 0) % 256 != 0) return -1;

        // 0-7
        // check header
        const uint8_t header[8]{ 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00 };
        if(std::memcmp(data, header, 8) != 0) return -1;

        // 8-9
        // Manufacturer ID: big-endian 16-bit value made up of three 5-bit letters
        char mid[4]{
            static_cast<char>(((data[8] >> 2) & 0x001f) + 64),
            static_cast<char>(((((data[8]) & 0x0003) << 3) | ((data[9] >> 5) & 0x001f)) + 64),
            static_cast<char>((data[9] & 0x1f) + 64),
            0x00,
        };

        edid->manufacturer = mid;

        // 10-11
        // Manufacturer product code. 16-bit number, little-endian.
        edid->product_code = *reinterpret_cast<const uint16_t *>(&data[10]);

        // 12-15
        // Serial number. 32 bits, little-endian.
        edid->serial_number = *reinterpret_cast<const uint32_t *>(&data[12]);

        // 16
        // week ： 1-54
        if(data[16] <= 54) edid->week = data[16];

        // 17
        // year + 1990
        edid->year = data[17] + 1990;

        // 18-19
        edid->version  = data[18];
        edid->revision = data[19];

        // 21-22
        if(data[22] != 0x00) edid->hsize = data[21];
        if(data[21] != 0x00) edid->vsize = data[22];

        // 23
        // gamma:  datavalue = (gamma×100) − 100 = (gamma − 1)×100
        if(data[23] != char(0xff)) edid->gamma = data[23] / 100.0 + 1.0;

        return 0;
    }
} // namespace probe::graphics
