# Release 1.2.2 (kext 1.1.1, app 8.8.2)
- App: the About panel showed "Version 8.7 (18)" (hardcoded). It is now derived
  from the app version: 8.8.2 shows as "Version 8.8 (2)".

# Release 1.2.1 (previous) (kext 1.1.1, app 8.8.1)

## Chinese (Simplified, zh-Hans) added
- Installer (PKG): new `PKG/Resources/Resources/zh-Hans.lproj` (Welcome,
  Conclusion, Support), built from the English one with the same images.
- App: new `App/RTLWiFiBar/zh-Hans.lproj/Localizable.strings` (41 UI strings);
  `zh-Hans` added to `CFBundleLocalizations`. 16 languages in total.
- Traditional Chinese (zh-Hant) is not included yet.

## Installer images
- `PKG/Resources/Resources` replaced by the updated set (new Welcome.rtfd
  images, incl. AppNetwork.png) for all languages.

# Release 1.2.0 (previous)

## App: user interface translated
- RTLWiFiBar menu, status lines, buttons, tooltips and the password dialog now
  follow the macOS language. Languages: da, de, en, es, fr, fr-CA, it, ja, ko, nl, pl,
  pt-PT, ru, sv, tr (zh-Hans added in 1.2.1).
- 41 UI strings, wrapped in NSLocalizedString (English text is the key, so a
  missing translation falls back to English). Files:
  `App/RTLWiFiBar/<lang>.lproj/Localizable.strings`.
  `Create_Build.tool` copies the .lproj folders into the app bundle.
- Not translated on purpose: RTLWiFiBar.log, driver diagnostics, and the
  technical join-failure explanations (they stay English so logs are readable
  when debugging).
- Version bumped to 8.8.0; `CFBundleLocalizations` added to the app Info.plist.
- Translations are machine-written and not yet reviewed by native speakers.

## Kext
Unchanged from 1.1.1.

## Planned next
- App button to remove old/absent network adapters (so a replacement dongle
  does not show up as "802.11n NIC 2"). Not in this release.
- RTL8188EU DHCP (see KNOWN-ISSUES.md).

# Release 1.1.1 (previous) (kext 1.1.1, app 8.7.25)
- Fix: opening System Settings > Network right after connecting dropped the
  link. macOS disables/enables the interface when that pane opens; the driver
  already ignored this while associated (1.0.28) but not while the join was
  still in progress (4-way handshake not yet finished), where it fully reset
  the station. It is now ignored in both cases.

# Release 1.1.0 (previous) (kext 1.1.0, app 8.7.25)

First consolidated release. It replaces the 1.0.x patch series; the per-patch
notes are kept in `docs/patch-history/`.

## What changed since the last validated patch (1.0.56 / kext 1.0.53)
- Edimax EW-7611UCB (7392:a611, RTL8723BU) joins reliably: its 4th bulk-out
  endpoint is accepted (clamped to 3) instead of failing discovery.
- RTL8821AU (TP-Link T2U Plus): firmware download tolerates a stale checksum
  bit, TX-empty wait and IQK/LCK non-convergence are no longer fatal on a
  healthy bus, stays at 20 MHz (80 MHz made the link flap).
- RTL8723BU: calibration non-convergence is no longer fatal.
- 8812/8821 power-plan: invalid efuse bytes fall back to the nearest valid group.
- The experimental 1.0.57/1.0.58 changes for the RTL8188EU family are NOT
  included (the 1.0.58 TX-path experiment had no effect and was reverted).

## Packaging
- Removed the stale prebuilt `PKG/statusbarapp/RTLWiFiBar.zip` (old v8.7.16
  binary) from the source tree. `Create_Build.tool` builds the app and
  regenerates this file on every build; it is now git-ignored.

## Diagnostics
Passive diagnostics published in the IORegistry (RTWDiag*, RTWDiscoveryFail,
RTWEfuse*) and copied into RTLWiFiBar.log are intentionally kept: they have no
effect on behaviour and are needed to finish the RTL8188EU work.

## Tested
See `DONGLES.md`.
