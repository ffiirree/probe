#ifdef _WIN32

#include "probe/smbios.h"

#include <Windows.h>

struct RawSMBIOSData
{
    BYTE  Used20CallingMethod;
    BYTE  SMBIOSMajorVersion;
    BYTE  SMBIOSMinorVersion;
    BYTE  DmiRevision;
    DWORD Length;
    BYTE  SMBIOSTableData[ANYSIZE_ARRAY];
};

namespace probe::smbios
{
    // https://www.dmtf.org/standards/smbios
    smbios_t smbios()
    {
        const auto rsmb_size = ::GetSystemFirmwareTable('RSMB', 0, nullptr, 0);
        if (rsmb_size <= 0) return {};

        const auto winrawsmb = static_cast<RawSMBIOSData *>(::HeapAlloc(::GetProcessHeap(), 0, rsmb_size));
        if (winrawsmb == nullptr) return {};

        if (::GetSystemFirmwareTable('RSMB', 0, winrawsmb, rsmb_size) != rsmb_size) return {};

        smbios_t smb{};
        smb.version.major = winrawsmb->SMBIOSMajorVersion;
        smb.version.minor = winrawsmb->SMBIOSMinorVersion;
        smb.version.patch = winrawsmb->DmiRevision;
        smb.data.resize(winrawsmb->Length);
        ::memcpy(smb.data.data(), winrawsmb->SMBIOSTableData, winrawsmb->Length);

        ::HeapFree(::GetProcessHeap(), 0, winrawsmb);

        //
        for (size_t i = 0; i + sizeof(header_t) < smb.data.size();) {
            const auto header = reinterpret_cast<const header_t *>(&smb.data[i]);

            item_t item{ header->type, static_cast<uint8_t>(header->length - 4), header->handle };
            item.fields = &smb.data[i] + 4;

            // text strings
            i += header->length; // + item length

            // Look for the end of the struct that must be terminated by \0\0
            while (i + 1 < smb.data.size()) {
                const auto len = std::strlen(reinterpret_cast<const char *>(smb.data.data() + i));
                if (len) {
                    item.strings.push_back(reinterpret_cast<const char *>(smb.data.data() + i));
                }

                i += len;
                if (0 == smb.data[i] && 0 == smb.data[i + 1]) {
                    i += 2;
                    break;
                }
                i += 1;
            }

            smb.table.push_back(item);
        }

        return smb;
    }
} // namespace probe::smbios

#endif // _WIN32