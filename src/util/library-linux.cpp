#ifdef __linux__

#include "probe/library.h"

#include <dlfcn.h>

namespace probe::library
{
    library_t::~library_t()
    {
        if (handle) {
            ::dlclose(handle);
            handle = nullptr;
        }
    }

    std::shared_ptr<library_t> load(const std::string& name)
    {
        auto handle = ::dlopen(name.c_str(), RTLD_NOW);
        return handle ? std::make_shared<library_t>(name, handle) : nullptr;
    }

    void *address_of(const std::shared_ptr<library_t>& lib, const std::string& sym)
    {
        if (!lib || !lib->handle) return nullptr;
        return ::dlsym(lib->handle, sym.c_str());
    }
} // namespace probe::library

#endif