#ifdef __linux__

#include "probe/system.h"
#include "probe/util.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <unordered_map>

namespace probe::system
{
    theme_t theme()
    {
        if (desktop() == desktop_t::GNOME || desktop() == desktop_t::Unity) {

            auto color_scheme = probe::util::exec_sync(
                { "gsettings", "get", "org.gnome.desktop.interface", "color-scheme" });
            if (!color_scheme.empty()) {
                if (color_scheme[0].find("dark") != std::string::npos) {
                    return theme_t::dark;
                }
                if (color_scheme[0].find("light") != std::string::npos) {
                    return theme_t::light;
                }
            }

            auto gtk_theme =
                probe::util::exec_sync({ "gsettings", "get", "org.gnome.desktop.interface", "gtk-scheme" });
            if (!gtk_theme.empty()) {
                if (gtk_theme[0].find("dark") != std::string::npos) {
                    return theme_t::dark;
                }
                if (gtk_theme[0].find("light") != std::string::npos) {
                    return theme_t::light;
                }
            }
        }

        // TODO : other desktop env
        return theme_t::light;
    }

    std::string kernel_name() { return "Linux"; }

    version_t kernel_version()
    {
        utsname uts{};
        if (uname(&uts) == -1) {
            return {};
        }

        return to_version(uts.release);
    }

    kernel_info_t kernel_info() { return { kernel_name(), kernel_version() }; }

    static std::unordered_map<std::string, std::string> parse_kv(std::ifstream& fstream)
    {
        std::unordered_map<std::string, std::string> kvs{};

        for (std::string line; std::getline(fstream, line);) {
            auto pos = line.find('=');
            if (pos != std::string::npos) {
                auto value = line.substr(pos + 1);
                if (std::count(value.begin(), value.end(), '"') == 2) {
                    if (value[0] == value.back() && value.back() == '"') {
                        value = value.substr(1, value.size() - 2);
                    }
                }
                kvs.emplace(std::string(line.c_str(), pos), value);
            }
        }

        return kvs;
    }

    // https://gist.github.com/natefoo/814c5bf936922dad97ff
    version_t os_version()
    {
        version_t ver{};
        if (std::filesystem::exists("/etc/os-release")) {
            std::ifstream release("/etc/os-release");
            if (release && release.is_open()) {
                auto kvs = parse_kv(release);

                // version
                if (auto version = kvs.find("VERSION"); version != kvs.end()) {
                    ver = to_version(version->second);
                }
                else if (auto version_id = kvs.find("VERSION_ID"); version_id != kvs.end()) {
                    ver = to_version(version_id->second);
                }

                // codename
                if (auto codename = kvs.find("VERSION_CODENAME"); codename != kvs.end()) {
                    ver.codename = codename->second;
                }
            }
        }

        if (ver == version_t{} && std::filesystem::exists("/etc/lsb-release")) {
            std::ifstream release("/etc/lsb-release");
            if (release && release.is_open()) {
                auto kvs = parse_kv(release);

                if (auto version_id = kvs.find("DISTRIB_RELEASE"); version_id != kvs.end()) {
                    ver = to_version(version_id->second);
                }

                if (auto codename = kvs.find("DISTRIB_CODENAME"); codename != kvs.end()) {
                    ver.codename = codename->second;
                }
            }
        }

        return ver;
    }

    std::string os_name()
    {
        if (std::filesystem::exists("/etc/os-release")) {
            std::ifstream release("/etc/os-release");
            if (release && release.is_open()) {
                auto kvs = parse_kv(release);
                if (auto pretty_name = kvs.find("PRETTY_NAME"); pretty_name != kvs.end()) {
                    return pretty_name->second;
                }
                else if (auto name = kvs.find("NAME"); name != kvs.end()) {
                    return name->second;
                }
            }
        }
        else if (std::filesystem::exists("/etc/lsb-release")) {
            std::ifstream release("/etc/lsb-release");
            if (release && release.is_open()) {
                auto kvs = parse_kv(release);

                if (auto desc = kvs.find("DISTRIB_DESCRIPTION"); desc != kvs.end()) {
                    return desc->second;
                }
                else if (auto name = kvs.find("DISTRIB_ID"); name != kvs.end()) {
                    return name->second;
                }
            }
        }

        return "Linux";
    }

    os_info_t os_info() { return { os_name(), theme(), os_version() }; }

    std::string hostname()
    {
        char buffer[256]{};
        if (::gethostname(buffer, 256) == 0) {
            return buffer;
        }
        return {};
    }

    std::string username()
    {
        char buffer[256]{};
        if (::getlogin_r(buffer, 256) == 0) {
            return buffer;
        }
        return {};
    }
} // namespace probe::system

static probe::version_t gnome_version();
static probe::version_t cinnamon_version();

namespace probe::system
{
    desktop_t desktop()
    {
        const std::string de = std::getenv("XDG_CURRENT_DESKTOP");
        // GNOME
        if (std::regex_search(de, std::regex("gnome", std::regex_constants::icase))) {
            return desktop_t::GNOME;
        }
        // Unity
        if (std::regex_search(de, std::regex("unity", std::regex_constants::icase))) {
            return desktop_t::Unity;
        }
        // Cinnamon
        if (std::regex_search(de, std::regex("\\bcinnamon\\b", std::regex_constants::icase))) {
            return desktop_t::Cinnamon;
        }
        return desktop_t::Unknown;
    }

    version_t desktop_version()
    {
        switch (desktop()) {
        case desktop_t::Unity:
        case desktop_t::GNOME: return gnome_version();
        case desktop_t::Cinnamon: return cinnamon_version();
        // TODO:
        default: return {};
        }
    }
} // namespace probe::system

namespace probe::system
{
    // https://unix.stackexchange.com/questions/202891/how-to-know-whether-wayland-or-x11-is-being-used
    // XDG_SESSION_ID / XDG_SESSION_TYPE may be not set
    // loginctl show-session $(loginctl | grep $(whoami) | awk '{print $1}') -p Type

    window_system_t window_system()
    {
        auto whoami  = probe::system::username();
        auto ws_type = window_system_t::Unknown;

        probe::util::exec_sync({ "loginctl" }, [&](const std::string& line) -> bool {
            if (std::regex_search(line, std::regex(whoami))) {
                auto session = probe::util::trim(line);
                auto epos    = session.find_first_of(probe::util::whitespace);

                if (epos == std::string::npos) return false;

                auto sid = session.substr(0, epos);

                auto type =
                    probe::util::exec_sync({ "loginctl", "show-session", sid.c_str(), "-p", "Type" });

                if (type.empty()) return false;

                if (std::regex_search(type[0], std::regex("\\bx11\\b", std::regex_constants::icase)))
                    ws_type = window_system_t::X11;

                if (std::regex_search(type[0], std::regex("\\bwayland\\b", std::regex_constants::icase)))
                    ws_type = window_system_t::Wayland;

                return false;
            }
            return true;
        });

        return ws_type;
    }
} // namespace probe::system

// GNOME 4 : gnome-shell
// GNOME 3 : /usr/share/gnome/gnome-version.xml
// GNOME 2 : /usr/share/gnome-about/gnome-version.xml
static probe::version_t gnome_version()
{
    probe::version_t version{};
    std::string filename{};

    if (std::filesystem::exists("/usr/share/gnome/gnome-version.xml")) {
        filename = "/usr/share/gnome/gnome-version.xml";
    }
    else if (std::filesystem::exists("/usr/share/gnome-about/gnome-version.xml")) {
        filename = "/usr/share/gnome-about/gnome-version.xml";
    }

    if (!filename.empty()) {
        std::ifstream verfile(filename);
        if (verfile && verfile.is_open()) {
            for (std::string line{}; std::getline(verfile, line);) {
                std::smatch matchs;
                if (std::regex_match(line, matchs, std::regex("[ \t]*<platform>(\\d+)</platform>\\s*"))) {
                    if (matchs.size() == 2 && !matchs[1].str().empty()) {
                        version.major = std::stoi(matchs[1].str());
                    }
                }
                else if (std::regex_match(line, matchs, std::regex("[ \t]*<minor>(\\d+)</minor>\\s*"))) {
                    if (matchs.size() == 2 && !matchs[1].str().empty()) {
                        version.minor = std::stoi(matchs[1].str());
                    }
                }
                else if (std::regex_match(line, matchs, std::regex("[ \t]*<micro>(\\d+)</micro>\\s*"))) {
                    if (matchs.size() == 2 && !matchs[1].str().empty()) {
                        version.patch = std::stoi(matchs[1].str());
                    }
                }
            }
        }
    }
    else {
        auto ver = probe::util::exec_sync({ "gnome-shell", "--version" });
        if (!ver.empty()) {
            version = probe::to_version(ver[0]);
        }
    }

    return version;
}

static probe::version_t cinnamon_version()
{
    auto ver = probe::util::exec_sync({ "cinnamon", "--version" });
    if (!ver.empty()) {
        return probe::to_version(ver[0]);
    }
    return {};
}

#endif // __linux__