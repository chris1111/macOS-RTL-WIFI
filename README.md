# macOS-RTL-WIFI 
Realtek RTL8188EU USB Wi-Fi driver for Intel macOS

Independent macOS kernel-extension project for Realtek **RTL8188EU** USB Wi-Fi
adapters on Intel Macs, macOS 11 Big Sur through macOS 26. Station mode,
2.4 GHz (802.11b/g), WPA2-Personal.

> **This driver is in development.** Expect changes between releases; test
> before relying on it for daily use, and report issues on the project page.

- **Kext + installer package.** `Create_Build.tool` builds the kext and, from
  the `PKG/` folder in this tree, the full `Wireless USB macOS-RTL-WIFI.pkg`
  installer: original Realtek StatusBarApp (v5.0.9.b11) + RtWlanHelper, launch
  agent, automatic EFI drop-in for OpenCore or Clover (including
  the OpenCore `config.plist` entry) and an uninstaller.
- **Builds from the command line** with `./Create_Build.tool` (plain
  `xcodebuild` also works for the kext alone).
- **No external SDK, no downloads.** Kernel headers come from the macOS SDK
  already inside Xcode; the small kernel-safe C++ subset the sources use lives
  in this tree under `libs/kstl/` (own library, GPL-2.0). The packaging step
  uses `pkgbuild` and `pkgutil`, which ship with macOS.

## Supported adapters — full verified Realtek USB fleet, 263 personalities

The kext ships the complete verified identity catalog
(`Core/USBWiFiProfiles.inc`) as IOKit personalities. Only identities with
verified catalog provenance ship — 8 unverified rows and the station-gated
RTL8814BU stay out (fail-closed).

| Family | USB IDs | Backend status |
|---|---|---|
| RTL8188EU / RTL8192EU (r92e) | 28 | field-proven on this iMac |
| RTL8192CU era (8188CU/CUS/CTV/RU/92C) | 88 | implemented, firmware embedded |
| RTL8812AU | 36 | implemented, firmware embedded |
| RTL8821AU | 35 | implemented, firmware embedded |
| RTL8822BU / RTL8822CU | 32 | implemented, firmware embedded |
| RTL8821CU | 12 | implemented, firmware embedded |
| RTL8814AU | 17 | implemented, firmware embedded |
| RTL8192DU | 5 | implemented, firmware embedded |
| RTL8192FU | 4 | implemented, firmware embedded |
| RTL8723BU | 3 | implemented, firmware embedded |
| RTL8710BU | 2 | implemented, firmware embedded |

The driver identifies the chip at attach time and dispatches through the
family backend table (`Core/FamilyBackend.c`); every family's firmware image
is embedded in the Driver layer. EU/92EU is hardware-validated; the other
families run the same verify-before-touch attach and are expected to fail
cleanly rather than wedge the machine, but they need field logs for
validation. Full per-ID lookup (decimal columns for ioreg output):
`USB-ID-fleet-map.md`.

Support Adapter: Many of this list not verifying yet.

- Acer UWA5
- Alfa AWUS036AC 
- Alfa AWUS036ACH
- Archer T2U Plus (AC600) Not Working
- Archer T2U Nano
- Archer T3U Nano
- Archer T3U
- Archer T3U Plus
- Archer T2U MINI V3
- ArcherT4U V1, V2, V3
- Archer T9UH V1, V2
- ASUS USB AC68
- ASUS USB-N13
- ASUS USB Nano-AC53
- BrosTrend FBA_AC3
- COMFAST CF-811AC
- COMFAST CF-812AC
- Comfast CF-WU810N
- Comfast CF-758F
- Cudy WU1300S
- Cudy WU700
- CXFTEOXK
- DLink DWA-121 N150
- DLink DWA-131 E
- DLink DWA-171 C
- DLink DWA-182 D
- DLink DWA-192 A
- EDIMAX EW-7611UCB
- EDIMAX EW-7722UTn V2
- EDIMAX_EW-7822ULC
- EDIMAX EW-7833UAC
- EDIMAX EW-7612Uan V2
- EDIMAX N300
- EDIMAX EW-7811Un (N150) Not Working
- EDUP EP-AC1689
- Fenvi AC1300 (RTL8812bu)
- FILOWA USB WiFi-RTL8812BU
- Foktech AC600 Nano
- Jensen Eagle 100-AC
- Kextech MINI USB RTL8192
- Linksys WUSB6300 V2
- Linksys WUSB6400M
- Netgear A6100
- Netgear A6150
- Netgear A7000
- Netis WF2120 N Nano USB
- M-Tech UW-01 USB
- Mercusys MW300UM
- Plexgear AC1200
- Raimadim WiFi AC1300 [RTL8822BU]
- Sitecom WLA7100
- TechKey AC1200
- TL-WN823Nv1/v2/v3/v4
- TL-WN725Nv3
- TL-WN723Nv2/v3
- TL-WN722Nv2/v3
- TL-WN821Nv6
- TL-WN822Nv4/v5
- TENDA W311-MINI
- TENDA U12
- TRENDnet N150 Micro
- TRENDnet TEW-808UBM
- TRENDnet TEW-908UB
- UGreen CM448
- YUNCLOUD Realtek (RTL8814AU)
- ZAPO W58L (RTL881lAU)
- Zebronics USB150WF1
- Zyxel NWD6602
- Zyxel NWD6605

## Requirements

- Intel Mac (the kext is x86_64 only), full Xcode with command-line tools
  selected (`sudo xcode-select -s /Applications/Xcode.app/Contents/Developer`).
- Nothing else. The build reads the macOS SDK that ships inside Xcode
  (`Kernel.framework` headers) and the in-tree `libs/kstl/` subset — no git
  clone, no fetched SDK, no network access.
- An OpenCore setup you administer (Clover also works: `EFI/CLOVER/kexts/Other`).
- For the installer package, nothing extra: `pkgbuild` and `pkgutil` are part
  of macOS.

## Build from the command line

One command, no arguments required:

       ./Create_Build.tool          # release build + installer package
       ./Create_Build.tool debug    # debug build + installer package
       ./Create_Build.tool -v       # show the full compiler output

Products:

- `Build/Release/macOS-RTL-WIFI.kext` — the driver.
- `Build/Release/Wireless USB macOS-RTL-WIFI.pkg` — the installer package.
  Before packaging, the tool copies the fresh kext into the
  `PKG/efifolderoc/` and `PKG/efifolderclover/` staging folders, so the EFI
  choices always carry the kext that was just built. When it finishes, it
  opens `Build/Release` in Finder.

The package offers three choices (sub-packages built with `pkgbuild` from
`PKG/`, flattened with `pkgutil` and the `PKG/Resources/Distribution` script;
welcome, read-me and conclusion pages ship in the same 15 languages as the
bundled StatusBarApp — da, de, en, es, fr, fr-CA, it, ja, ko, nl, pl, pt-PT,
ru, sv, tr — picked from the system language, English as fallback):

| Choice | What it installs |
|---|---|
| Drivers OC macOS-RTL-WIFI | kext to `EFI/OC/Kexts/` on the ESP + `Kernel > Add` entry merged into `config.plist` |
| Drivers CLOVER macOS-RTL-WIFI | kext to `EFI/CLOVER/kexts/Other/` on the ESP (Clover scans kexts, no config.plist edit) |
| Uninstall Utility | removes the WLAN app support, launch agent and the kext from the ESP |

Both driver choices also install the original Realtek StatusBarApp (v5.0.9.b11)
and RtWlanHelper into `/Library/Application Support/WLAN`, add the launch
agent (`Wlan.Software.plist`) and require a restart. The uninstaller removes
the `macOS-RTL-WIFI.kext` entries from the OpenCore `config.plist` and deletes
the kext from the ESP (`EFI/OC/Kexts/` and `EFI/CLOVER/kexts/Other/`).


## Provenance and credits

Primary upstream reference for the RTL8188EU hardware programming:

**https://github.com/lwfinger/rtl8188eu** (GPL-2.0)

The full upstream list — Linux `rtw88` / `rtl8xxxu`, OpenBSD `rtwn` and
`net80211`, the OpenIntelWireless `itlwm` net80211 macOS integration, FreeBSD
`rtwn`, `wireless-regdb`, Realtek firmware terms — is documented in
[CREDITS.md](CREDITS.md) and [SOURCES.md](SOURCES.md).

## License

GPL-2.0 — see [LICENSE-GPL-2.0.txt](LICENSE-GPL-2.0.txt). Third-party notices
and licenses are retained in the respective files and under `Firmware/`.

## Status

This tree carries the driver code generation field-tested as 1.0.25 (now
re-versioned 1.0.0 under the new identity): the TODS frame-direction fix in
`Driver/RTWUSBController.cpp` (the `IEEE80211_FC1_DIR_TODS` frame-direction
check) and the DHCP/link heal backoff sequence (240000 / 480000 / 900000 ms).
The rebuild changes packaging, identity, build command and SDK source only —
re-verify on hardware after the first EFI swap.
