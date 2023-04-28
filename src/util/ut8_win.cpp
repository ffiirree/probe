#ifdef _WIN32

#include "probe/defer.h"
#include "probe/util.h"

namespace probe::util
{
    // the null-terminating-character is guaranteed by std::string
    std::string to_utf8(const wchar_t *wptr, size_t wlen)
    {
        if (!wptr) return {};

        if (wlen == 0) wlen = std::char_traits<wchar_t>::length(wptr);

        // non-null character
        size_t mlen =
            WideCharToMultiByte(CP_UTF8, 0, wptr, static_cast<int>(wlen), nullptr, 0, nullptr, nullptr);

        std::string mstr(mlen, {});
        WideCharToMultiByte(CP_UTF8, 0, wptr, static_cast<int>(wlen), mstr.data(),
                            static_cast<int>(mstr.size()), nullptr, nullptr);

        return mstr;
    }

    std::wstring to_utf16(const char *mstr, size_t mlen)
    {
        if (!mstr) return {};

        if (mlen == 0) mlen = std::char_traits<char>::length(mstr);

        // non-null character
        size_t wlen = MultiByteToWideChar(CP_UTF8, 0, mstr, static_cast<int>(mlen), nullptr, 0);

        std::wstring wstr(wlen, {});
        MultiByteToWideChar(CP_UTF8, 0, mstr, static_cast<int>(mlen), wstr.data(),
                            static_cast<int>(wstr.size()));

        return wstr;
    }
} // namespace probe::util

#endif