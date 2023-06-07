#ifdef __linux__

#include "probe/util.h"

namespace probe::util
{
    // TODO:
    std::string to_utf8(const wchar_t *wptr, size_t wlen)
    {
        if (!wptr) return {};

        if (wlen == 0) wlen = std::char_traits<wchar_t>::length(wptr);

        return {};
    }

    // TODO:
    std::wstring to_utf16(const char *, size_t) { return {}; }
} // namespace probe::util

#endif