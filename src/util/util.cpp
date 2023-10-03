#include "probe/util.h"

#include "probe/system.h"

#include <algorithm>
#include <fstream>
#include <ranges>
#include <regex>
#include <sstream>

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

    std::string trim(const std::string& str)
    {
        auto lpos = str.find_first_not_of(whitespace);
        auto rpos = str.find_last_not_of(whitespace);
        if (lpos != std::string::npos && rpos != std::string::npos && rpos >= lpos) {
            return str.substr(lpos, rpos - lpos + 1);
        }
        // empty or all spaces
        return {};
    }

    std::string toupper(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(),
                       [](unsigned char ch) { return std::toupper(ch); });
        return str;
    }

    std::string tolower(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(),
                       [](unsigned char ch) { return std::tolower(ch); });
        return str;
    }

    std::string fread(const std::string& file)
    {
        std::ifstream fd(file);

        if (!fd) return {};

        std::stringstream buffer;
        buffer << fd.rdbuf();

        return buffer.str();
    }

    void fread(const std::string& file, const std::function<bool(const std::string&)>& callback)
    {
        std::ifstream fd(file);

        if (fd) {
            for (std::string line; std::getline(fd, line);) {
                if (!callback(line)) break;
            }
        }
    }

    std::optional<int32_t> to_32i(const std::string& str, int base) noexcept
    {
        try {
            return static_cast<int32_t>(std::stol(str, nullptr, base));
        }
        catch (...) {
            return std::nullopt;
        }
    }

    std::optional<uint32_t> to_32u(const std::string& str, int base) noexcept
    {
        try {
            return static_cast<uint32_t>(std::stoul(str, nullptr, base));
        }
        catch (...) {
            return std::nullopt;
        }
    }

    std::optional<int64_t> to_64i(const std::string& str, int base) noexcept
    {
        try {
            return static_cast<int64_t>(std::stoll(str, nullptr, base));
        }
        catch (...) {
            return std::nullopt;
        }
    }

    std::optional<uint64_t> to_64u(const std::string& str, int base) noexcept
    {
        try {
            return static_cast<uint64_t>(std::stoull(str, nullptr, base));
        }
        catch (...) {
            return std::nullopt;
        }
    }

    std::optional<bool> to_bool(const std::string& str) noexcept
    {
        return (std::regex_match(str, std::regex("1|on|true", std::regex_constants::icase)));
    }

    std::string env(const std::string& name)
    {
        auto value = std::getenv(name.c_str());
        return value ? value : "";
    }
} // namespace probe::util
