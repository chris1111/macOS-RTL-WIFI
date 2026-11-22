# Source provenance — macOS-RTL-WIFI

| Component | Upstream | Pin / revision | License |
|---|---|---|---|
| 8188E hardware programming reference | https://github.com/lwfinger/rtl8188eu | `Core/RTL8188EU-Upstream.json` (commit f5d1c8df2e2d8b217ea0113bf2cf3e37df8cb716) | GPL-2.0 |
| r92e common core, HT rate config | Linux `rtl8xxxu` (pinned) | `Stack/rtwn/r92e_ht.inc` header note | GPL-2.0 |
| USB driver core + 8188E branches | OpenBSD `if_urtwn.c` v1.114 lineage | file headers in `Driver/RTW8192EUHAL.cpp` | BSD |
| net80211 stack (macOS port) | OpenIntelWireless itlwm / OpenBSD net80211 | `Stack/itl80211/` file headers | itlwm license (`Stack/itl80211/LICENSE.itlwm`) |
| FreeBSD rtwn references | freebsd/freebsd-src | `Stack/rtwn/FREEBSD-REFERENCE-SHA256.json` | BSD |
| Regulatory data | wireless-regdb 40095bd16e53d5c5682fd6e7e3dbbbfaf0fe5410 | `Core/DFSRegulatoryTables.inc` header, `Stack/wireless-regdb/` | ISC |
| Kernel headers (build-time) | macOS SDK shipped inside Xcode (`Kernel.framework`) | `SDKROOT` on the build machine | Apple SDK terms |
| Kernel-safe C++ subset | this project — own library | `libs/kstl/` (array, algorithm, cstddef, cstdint, cstring, cstdlib, type_traits, utility, vector) | GPL-2.0 |

## Firmware

The RTL8188EU firmware image is compiled into the driver
(`Driver/LegacyUSBWiFiFirmware.inc`). Its provenance record — image identity,
size, SHA-256, signature byte and license — is `Firmware/legacy-firmware-provenance.json`
(entry `rtl8188eu`, 13,904 bytes, SHA-256 `1241ddbfc87f0495e0bf09d8b94c94680b60a3d8eeab89462b3c4d8d3e8a1ee0`,
license `Firmware/urtwn-license`). The upstream `lwfinger/rtl8188eu` image
`rtl8188eufw.bin` is byte-identical to this pinned image.

## Project-authored material

- macOS IOKit integration (`Driver/RTWUSBController.cpp`, `Driver/RtWlanU*`,
  `Driver/RTWUserClient.cpp`), including the TODS frame-direction fix and the
  DHCP/link heal escalation (240000/480000/900000 ms).
- Xcode project, `mk.tool` build command, `libs/kstl/` kernel C++ subset,
  build self-checks, documentation and this provenance file.

File-level copyright and license notices inside the source files are part of
the material and must be preserved in any redistribution.
