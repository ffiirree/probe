#ifdef __linux__

#include "probe/cpu.h"
#include "probe/defer.h"
#include "probe/util.h"

#include <algorithm>
#include <cpuid.h>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <optional>
#include <regex>
#include <string>
#include <sys/utsname.h>
#include <unordered_set>

namespace probe::cpu
{
    static std::optional<std::string> cpuinfo_read_first_of(const char *key)
    {
        std::ifstream cpuinfo("/proc/cpuinfo");

        if (!cpuinfo.is_open() || !cpuinfo) return std::nullopt;

        for (std::string line; std::getline(cpuinfo, line);) {
            if (line.find(key) == 0) {
                const auto nonspace_id = line.find_first_not_of(" \t", line.find_first_of(':') + 1);
                return line.c_str() + nonspace_id;
            }
        }

        return std::nullopt;
    }

    static uint32_t cpuinfo_count_of(const char *key)
    {
        std::ifstream cpuinfo("/proc/cpuinfo");

        if (!cpuinfo.is_open() || !cpuinfo) return 0;

        uint32_t counter = 0;
        for (std::string line; std::getline(cpuinfo, line);) {
            if (line.find(key) == 0) {
                counter++;
            }
        }
        return counter;
    }

    static uint32_t cpuinfo_unique_count_of(const char *key)
    {
        std::ifstream cpuinfo("/proc/cpuinfo");

        if (!cpuinfo.is_open() || !cpuinfo) return 0;

        std::unordered_set<std::string> values{};
        for (std::string line; std::getline(cpuinfo, line);) {
            if (line.find(key) == 0) {
                const auto nonspace_id = line.find_first_not_of(" \t", line.find_first_of(':') + 1);
                values.insert(line.c_str() + nonspace_id);
            }
        }
        return values.size();
    }

    architecture_t architecture()
    {
        utsname buf{};
        if (uname(&buf) == -1) return cpu::architecture_t::unknown;

        if (!strcmp(buf.machine, "x86_64"))
            return cpu::architecture_t::x86_64;
        else if (strstr(buf.machine, "arm") == buf.machine)
            return cpu::architecture_t::arm;
        else if (!strcmp(buf.machine, "ia64") || !strcmp(buf.machine, "IA64"))
            return cpu::architecture_t::itanium;
        else if (!strcmp(buf.machine, "i686"))
            return cpu::architecture_t::x86;
        else
            return cpu::architecture_t::unknown;
    }

    uint64_t frequency()
    {
        return static_cast<uint64_t>(
            std::strtod(cpuinfo_read_first_of("cpu MHz").value_or("0").c_str(), nullptr) * 1'000'000);
    }

    // https://stackoverflow.com/questions/150355/programmatically-find-the-number-of-cores-on-a-machine
    quantities_t quantities()
    {
        cpu::quantities_t ret{};

        ret.logical  = cpuinfo_count_of("processor");
        ret.packages = cpuinfo_unique_count_of("physical id");
        ret.physical = ret.logical / ret.packages;

        return ret;
    }

    static std::optional<unsigned long> file_read_lu(const std::filesystem::path& path)
    {
        auto fd = ::fopen(path.c_str(), "r");
        unsigned long value;

        if (fd) {
            defer(::fclose(fd));

            if (1 == ::fscanf(fd, "%lu", &value)) {
                return value;
            }
        }
        return std::nullopt;
    }

    static std::string file_read(const std::string& file)
    {
        std::ifstream ifs(file);

        if (!ifs) return {};

        std::stringstream buffer;
        buffer << ifs.rdbuf();

        return buffer.str();
    }

    // /sys/devices/system/cpu/cpu<N>/cache/index<M>/<F>
    std::vector<cache_t> caches()
    {
        // cpus
        std::vector<std::filesystem::path> cpus{};
        for (const auto& entry : std::filesystem::directory_iterator("/sys/devices/system/cpu")) {
            auto dirname = entry.path().filename().string();
            if (std::regex_search(dirname, std::regex("\\bcpu[\\d]+"))) cpus.emplace_back(entry);
        }

        std::map<std::string, cache_t> caches;

        for (size_t i = 0; i < cpus.size(); ++i) {
            for (size_t idx = 0; idx < 8; ++idx) {

                std::string subdir = "cache/index" + std::to_string(idx);

                if (std::filesystem::exists(cpus[i] / subdir)) {
                    auto level = file_read_lu(cpus[i] / subdir / "level");
                    if (!level.has_value()) continue;

                    auto size_str = file_read(cpus[i] / subdir / "size");
                    if (size_str.empty()) continue;
                    auto size = std::stoul(size_str);
                    auto upos = size_str.find_first_not_of("0123456789 ");
                    if (upos != std::string::npos) {
                        if (size_str[upos] == 'K') size *= 1'024;
                        if (size_str[upos] == 'M') size *= 1'024 * 1'024;
                        if (size_str[upos] == 'G') size *= 1'024 * 1'024 * 1'024;
                    }

                    auto line_size = file_read_lu(cpus[i] / subdir / "coherency_line_size");
                    if (!line_size.has_value()) continue;

                    auto associativity = file_read_lu(cpus[i] / subdir / "ways_of_associativity");
                    if (!associativity.has_value()) continue;

                    auto id = file_read_lu(cpus[i] / subdir / "id");
                    if (!id.has_value()) continue;

                    // type
                    auto type_str = probe::util::trim(file_read(cpus[i] / subdir / "type"));
                    if (type_str.empty()) continue;
                    auto type = to_cache_type(type_str);

                    caches[type_str + std::to_string(((level.value() << 8) | id.value()))] = cache_t{
                        .level         = static_cast<int32_t>(level.value()),
                        .associativity = static_cast<int32_t>(associativity.value()),
                        .line_size     = line_size.value(),
                        .size          = size,
                        .type          = type,
                    };
                }
            }
        }

        std::vector<cache_t> ret;

        for (const auto& [k, v] : caches) {
            ret.emplace_back(std::move(v));
        }

        return ret;
    }

    std::vector<cache_t> cache(int l, cache_type_t t)
    {
        auto all = caches();
        std::vector<cache_t> ret;

        for (const auto& c : all) {
            if (c.level == l && c.type == t) {
                ret.emplace_back(c);
            }
        }

        return ret;
    }

    vendor_t vendor() { return vendor_cast(cpuinfo_read_first_of("vendor").value_or("")); }

    std::string name() { return cpuinfo_read_first_of("model name").value_or(""); }

    cpu_info_t info()
    {
        return { name(), vendor(), architecture(), endianness(), frequency(), quantities() };
    }

    void cpuid(int32_t (&info)[4], int32_t leaf, int32_t subleaf)
    {
        __cpuid_count(leaf, subleaf, info[0], info[1], info[2], info[3]);
    }
} // namespace probe::cpu

#endif // __linux__