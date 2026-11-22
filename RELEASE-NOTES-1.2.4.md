# Release 1.2.4 (kext 1.1.1, app 8.8.4)
- App 8.8.4: after a link that lived 30 s or more is ended mid-session (the AP
  deauthenticated the station, or a local reset), the app now rejoins the saved
  network by itself, using the same bounded auto-rejoin as at startup (it waits
  for a survey that sees the network, and stands down if you pick or forget a
  network). At most 6 per rolling hour; flaps under 30 s stay manual. Field
  log 2026-11-22 (Archer T4U, 5 GHz): the AP dropped the station twice
  (deauth reason 4, then reason 7) and nothing rejoined until "Rejoin" was
  clicked.
- Adapters: Archer T3U Nano and EDIMAX EW-7811Un (N150) reported working by users; listed with a distinct mark (not tested by the maintainer).
- App: after the dongle was unplugged and plugged back in, the saved network was
  not rejoined (field log 2026-11-22, Archer T4U: replug, driver returned, link
  stayed down). The driver-return watch is now also armed by the dead-driver
  client restart, and a successful connection resets the per-session rescue
  budget (2 rescue rejoins).

# Release 1.2.3 (kext 1.1.1, app 8.8.2 - no code change)

## Documentation and licensing
- README rewritten: Quick start, per-family test status, 13 tested adapters,
  known limitations, how to report a problem.
- New `SUPPORTED-ADAPTERS` list merged into the README (tested adapters marked).
- New `THIRD-PARTY-NOTICES.md`: both Realtek firmware licenses, OpenBSD/BSD/ISC
  and itlwm notices. Shipped in Release.zip (workflow) and `Build/Release`
  (`Create_Build.tool`).
- `Driver/RTL8192EUFirmware.inc`: full urtwn notice in the header (was a one-line pointer).
- New project page `docs/index.html` (dark, responsive, single self-contained file): tested adapter table with filter and search, install steps, known limits. Enable it in GitHub: Settings, Pages, Deploy from branch `main`, folder `/docs`.
- `Create_Build.tool`: RTLWiFiBar.app is now copied to `Build/Release` even without `Support Donate.html`.

# Release 1.2.2 (previous) (kext 1.1.1, app 8.8.2)
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
