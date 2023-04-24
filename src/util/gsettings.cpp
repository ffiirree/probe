#ifdef __linux__

#include "probe/util.h"

namespace probe::util::gsettings
{
    PROBE_API version_t version()
    {
        auto ver = exec_sync({ "gsettings", "--version" });

        if(ver.empty()) return {};

        return to_version(ver[0]);
    }

    std::vector<std::string> list_schemas() { return exec_sync({ "gsettings", "list-schemas" }); }

    std::vector<std::string> list_keys(const std::string& schema)
    {
        return exec_sync({ "gsettings", "list-keys", schema.c_str() });
    }

    bool contains(const std::string& schema)
    {
        bool found = false;
        exec_sync({ "gsettings", "list-schemas" }, [&](const std::string& str) {
            found = (str == schema);
            return !found;
        });
        return found;
    }

    bool contains(const std::string& schema, const std::string& key)
    {
        bool found = false;
        exec_sync({ "gsettings", "list-keys", schema.c_str() }, [&](const std::string& str) {
            found = (str == key);
            return !found;
        });
        return found;
    }
} // namespace probe::util::gsettings

#endif