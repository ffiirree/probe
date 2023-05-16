<h2 align="center">Porbe</h2>

<p align="center">
  Hardware and OS information library for C++
</p>

## Usage

**Probe** contains several modules, including `system`, `cpu`, `graphics`, `process`, `network`, `disk`, `media device` and so on, it is simple to use like:

```C++
#include "probe/system.h"

#include "probe/memory.h"
#include "probe/util.h"

#include <iostream>

int main()
{
    auto os     = probe::system::os_info();
    auto kernel = probe::system::kernel_info();
    auto mem    = probe::memory::status();

    std::cout << "Operating System:\n"
              << "    Name             : " << os.name << '\n'
              << "    Version          : " << probe::to_string(os.version) << '\n'
              << "    Kernel           : " << kernel.name << '\n'
              << "    Kernel Version   : " << probe::to_string(kernel.version) << '\n'
              << "    Theme            : " << probe::to_string(os.theme) << '\n'
              << "    Desktop ENV      : " << probe::to_string(probe::system::desktop()) << '\n'
              << "    Memory           : " << probe::util::GB(mem.avail) << " / "
              << probe::util::GB(mem.total) << " GB\n";
    return 0;
}
```

Output:

```yaml
Operating System:                                   |   Operating System:
    Name             : Windows 11 Pro               |       Name             : Ubuntu 18.04.6 LTS
    Version          : 10.0.25346.1001 (22H2)       |       Version          : 18.4.6.0 (bionic)
    Kernel           : Windows NT                   |       Kernel           : Linux
    Kernel Version   : 10.0.25346.1001              |       Kernel Version   : 5.4.0.148
    Theme            : dark                         |       Theme            : light
    Desktop ENV      : Windows                      |       Desktop ENV      : GNOME (3.28.2.0)
    Memory           : 15.2921 / 31.9388 GB         |       Memory           : 9.82002 / 15.6046 GB
```

## Details

### System

| properties          |      Windows       |      Linux      | commments                 |
| ------------------- | :----------------: | :-------------: | ------------------------- |
| os name             |      &#10004;      |    &#10004;     | Windows 11 Pro            |
| os version          |      &#10004;      |    &#10004;     | 10.0.25346.1001(22H2)     |
| kernel name         |      &#10004;      |    &#10004;     | Windows NT                |
| kernel version      |      &#10004;      |    &#10004;     | 10.0.25346.1001           |
| hostname            |      &#10004;      |    &#10004;     | Device Name/Computer Name |
| username            |      &#10004;      |    &#10004;     | Name of Current User      |
| dark/light mode     |      &#10004;      | `Ubuntu 22.04`  | dark                      |
| desktop environment |    `= Windows`     | `GNOME`/`Unity` | GNOME                     |
| DE version          | same as os version | `GNOME`/`Unity` | GNOME (3.28.2.0)          |
| memory              |      &#10004;      |    &#10004;     | 15.29 / 31.94 GB          |

### CPU

| properties     | Windows  |  Linux   | commments                                |
| -------------- | :------: | :------: | ---------------------------------------- |
| name           | &#10004; | &#10004; | Intel(R) Core(TM) i7-9700K CPU @ 3.60GHz |
| vendor         | &#10004; | &#10004; | GenuineIntel                             |
| sockets        | &#10004; | &#10004; | 1                                        |
| cores          | &#10004; | &#10004; | 8                                        |
| logical cores  | &#10004; | &#10004; | 8                                        |
| base frequency | &#10004; | &#10004; | 3.6 GHz                                  |
| architecture   | &#10004; | &#10004; | x64                                      |
| endianness     | &#10004; | &#10004; | little                                   |
| CPUID/features | &#10004; | &#10004; | check features of CPU by cpuid           |
| L1 Data cache  | &#10004; | &#10004; | 8 x  32 KB                               |
| L1 Inst. cache | &#10004; | &#10004; | 8 x  32 KB                               |
| L2 cache       | &#10004; | &#10004; | 8 x 256 KB                               |
| L3 cache       | &#10004; | &#10004; | 12 MB                                    |

### Graphics

#### GPU

| properties       | Windows  | Linux | commments               |
| ---------------- | :------: | :---: | ----------------------- |
| name             | &#10004; |       | NVIDIA GeForce RTX 2080 |
| vendor           | &#10004; |       | NVIDIA Corporation      |
| dedicated memory | &#10004; |       | 8390705152 B            |
| shared memory    | &#10004; |       | 17146988544 B           |
| frequency        |          |       |                         |

#### Display Monitor

| properties    |       Windows       |    Linux     | commments      |
| ------------- | :-----------------: | :----------: | -------------- |
| name          |      &#10004;       |   &#10004;   | "AOC2790"      |
| id            |      &#10004;       | same as name | "\\.\DISPLAY1" |
| primary       |      &#10004;       |   &#10004;   | true / false   |
| position      |      &#10004;       |   &#10004;   | (-2560, -1440) |
| resolution    |      &#10004;       |   &#10004;   | 2560 x 1440    |
| refresh rate  |      &#10004;       |   &#10004;   | 59.9 Hz        |
| BPP           |      &#10004;       |   &#10004;   | 32             |
| logical DPI   | `>=Windows 10 1607` |   &#10004;   | 96             |
| scale factor  |      &#10004;       |              | 125%           |
| orientation   |      &#10004;       |   &#10004;   | Landscape      |
| physical size |      &#10004;       |              | 60 x 34 cm     |
| physical DPI  |      &#10004;       |              | 108            |

##### Virtual Screen

> the desktop containing all displays

| properties | Windows  |  Linux   | commments      |
| ---------- | :------: | :------: | -------------- |
| position   | &#10004; | &#10004; | (-1920, -1080) |
| resolution | &#10004; | &#10004; | 4480 x 2520    |

> The position of the display can be negative on windows since the primary display always start at (0, 0)

#### Window List

> order: up to down

| properties     | Windows  |  Linux   | commments                                |
| -------------- | :------: | :------: | ---------------------------------------- |
| name           | &#10004; | &#10004; | "README.md - probe - Visual Studio Code" |
| classname      | &#10004; |    -     | "Chrome_WidgetWin_1"                     |
| position       | &#10004; | &#10004; | (741, 328)                               |
| size           | &#10004; | &#10004; | 1568 x 995                               |
| navtive handle | &#10004; | &#10004; | 132198                                   |
| visible        | &#10004; | &#10004; | true / false                             |
| process id     | &#10004; |    -     |                                          |
| process name   | &#10004; |    -     | explorer.exe                             |

### Disk

> Run as Administrator

| properties       | Windows  | Linux | commments                                                                                   |
| ---------------- | :------: | :---: | ------------------------------------------------------------------------------------------- |
| name             | &#10004; |       | \\\\.\\PhysicalDrive2                                                                       |
| path             | &#10004; |       | \\\\?\\scsi#disk&ven_phi&prod_h1#6&314c6ca8&0&000000#{53f56307-b6bf-11d0-94f2-00a0c91efb8b} |
| serial number    | &#10004; |       | 98BB9969766F                                                                                |
| GUID/signature   | &#10004; |       | {C65630B2-E183-4363-9E18-93CE442B9515} (GPT) or 9BD1A50 (MBR)                               |
| vendor id        | &#10004; |       | Samsung                                                                                     |
| product id       | &#10004; |       | Samsung SSD 970 EVO Plus                                                                    |
| bus type         | &#10004; |       | SATA / NVMe / USB ...                                                                       |
| removable        | &#10004; |       | true / false                                                                                |
| writable         | &#10004; |       | true / false                                                                                |
| trim             | &#10004; |       | true / false                                                                                |
| cylinders        | &#10004; |       | 121601                                                                                      |
| tracks/cylinders | &#10004; |       | 255                                                                                         |
| sectors/track    | &#10004; |       | 63                                                                                          |
| bytes/sector     | &#10004; |       | 512                                                                                         |
| partitions       | &#10004; |       | 6                                                                                           |

#### Partition

| properties | Windows  | Linux | commments                              |
| ---------- | :------: | :---: | -------------------------------------- |
| name       | &#10004; |       | Microsoft reserved partition           |
| style      | &#10004; |       | GPT / MBR / RAW                        |
| type id    | &#10004; |       | {E3C9E316-0B5C-4DB8-817D-F92DF00215AE} |
| GUID       | &#10004; |       | {733189C5-6252-4F42-9577-151494026B2B} |
| offset     | &#10004; |       | 17408                                  |
| length     | &#10004; |       | 16759808                               |

#### Volume

| properties    | Windows  | Linux | commments                                             |
| ------------- | :------: | :---: | ----------------------------------------------------- |
| label         | &#10004; |       | New Volume                                            |
| letter        | &#10004; |       | C:\\                                                  |
| filesystem    | &#10004; |       | NTFS                                                  |
| serial number | &#10004; |       | 10194AA9                                              |
| GUID path     | &#10004; |       | \\\\?\\Volume{803b42f7-bbee-4d30-ad22-2d0fe90072b6}\\ |
| capacity      | &#10004; |       | 1870.54 GB                                            |
| free space    | &#10004; |       | 105.929 GB                                            |

### Audio Devices

| properties  | Windows  | Linux | commments                                               |
| ----------- | :------: | :---: | ------------------------------------------------------- |
| name        | &#10004; |       | Logi C310 HD WebCam                                     |
| vendor      |          |       |                                                         |
| id          | &#10004; |       | {0.0.1.00000000}.{d6224b54-e7f7-49d9-ab10-b2071cb4eea5} |
| description | &#10004; |       | Microphone                                              |
| type        | &#10004; |       | AudioSource                                             |
| state       | &#10004; |       | Active                                                  |

### Process

| properties | Windows  |  Linux   | commments                    |
| ---------- | :------: | :------: | ---------------------------- |
| pid        | &#10004; | &#10004; | process id                   |
| ppid       | &#10004; | &#10004; | pid of parent process        |
| state      |          | &#10004; | R, S, D, Z, T, t, X          |
| priority   | &#10004; | &#10004; |                              |
| name       | &#10004; | &#10004; | Win: "xxx.exe", Linux: "xxx" |
| path       | &#10004; |          | "C:\path\to\file.exe"        |
| cmdline    |          | &#10004; |                              |
| starttime  | &#10004; | &#10004; | unix time, ns                |
| nb_threads | &#10004; | &#10004; |                              |
| user       | &#10004; | &#10004; | username                     |

#### Linux

Parse the following files:

- /proc/uptime
- /proc/[pid]/stat
- /proc/[pid]/status
- /proc/[pid]/statm
- /proc/[pid]/cmdline

### Network

| properties             |  Windows  |  Linux   | commments                                                         |
| ---------------------- | :-------: | :------: | ----------------------------------------------------------------- |
| name                   | &#10004;  | &#10004; | Windows: Intel(R) Ethernet Connection (7) I219-V; Linux: eno1     |
| vendor id              | (PCI Bus) | &#10004; | 0x0123                                                            |
| product                | (PCI Bus) | &#10004; | Ethernet Connection (7) I219-V                                    |
| guid                   | &#10004;  |    -     | {FFE7027E-6386-11EB-95F5-806E6F6E6963}                            |
| interface guid         | &#10004;  |          | {653D76BA-C95E-4A46-A3D6-A178841A7AC8}                            |
| description            | &#10004;  | &#10004; | Ethernet                                                          |
| manufacturer           | &#10004;  | &#10004; | Intel                                                             |
| enabled                |           | &#10004; | true/false                                                        |
| bus                    | (PCI Bus) | &#10004; | PCI / USB...                                                      |
| bus_info               | (PCI Bus) |          |                                                                   |
| driver                 | &#10004;  | &#10004; |                                                                   |
| driver version         | &#10004;  | &#10004; |                                                                   |
| virtual                | &#10004;  | &#10004; | true / false, **guessed** by physical address and the description |
| type                   | &#10004;  | &#10004; | Ethernet / IEEE 802.11 / ...                                      |
| physical address (MAC) | &#10004;  | &#10004; | 00:50:56:C0:00:01                                                 |
| DHCP Enabled           | &#10004;  |          | true                                                              |
| MTU                    | &#10004;  | &#10004; | 1500                                                              |
| ipv4_addresses         | &#10004;  | &#10004; | 192.168.189.1                                                     |
| ipv6_addresses         | &#10004;  | &#10004; | fe80::22bc:b757:c253:a7c8                                         |
| dhcpv4_server          | &#10004;  |          |                                                                   |
| dhcpv6_server          | &#10004;  |          |                                                                   |
| ipv4_gateways          | &#10004;  |          |                                                                   |
| ipv6_gateways          | &#10004;  |          |                                                                   |
| dns_suffix             | &#10004;  |          |                                                                   |
| ipv4_dns_servers       | &#10004;  |          |                                                                   |
| ipv6_dns_servers       | &#10004;  |          |                                                                   |

#### Traffic Statistics

| properties       | Windows  | Linux | commments      |
| ---------------- | :------: | :---: | -------------- |
| guid             | &#10004; |       | interface guid |
| ibytes           | &#10004; |       |                |
| obytes           | &#10004; |       |                |
| unicast ibytes   | &#10004; |       |                |
| unicast obytes   | &#10004; |       |                |
| broadcast ibytes | &#10004; |       |                |
| broadcast obytes | &#10004; |       |                |
| multicast ibytes | &#10004; |       |                |
| multicast obytes | &#10004; |       |                |

### Utils

| functions       | Windows  |  Linux   | commments     |
| --------------- | :------: | :------: | ------------- |
| thread_set_name | &#10004; | &#10004; |               |
| thread_get_name | &#10004; | &#10004; |               |
| to_utf8         | &#10004; |          | wchar -> utf8 |
| to_utf16        | &#10004; |          | utf8 -> wchar |

#### Windows

| functions / classes        | commments                                |
| -------------------------- | ---------------------------------------- |
| registry::read             | read value of the registry key           |
| registry::RegistryListener | listen change events of the registry key |

#### Linux

| functions / classes | commments                                                      |
| ------------------- | -------------------------------------------------------------- |
| exec_sync           | execute a commond and return the standard output               |
| pipe_open           | execute a commond and redirect the standard output to the pipe |
| PipeListener        | listen the pipe of the the executed commond                    |
| gsettings functions | wrapper to gsettings commond                                   |

### Compilation Requirement

- `C++ 20`
- `CMake 3.16`
- `Windows 10 1607`
- `Linux` : install `libxrandr-dev`

```bash
mkdir build && cd build
cmake .. -DPROBE_EXAMPLES=ON -DBUILD_SHARED_LIBS=ON
cmake --build . --config Release -j16
```
