#ifdef _WIN32
#include "probe/library.h"

#include <Windows.h>

namespace probe::library
{
    library_t::~library_t()
    {
        handle = nullptr;
    }

    std::shared_ptr<library_t> load(const std::string& name)
    {
        return nullptr;
    }

    void *address_of(const std::shared_ptr<library_t>& lib, const std::string& sym)
    {
        return nullptr;
    }
} // namespace probe::library
#endif