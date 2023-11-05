#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <regex>
#include <string>

enum state_t
{
    STATE_VENDOR = 0x0010,
    STATE_CLASS  = 0x0020,
};

enum level_t
{
    LEVEL_0 = 0x0000,
    LEVEL_1 = 0X0001,
    LEVEL_2 = 0x0002,
};

inline static bool isvalid(const std::string& str)
{
    return str.size() >= 6 && str[0] != '#' &&
           ((str[0] >= '0' && str[0] <= '9') || (str[0] >= 'a' && str[0] <= 'f') || (str[0] == '\t') ||
            (str[0] == 'C'));
}

static level_t getlevel(const std::string& str)
{
    if (str[0] != '\t') return LEVEL_0;
    if (str[0] == '\t' && str[1] != '\t') return LEVEL_1;
    if (str[0] == '\t' && str[1] == '\t') return LEVEL_2;

    return LEVEL_0;
}

static std::optional<std::pair<std::string, std::string>> parse_vendor(const std::string& str)
{
    std::smatch matches;
    if (std::regex_match(str, matches, std::regex("([\\d\\w]{4})[ |\t]+(.*)"))) {
        if (matches.size() == 3) {
            return std::pair{ matches[1], matches[2] };
        }
    }
    return std::nullopt;
}

static std::optional<std::pair<std::string, std::string>> parse_device(const std::string& str)
{
    std::smatch matches;
    if (std::regex_match(str, matches, std::regex("\t([\\d\\w]{4})[ |\t]+(.*)"))) {
        if (matches.size() == 3) {
            return std::pair{ matches[1], matches[2] };
        }
    }
    return std::nullopt;
}

#ifdef PARSE_SUBSYSTEMS
static std::optional<std::tuple<std::string, std::string, std::string>>
parse_subvendor(const std::string& str)
{
    std::smatch matches;
    if (std::regex_match(str, matches, std::regex("\t\t([\\d\\w]{4})[ |\t]+([\\d\\w]{4})[ |\t]+(.*)"))) {
        if (matches.size() == 4) {
            return std::tuple{ matches[1], matches[2], matches[3] };
        }
    }
    return std::nullopt;
}
#endif

int main(int argc, char *argv[])
{
    std::string path = (argc > 1) ? argv[1] : "../pciids/pci.ids";

    std::ifstream fd(path);

    if (!fd) {
        std::cerr << "can open the pciids file: " << path << ".\n";
        return -1;
    }

    std::map<std::string, std::string> vendors{};
    std::map<std::string, std::string> devices{};
    std::map<std::string, std::string> subsystems{};

    state_t     state = state_t::STATE_VENDOR;
    std::string vid{};
    std::string did{};
    for (std::string line{}; std::getline(fd, line);) {
        if (!isvalid(line)) continue;

        if (line[0] == 'C') state = STATE_CLASS;
        if (line[0] != 'C' && line[0] != '\t') state = STATE_VENDOR;

        switch (state | getlevel(line)) {
        case STATE_VENDOR | LEVEL_0: {
            did = {};

            auto kv = parse_vendor(line);
            if (!kv.has_value()) {
                vid = {};
                std::cerr << "error at : " << line << "\n";
                break;
            }
            vid                       = kv.value().first;
            vendors[kv.value().first] = kv.value().second;
            std::cout << kv.value().first << ": " << kv.value().second << "\n";
            break;
        }

        case STATE_VENDOR | LEVEL_1: {
            if (vid.empty()) break;

            auto kv = parse_device(line);
            if (!kv.has_value()) {
                std::cerr << "error at : " << line << "\n";
                break;
            }

            did                             = kv.value().first;
            devices[vid + kv.value().first] = kv.value().second;
            std::cout << "\t" << kv.value().first << ": " << kv.value().second << "\n";
            break;
        }
        case STATE_VENDOR | LEVEL_2: {
#ifdef PARSE_SUBSYSTEMS
            if (vid.empty() || did.empty()) break;

            auto kv = parse_subvendor(line);
            if (!kv.has_value()) {
                std::cerr << "error at : " << line << "\n";
                break;
            }

            auto [subvid, subdid, subdname]         = kv.value();
            subsystems[vid + did + subvid + subdid] = subdname;
            std::cout << "\t\t" << subvid << " " << subdid << " " << subdname << "\n";
#endif
            break;
        }

        // TODO:
        case STATE_CLASS | LEVEL_0: break;
        case STATE_CLASS | LEVEL_1: break;
        case STATE_CLASS | LEVEL_2: break;

        default:                    std::cerr << "unknown state"; break;
        }
    }

    // generate the "pciids.h" header
    std::ofstream pcih("pciids.h");
    if (!pcih) {
        std::cerr << "failed to generate the header file.";
        return -1;
    }

    pcih << "#ifndef PROBE_PCIIDS_H\n"
         << "#define PROBE_PCIIDS_H\n"
         << "\n"
         << "// Don't modify this file, which was generated by tools/pciids.\n"
         << "\n"
         << "// clang-format off\n";

    // vendors
    pcih << "#define PCIIDS_VENDORS \\\n";
    for (const auto& [k, v] : vendors) {
        pcih << "V(0x" << k << "," << std::quoted(v) << ")\\\n";
    }
    pcih << "\n";

    // devices
    pcih << "#define PCIIDS_DEVICES \\\n";
    for (const auto& [k, v] : devices) {
        pcih << "P(0x" << k << "," << std::quoted(v) << ")\\\n";
    }
    pcih << "\n";

    // subsystems
    pcih << "#define PCIIDS_SUBSYSTEMS \\\n";
#ifdef PARSE_SUBSYSTEMS
    for (const auto& [k, v] : subsystems) {
        pcih << "S(0x" << k << ", " << std::quoted(v) << ")\\\n";
    }
#endif

    pcih << "// clang-format on\n\n"
         << "#endif //! PROBE_PCIIDS_H"
         << "\n";

    return 0;
}