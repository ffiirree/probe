#ifdef _WIN32

#include "probe/defer.h"
#include "probe/graphics.h"
#include "probe/util.h"

#include <DXGI.h>
#include <Windows.h>

namespace probe::graphics
{
    std::vector<gpu_info_t> info()
    {
        IDXGIFactory *factory = nullptr;
        if(FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)(&factory)))) {
            return {};
        }
        defer(if(factory != nullptr) {
            factory->Release();
            factory = nullptr;
        });

        std::vector<gpu_info_t> cards;

        UINT idx              = 0;
        IDXGIAdapter *adapter = nullptr;
        while(factory->EnumAdapters(idx, &adapter) != DXGI_ERROR_NOT_FOUND) {
            DXGI_ADAPTER_DESC desc{};
            adapter->GetDesc(&desc);

            cards.emplace_back(gpu_info_t{
                probe::util::to_utf8(desc.Description), vendor_cast(desc.VendorId),
                desc.DedicatedVideoMemory, desc.SharedSystemMemory,
                0 // TODO:
            });

            ++idx;
            adapter->Release();
        }
        return cards;
    }
} // namespace probe::graphics

#endif // _WIN32