#ifdef _WIN32

#include "probe/defer.h"
#include "probe/media.h"
#include "probe/util.h"

#include <Audioclient.h>
#include <functiondiscoverykeys.h>
#include <mmdeviceapi.h>
#include <optional>
#include <propsys.h>
#include <Windows.h>
#include <winrt/base.h>

#define RETURN_NONE_IF_FAILED(HR)                                                                          \
    if (FAILED(HR)) return {};

namespace probe::media
{
    static std::optional<device_t> device_info(IMMDevice *dev)
    {
        if (!dev) return {};

        winrt::com_ptr<IPropertyStore> props = nullptr;
        LPWSTR id                            = nullptr;
        PROPVARIANT name                     = {};
        PROPVARIANT desc                     = {};

        // Initialize container for property value.
        PropVariantInit(&name);
        defer(PropVariantClear(&name));

        PropVariantInit(&desc);
        defer(PropVariantClear(&desc));

        // Get the endpoint ID string.
        RETURN_NONE_IF_FAILED(dev->GetId(&id));
        defer(CoTaskMemFree(id); id = nullptr);

        RETURN_NONE_IF_FAILED(dev->OpenPropertyStore(STGM_READ, props.put()));

        // Get the endpoint's friendly-name property.
        RETURN_NONE_IF_FAILED(props->GetValue(PKEY_DeviceInterface_FriendlyName, &name));

        // desc
        RETURN_NONE_IF_FAILED(props->GetValue(PKEY_Device_DeviceDesc, &desc));

        // type
        winrt::com_ptr<IMMEndpoint> endpoint{};
        dev->QueryInterface(winrt::guid_of<IMMEndpoint>(), endpoint.put_void());

        EDataFlow data_flow{};
        RETURN_NONE_IF_FAILED(endpoint->GetDataFlow(&data_flow));

        auto type = device_type_t::audio;
        // clang-format off
        switch (data_flow) {
        case eRender:   type |= device_type_t::sink;                            break;
        case eCapture:  type |= device_type_t::source;                          break;
        case eAll:      type |= device_type_t::source | device_type_t::sink;    break;
        default:                                                                break;
        }
        // clang-format on

        // state
        DWORD state{};
        RETURN_NONE_IF_FAILED(dev->GetState(&state));

        return device_t{
            .name        = probe::util::to_utf8(name.pwszVal),
            .id          = probe::util::to_utf8(id),
            .description = probe::util::to_utf8(desc.pwszVal),
            .type        = type,
            .state       = static_cast<device_state_t>(state),
        };
    }

    static std::vector<device_t> endpoints(device_type_t type)
    {
        winrt::com_ptr<IMMDeviceEnumerator> enumerator{};
        winrt::com_ptr<IMMDeviceCollection> collection{};
        std::vector<device_t> devices{};

        winrt::init_apartment();
        defer(winrt::uninit_apartment());

        RETURN_NONE_IF_FAILED(CoCreateInstance(winrt::guid_of<MMDeviceEnumerator>(), nullptr, CLSCTX_ALL,
                                               winrt::guid_of<IMMDeviceEnumerator>(),
                                               enumerator.put_void()));

        RETURN_NONE_IF_FAILED(enumerator->EnumAudioEndpoints(
            any(type & device_type_t::source) ? eCapture : eRender, DEVICE_STATE_ACTIVE, collection.put()));

        UINT count = 0;
        RETURN_NONE_IF_FAILED(collection->GetCount(&count));
        for (ULONG i = 0; i < count; i++) {
            winrt::com_ptr<IMMDevice> endpoint{};
            RETURN_NONE_IF_FAILED(collection->Item(i, endpoint.put()));

            auto dev = device_info(endpoint.get());
            if (dev.has_value()) {
                devices.emplace_back(dev.value());
            }
        }

        return devices;
    }

    std::vector<device_t> audio_sources() { return endpoints(device_type_t::source); }

    std::vector<device_t> audio_sinks() { return endpoints(device_type_t::sink); }

    std::optional<device_t> audio_device_by_id(const std::string& id)
    {
        winrt::com_ptr<IMMDeviceEnumerator> enumerator{};

        winrt::init_apartment();
        defer(winrt::uninit_apartment());

        RETURN_NONE_IF_FAILED(CoCreateInstance(winrt::guid_of<MMDeviceEnumerator>(), nullptr, CLSCTX_ALL,
                                               winrt::guid_of<IMMDeviceEnumerator>(),
                                               enumerator.put_void()));

        winrt::com_ptr<IMMDevice> device{};
        RETURN_NONE_IF_FAILED(enumerator->GetDevice(probe::util::to_utf16(id).c_str(), device.put()));

        return device_info(device.get());
    }
} // namespace probe::media

#endif