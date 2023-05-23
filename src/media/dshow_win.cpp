#ifdef _WIN32

#include "probe/defer.h"
#include "probe/media.h"
#include "probe/util.h"

#include <dshow.h>
#include <windows.h>
#include <winrt/base.h>

#define RETURN_NONE_IF_FAILED(HR)                                                                          \
    if (FAILED(HR)) return {};

namespace probe::media
{
    static std::optional<device_t> DisplayDeviceInformation(const winrt::com_ptr<IMoniker>& moniker)
    {
        winrt::com_ptr<IPropertyBag> props{};
        if (FAILED(moniker->BindToStorage(nullptr, nullptr, winrt::guid_of<IPropertyBag>(),
                                          props.put_void()))) {
            return std::nullopt;
        }

        VARIANT var;
        VariantInit(&var);
        defer(VariantClear(&var));

        std::wstring name{}, desc{}, id{};

        // Get description or friendly name.
        if (SUCCEEDED(props->Read(L"Description", &var, 0))) {
            desc = var.bstrVal;
        }
        if (SUCCEEDED(props->Read(L"FriendlyName", &var, 0))) {
            name = var.bstrVal;
        }
        if (SUCCEEDED(props->Read(L"DevicePath", &var, 0))) {
            // The device path is not intended for display.
            id = var.bstrVal;
        }

        return device_t{
            .name        = probe::util::to_utf8(name),
            .id          = probe::util::to_utf8(id),
            .description = probe::util::to_utf8(desc),
            .type        = device_type_t::video | device_type_t::source,
        };
    }

    std::vector<device_t> cameras()
    {
        winrt::init_apartment();
        defer(winrt::uninit_apartment());

        winrt::com_ptr<ICreateDevEnum> dev_enum{};
        RETURN_NONE_IF_FAILED(CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER,
                                               winrt::guid_of<ICreateDevEnum>(), dev_enum.put_void()));

        // Create an enumerator for the category.
        winrt::com_ptr<IEnumMoniker> enum_moniker{};
        RETURN_NONE_IF_FAILED(
            dev_enum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, enum_moniker.put(), 0));

        winrt::com_ptr<IMoniker> moniker{};
        std::vector<device_t> list;
        while (enum_moniker->Next(1, moniker.put(), nullptr) == S_OK) {
            if (auto dev = DisplayDeviceInformation(moniker); dev) {
                list.emplace_back(dev.value());
            }
        }

        return list;
    }
} // namespace probe::media

#endif // !_WIN32