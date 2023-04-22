#include "probe/util.h"

#include "probe/system.h"

#include <algorithm>
#include <limits>

namespace probe::util
{
    std::string to_utf8(const std::wstring& wstr) { return to_utf8(wstr.c_str(), wstr.size()); }

    std::string to_utf8(const char *wstr, size_t size)
    {
        return to_utf8(reinterpret_cast<const wchar_t *>(wstr), size);
    }

    std::string to_utf8(const std::string& wstr)
    {
        // the length of wstr is not known, use null to detect the real length
        return to_utf8(reinterpret_cast<const wchar_t *>(wstr.c_str()), 0);
    }

    std::wstring to_utf16(const std::string& mstr) { return to_utf16(mstr.c_str(), mstr.size()); }
} // namespace probe::util
