#ifdef _WIN32

#include "probe/library.h"
#include "probe/util.h"

#include <Windows.h>

namespace probe::library
{
    library_t::~library_t()
    {
        if (handle) {
            ::FreeLibrary(reinterpret_cast<HMODULE>(handle));
            handle = nullptr;
        }
    }

    std::shared_ptr<library_t> load(const std::string& name)
    {
        auto handle = ::LoadLibrary(probe::util::to_utf16(name).c_str());
        return handle ? std::make_shared<library_t>(name, handle) : nullptr;
    }

    void *address_of(const std::shared_ptr<library_t>& lib, const std::string& sym)
    {
        if (!lib || !lib->handle) return nullptr;
        return reinterpret_cast<void *>(
            ::GetProcAddress(reinterpret_cast<HMODULE>(lib->handle), sym.c_str()));
    }
} // namespace probe::library

#endif