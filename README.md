<h3 align="center">Porbe</h3>

<p align="center">
  Hardware and OS information library for C++
</p>

## Usage

**Probe** contains several modules, including `system`, `cpu`, `graphics` and so on, it is simple to use like:

```C++
#include "probe/system.h"

#include "probe/util.h"

#include <iostream>

int main()
{
    auto os     = probe::system::os_info();
    auto kernel = probe::system::kernel_info();
    auto mem    = probe::system::memory();

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
Operating System:
    Name             : Windows 11 Pro
    Version          : 10.0.25346.1001 (22H2)
    Kernel           : Windows NT
    Kernel Version   : 10.0.25346.1001
    Theme            : dark
    Desktop ENV      : Windows
    Memory           : 15.2921 / 31.9388 GB
```

## Details

### System

| properties          |      Windows       |      Linux      | commments             |
| ------------------- | :----------------: | :-------------: | --------------------- |
| os name             |      &#10004;      |    &#10004;     | Windows 11 Pro        |
| os version          |      &#10004;      |    &#10004;     | 10.0.25346.1001(22H2) |
| kernel name         |      &#10004;      |    &#10004;     | Windows NT            |
| kernel version      |      &#10004;      |    &#10004;     | 10.0.25346.1001       |
| dark/light mode     |      &#10004;      | `Ubuntu 22.04`  | dark                  |
| desktop environment |    `= Windows`     | `GNOME`/`Unity` | GNOME                 |
| DE version          | same as os version | `GNOME`/`Unity` | GNOME (3.28.2.0)      |
| memory              |      &#10004;      |                 | 15.29 / 31.94 GB      |

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
| L1 Data cache  | &#10004; |          | 8 x  32 KB                               |
| L1 Inst. cache | &#10004; |          | 8 x  32 KB                               |
| L2 cache       | &#10004; |          | 8 x 256 KB                               |
| L3 cache       | &#10004; |          | 12 MB                                    |

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

### Disk

| properties         | Windows  | Linux | commments                                                                                   |
| ------------------ | :------: | :---: | ------------------------------------------------------------------------------------------- |
| name               | &#10004; |       | \\\\.\\PhysicalDrive2                                                                       |
| path               | &#10004; |       | \\\\?\\scsi#disk&ven_phi&prod_h1#6&314c6ca8&0&000000#{53f56307-b6bf-11d0-94f2-00a0c91efb8b} |
| Serial Number      | &#10004; |       | 98BB9969766F                                                                                |
| VendorID           | &#10004; |       | Phi                                                                                         |
| ProductID          | &#10004; |       | Hi                                                                                          |
| Cylinders          | &#10004; |       | 121601                                                                                      |
| Tracks / Cylinders | &#10004; |       | 255                                                                                         |
| Sectors / Track    | &#10004; |       | 63                                                                                          |
| Bytes / Sector     | &#10004; |       | 512                                                                                         |

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
