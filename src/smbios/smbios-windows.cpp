#ifdef _WIN32

#include "probe/smbios.h"

#include <optional>
#include <Windows.h>

struct RawSMBIOSData
{
    BYTE Used20CallingMethod;
    BYTE SMBIOSMajorVersion;
    BYTE SMBIOSMinorVersion;
    BYTE DmiRevision;
    DWORD Length;
    BYTE SMBIOSTableData[ANYSIZE_ARRAY];
};

namespace probe::smbios
{
    // https://www.dmtf.org/standards/smbios
    smbios_t smbios()
    {
        auto rsmb_size = ::GetSystemFirmwareTable('RSMB', 0, nullptr, 0);
        if (rsmb_size <= 0) return {};

        auto winrawsmb = reinterpret_cast<RawSMBIOSData *>(::HeapAlloc(::GetProcessHeap(), 0, rsmb_size));
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
        for (size_t i = 0; i < smb.data.size();) {
            smb.table.emplace_back(reinterpret_cast<smbios_header_t *>(&smb.data[i]));

            // next
            i += smb.data[i + 1];

            // Look for the end of the struct that must be terminated by \0\0
            while (i + 1 < smb.data.size()) {
                if (0 == smb.data[i] && 0 == smb.data[i + 1]) {
                    i += 2;
                    break;
                }
                ++i;
            }
        }

        return smb;
    }
} // namespace probe::smbios

#endif // _WIN32