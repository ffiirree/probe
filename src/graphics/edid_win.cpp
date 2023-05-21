#ifdef _WIN32

#include "probe/graphics.h"
#include "probe/util.h"

#include <array>

namespace probe::graphics
{
    std::array<char, 256> edid_of(const std::string& name, const std::string& driver)
    {
        std::array<char, 256> arr{};

        HKEY key, subkey, edid_key;
        std::wstring key_str = LR"(SYSTEM\CurrentControlSet\Enum\DISPLAY\)" + probe::util::to_utf16(name);
        if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, key_str.c_str(), 0, KEY_READ, &key) == ERROR_SUCCESS) {
            DWORD subkey_size = MAX_PATH;
            FILETIME ft;
            WCHAR subkey_str[MAX_PATH];
            for (DWORD idx = 0;
                 !::RegEnumKeyEx(key, idx, subkey_str, &subkey_size, nullptr, nullptr, nullptr, &ft);
                 ++idx, subkey_size = MAX_PATH) {

                if (::RegOpenKeyEx(key, subkey_str, 0, KEY_READ, &subkey) == ERROR_SUCCESS) {
                    DWORD size = MAX_PATH;
                    if (::RegQueryValueEx(subkey, L"Driver", nullptr, nullptr,
                                          reinterpret_cast<LPBYTE>(subkey_str), &size) == ERROR_SUCCESS) {
                        if (probe::util::to_utf8(subkey_str) == driver) {
                            if (::RegOpenKeyEx(subkey, L"Device Parameters", 0, KEY_READ, &edid_key) ==
                                ERROR_SUCCESS) {
                                DWORD edid_size = 256;
                                if (::RegQueryValueEx(edid_key, L"EDID", nullptr, nullptr,
                                                      reinterpret_cast<LPBYTE>(arr.data()),
                                                      &edid_size) == ERROR_SUCCESS) {
                                    return arr;
                                }

                                ::RegCloseKey(edid_key);
                            }
                        } // check "Driver" == driver
                        ::RegCloseKey(subkey);
                    }
                }         // RegOpenKeyEx : subkey
            }
            ::RegCloseKey(key);
        }
        return arr;
    }
} // namespace probe::graphics

#endif