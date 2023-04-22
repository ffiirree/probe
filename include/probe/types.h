#ifndef PROBE_TYPES_H
#define PROBE_TYPES_H

#include "probe/dllport.h"

#include <string>

namespace probe
{
    struct version_t
    {
        uint32_t major{};
        uint32_t minor{};
        uint32_t patch{};
        uint32_t build{};

        std::string codename{};

        bool operator>=(const version_t&) const;
        bool operator<=(const version_t&) const;
        bool operator==(const version_t&) const;
    };

    enum class vendor_t
    {
        unknown   = 0x0000,
        NVIDIA    = 0x10de,
        Intel     = 0x8086,
        Microsoft = 0x1414,
        Qualcomm  = 0x17cb,
        AMD       = 0x1002,
        Apple     = 0x106b,
    };

    template<class O, class I> O vendor_cast(I i) { return static_cast<O>(i); }

    // cast uint32_t to vendor_t
    template<> PROBE_API vendor_t vendor_cast(uint32_t);

    // cast vendor_t to string
    template<> PROBE_API std::string vendor_cast(vendor_t);

    // gausses what vendor_t the string is
    template<> PROBE_API vendor_t vendor_cast(std::string_view name);

    PROBE_API std::string to_string(version_t);
} // namespace probe

#ifdef _WIN32
// windows versions
namespace probe
{
    // https://en.wikipedia.org/wiki/List_of_Microsoft_Windows_versions
    // windows 3.1, 1992-04-06
    inline const version_t WIN_3_1_1ST{ 3, 10, 102, 0, "Sparta" };

    // windows 95, 1995-08-24
    inline const version_t WIN_95_1ST{ 4, 0, 950, 0, "Chicago" };

    // windows 98, 1998-06-25
    inline const version_t WIN_98_1ST{ 4, 10, 1998, 0, "Memphis" };

    // windows 2000
    inline const version_t WIN_2000_1ST{ 5, 0, 2195, 0, "Janus" };

    // windows xp
    inline const version_t WIN_XP_1ST{ 5, 2, 2600, 0, "Whistler" };

    // windows vista
    inline const version_t WIN_VISTA_1ST{ 6, 0, 6000, 0, "Longhorn" };

    // windows 7
    inline const version_t WIN_7_1ST{ 6, 1, 7600, 0, "7" };

    // windows 8
    inline const version_t WIN_8_0_1ST{ 6, 2, 9200, 0, "8" };
    inline const version_t WIN_8_1_1ST{ 6, 3, 9600, 0, "Blue" };

    // windows 10
    inline const version_t WIN_10_1ST{ 10, 0, 10240, 16405, "1507" };
    inline const version_t WIN_10_1507{ 10, 0, 10240, 16405, "1507" };
    inline const version_t WIN_10_1511{ 10, 0, 10586, 3, "1511" };
    inline const version_t WIN_10_1607{ 10, 0, 14393, 10, "1607" };
    inline const version_t WIN_10_1703{ 10, 0, 15063, 138, "1703" };
    inline const version_t WIN_10_1709{ 10, 0, 16299, 19, "1709" };
    inline const version_t WIN_10_1803{ 10, 0, 17134, 48, "1803" };
    inline const version_t WIN_10_1809{ 10, 0, 17763, 1, "1809" };
    inline const version_t WIN_10_1903{ 10, 0, 18362, 116, "1903" };
    inline const version_t WIN_10_1909{ 10, 0, 18363, 476, "1909" };
    inline const version_t WIN_10_2004{ 10, 0, 19041, 264, "2004" };
    inline const version_t WIN_10_20H2{ 10, 0, 19042, 572, "20H2" };
    inline const version_t WIN_10_21H1{ 10, 0, 19043, 985, "21H1" };
    inline const version_t WIN_10_21H2{ 10, 0, 19044, 288, "21H2" };
    inline const version_t WIN_10_22H2{ 10, 0, 19045, 2130, "22H2" };

    // windows 11
    inline const version_t WIN_11_1ST{ 10, 0, 22000, 194, "21H2" };
    inline const version_t WIN_11_21H2{ 10, 0, 22000, 194, "21H2" };
    inline const version_t WIN_11_22H2{ 10, 0, 22621, 521, "22H2" };
} // namespace probe
#endif

#endif //! PROBE_TYPES_H