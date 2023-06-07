#ifdef _WIN32

#include "probe/graphics.h"
#include "probe/util.h"

#include <DXGI.h>
#include <Windows.h>
#include <winrt/base.h>

namespace probe::graphics
{
    std::vector<gpu_info_t> info()
    {
        winrt::com_ptr<IDXGIFactory1> factory{};
        if (FAILED(::CreateDXGIFactory1(winrt::guid_of<IDXGIFactory1>(), factory.put_void()))) {
            return {};
        }

        std::vector<gpu_info_t> cards;

        winrt::com_ptr<IDXGIAdapter1> adapter{};
        for (UINT idx = 0; factory->EnumAdapters1(idx, adapter.put()) != DXGI_ERROR_NOT_FOUND;
             ++idx, adapter = nullptr) {
            DXGI_ADAPTER_DESC1 desc{};
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
                // Microsoft Basic Render Driver
                continue;
            }

            cards.emplace_back(gpu_info_t{
                .name             = probe::util::to_utf8(desc.Description),
                .vendor           = vendor_cast(desc.VendorId),
                .dedicated_memory = desc.DedicatedVideoMemory,
                .shared_memory    = desc.SharedSystemMemory,
                .frequency        = 0, // TODO:
            });
        }
        return cards;
    }
} // namespace probe::graphics

#endif // _WIN32