#ifndef PROBE_LIBRARY_H
#define PROBE_LIBRARY_H

#include "probe/dllport.h"

#include <memory>
#include <optional>
#include <string>

namespace probe::library
{
    struct library_t
    {
        std::string name{};
        void       *handle{};

        ~library_t();
    };

    PROBE_API std::shared_ptr<library_t> load(const std::string&);

    PROBE_API void *address_of(const std::shared_ptr<library_t>&, const std::string&);
} // namespace probe::library

#endif //! PROBE_LIBRARY_H