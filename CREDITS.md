# Credits and upstream acknowledgements

macOS-RTL-WIFI is an independent project. The RTLWiFiBar application, the
link-heal logic and the build system in this tree are this project's own work.
The driver stands on published upstream material and shares some files with
the projects credited below; every third-party copyright and license notice is
retained in the respective files.

## Primary upstream reference — RTL8188EU

- **https://github.com/lwfinger/rtl8188eu** — Larry Finger's standalone
  RTL8188EU Linux driver (GPL-2.0). Primary reference for the 8188E hardware
  programming: factory MAC/BB/RF register tables, power-on/LPS flows, IQK/LC
  calibration, TX/RX descriptor layout, USB aggregation defaults and the
  8188EU firmware image lineage. Pin record: `Core/RTL8188EU-Upstream.json`.

## Other upstream material

- [Linux kernel](https://github.com/torvalds/linux) — `rtw88`, `rtl8xxxu`,
  `rtlwifi` wireless drivers (register tables, rate control, coex references).
- [lwfinger/rtw88](https://github.com/lwfinger/rtw88) and
  [lwfinger/rtl8xxxu](https://github.com/lwfinger/rtl8xxxu) — documented Linux
  driver references.
- [OpenBSD](https://github.com/openbsd/src) — `rtwn` USB driver and `net80211`
  stack (BSD/ISC notices preserved per file).
- [FreeBSD](https://github.com/freebsd/freebsd-src) — Realtek `rtwn` reference
  implementations.
- [OpenIntelWireless/itlwm](https://github.com/OpenIntelWireless/itlwm) — the
  macOS-compatible `net80211` integration used by the Stack.
- [danielnmaldonado/TahoeRealtekWiFi](https://github.com/danielnmaldonado/TahoeRealtekWiFi)
  — macOS Realtek USB Wi-Fi project (GPL-2.0), used as a reference; some files
  in `Core/` and `Driver/` are shared with it. Original notices are retained.
- [wireless-regdb](https://git.kernel.org/pub/scm/linux/kernel/git/wens/wireless-regdb.git)
  — regulatory data reference (ISC license retained, `Stack/wireless-regdb/`).
- Realtek Corporation — hardware definitions and the separately licensed
  firmware images whose redistribution terms live under `Firmware/`.
- Realtek Corporation — Wireless Network Utility `StatusBarApp`.


## Bootloader injections

- Clover — `Bootloader injection`/`Mount EFI script`.
- OpenCore — `Bootloader injection`.

Individual contributors credited by imported files (including Larry Finger)
retain their own notices.

## Licenses

GPL-2.0 for this project does not supersede separately licensed upstream
components: BSD/ISC/MIT notices are preserved per file; firmware images carry
their own redistribution terms (`Firmware/urtwn-license`,
`Firmware/LICENCE.rtlwifi_firmware.txt`,
`Firmware/legacy-firmware-provenance.json`).
This file is a directory of credits, not a relicensing statement.
