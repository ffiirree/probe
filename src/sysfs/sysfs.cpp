#ifdef __linux__
#include "probe/sysfs.h"

#include <regex>

namespace probe::sys
{
    std::vector<std::string> buses()
    {
        std::vector<std::string> ret{};

        std::string path = "/sys/bus";

        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            ret.emplace_back(entry.path().filename());
        }
        return ret;
    }

    std::vector<std::tuple<std::string, std::filesystem::path, std::filesystem::path>>
    devices_by_class(const std::string& cls)
    {
        std::string path = "/sys/class/" + cls;

        if (!std::filesystem::exists(path)) return {};

        std::vector<std::tuple<std::string, std::filesystem::path, std::filesystem::path>> ret;
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            std::string name = entry.path().filename();
            std::filesystem::path device_path{}, driver_path{};
            if (std::filesystem::exists(entry.path() / "device")) {
                device_path = std::filesystem::canonical(entry.path() / "device");
            }

            if (std::filesystem::exists(entry.path() / "driver")) {
                driver_path = std::filesystem::canonical(entry.path() / "driver");
            }

            ret.emplace_back(name, device_path, driver_path);
        }

        return ret;
    }

    std::pair<std::filesystem::path, std::filesystem::path> device_by_class(const std::string& cls,
                                                                            const std::string& name)
    {

        std::filesystem::path dir = "/sys/class/" + cls + "/" + name;

        if (!std::filesystem::exists(dir)) return {};

        std::string device_path{}, driver_path{};
        if (std::filesystem::exists(dir / "device")) {
            device_path = std::filesystem::canonical(dir / "device");
        }
        if (std::filesystem::exists(dir / "device" / "driver")) {
            driver_path = std::filesystem::canonical(dir / "device" / "driver");
        }

        return { device_path, driver_path };
    }

    std::string guess_bus(const std::string& path)
    {
        std::smatch matchs;
        if (std::regex_match(path, matchs, std::regex("^/sys/bus/(\\w+)/drivers/[\\w/]+"))) {
            if (matchs.size() == 2) {
                return matchs[1].str();
            }
        }
        return {};
    }
} // namespace probe::sys

#endif