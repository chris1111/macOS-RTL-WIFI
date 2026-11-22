# BUILD NOTES — Golden Build v8.7.17

**Date:** 2026-11-18
**App (RTLWiFiBar):** `CFBundleShortVersionString` / `CFBundleVersion` **8.7.17**
**Kext (macOS-RTL-WIFI):** **1.0.9 — UNCHANGED** (zero driver code changes this round)
**Base:** v8.7.16 (the tree that shipped as `macOS-RTL-WIFI-RTLWiFiBar-Golden-Build-v8.7.16.zip`)

This round is the user's "final touch": **the saved network reconnects by
itself after a restart.** Field report 2026-11-18: *"everything is working
perfectly, and the network connects and stays connected. The only remaining
issue is that, after a reboot, the application needs to automatically
recognize the previously saved network — currently, I have to manually
reselect my network, after which it reconnects just as it did before the
reboot."* Everything below is **app-side only**; the join path that
v8.7.14/v8.7.15 fixed is used exactly as-is.

---

## 1. What changed

### 1.1 The saved network survives a restart (the v8.7.13 launch wipe is gone)

| File | Change |
|---|---|
| `App/RTLWiFiBar/AppDelegate.m` | `applicationDidFinishLaunching` no longer calls `[self.profiles resetAllProfiles]`. It now loads the persisted profiles (`savedSsids`), pins the **most recently used** saved network as the active selection again (`savedSsid`), seeds `knownSsids` from the store, and logs `startup: saved network "<ssid>" loaded - auto-rejoin armed, it fires when a scan sees the network` — or, with no profile, `startup: no saved network - pick yours in Networks and enter the password; it reconnects by itself after the next restart`. The stale `RTWActiveSsid` defaults key is still removed at launch (leftover cleanup from older builds). |
| `App/RTLWiFiBar/ProfileStore.h` / `.m` | New `lastUsed` stamp (epoch seconds) written on every `rememberNetwork:` call, and a new `- (nullable NSString *)mostRecentlyUsedSsid` selector that returns the SSID with the newest stamp (unstamped profiles from older builds sort behind stamped ones). `resetAllProfiles` itself is kept, but nothing calls it at launch anymore. |

### 1.2 One bounded startup auto-rejoin (v8.7.17)

| File | Change |
|---|---|
| `App/RTLWiFiBar/AppDelegate.m` | New section `#pragma mark Startup auto-rejoin (v8.7.17)` with three methods: `maybeFireStartupAutoRejoinWithRecords:` — armed once at launch (`autoRejoinArmedSsid`), fires **only** when a completed survey actually sees the saved SSID (printable-prefix compare, joinable record), the driver is reachable, the link is down and no other join is in flight; `submitAutoRejoin:passphrase:` — the manual `submitJoin` minus the modal failure alert (at login time nobody may be there to click it): arms the same join monitor (`noteJoinSubmittedForEntry`), so the outcome rides the existing 45 s window / two bounded re-submits / honest-failure machinery; `standDownStartupAutoRejoin:` — logs and disarms. `performScan` hands every completed survey to the fire-check right after the records land. Give-up rule: five surveys that never see the network (~100 s of the 20 s freshness cadence) end it loudly — `startup auto-rejoin gave up on <ssid> ... pick it in Networks to connect` — plus the same note on the menu line. |
| Stands-down | The user always wins: picking any network in Networks (`you picked the same network yourself` / `you picked a different network`), or Forgetting the saved one (`you forgot this network`), cancels the arm immediately. A linked-up state or a manual join in flight also stands it down. Mid-session link drops do **not** re-arm it — outside this one startup case the app still never joins on a timer, after a wake, or for a drop (the v8.7.13 rule stands). |

### 1.3 Wording updates (session-only → persists across restarts)

| Location | Old | New |
|---|---|---|
| `commitPendingNetworkSave` log | `...saved (this session only; after a restart you type the password again)` | `...saved (the app reconnects to it by itself after a restart; Forget removes it)` |
| Password dialog text | `...stays saved for this session only...after the app restarts you enter the password again...` | `...is remembered in this app's own profile file (...Profiles.plist - never the macOS Keychain) and the app reconnects to it by itself after a restart; use Forget in the menu to remove it.` |
| `rejoinSavedNow` alert | `...stays saved for this session; after a restart you enter the password again.` | `...stays saved and the app reconnects to it by itself after a restart; Forget removes it.` |
| Networks list checkmark | only the active pick marked remembered | **any** saved profile shows the checkmark (multiple persisted networks are all real now) |
| Menu Rejoin comment | "a profile now only exists after the user typed the password in this session" | profiles persist; Rejoin stays the manual button for drops / given-up rejoins |

### 1.4 Bookkeeping

- `App/RTLWiFiBar/Info.plist` — version bump `8.7.16` → **`8.7.17`** (both keys).
- `App/RTLWiFiBar/AppDelegate.m` — header design-notes gained the v8.7.17
  entry and the v8.7.13 block carries a one-line amendment pointer.
- `README.md` — the "Joins only when you ask" feature bullet now documents
  the restart reconnect, and a full **Golden Build v8.7.17** changelog
  section was added.
- `scripts/sim_build.sh` — new section 6: 22 assertions covering the
  removed launch wipe, the survey-gated one-shot auto-rejoin, the
  stand-down paths, the give-up rule, the persisted store stamping and
  the new messages.

## 2. What did NOT change

- **Driver** (`Driver/`, `Core/`, `Stack/`, `Headers/`, `Firmware/`,
  `libs/`) — untouched; kext stays **1.0.9** with the v8.7.15
  goodbye-frame + session-rescue fixes. The kext never learns anything
  about profiles — persistence is app-side only.
- **Join machinery** — `submitJoin`, `monitorJoinFlight`,
  `noteJoinSubmittedForEntry`, the scan guard, the 45 s window, the two
  bounded re-submits, the honest failure report: all byte-identical in
  behavior; the auto-rejoin is a fourth caller of the same path.
- **PKG scripts, agent plist, icon, Create_Build.tool, uninstaller** —
  untouched; exactly the v8.7.16 packaging round (`/Applications`
  install, old utility retired, Release folder app copy).
- **Forget, manual join, password prompt mechanics, Reveal Log** —
  untouched.

## 3. Behavior after a reboot (what to expect in the log)

```
RTLWiFiBar 8.7.17 (build 8.7.17) starting (pid ...)
startup: saved network "Network-Wifi" loaded - auto-rejoin armed, it fires when a scan sees the network
user client opened (type 0)
adapter MAC ...
driver version 1.0.9
scan triggered
scan done: N networks
startup auto-rejoin: "Network-Wifi" is in the survey - joining with the saved password
auto-rejoin submit for Network-Wifi -> ok
join progress for Network-Wifi: scanning for the network (state 1)
join monitor: link is up on Network-Wifi 3 s after submit (driver state 1: ...)
connection established - network "Network-Wifi" saved (the app reconnects to it by itself after a restart; Forget removes it)
```

Edge paths, all loud: five scans without the network → `startup
auto-rejoin gave up on ...`; a rejected submit → `auto-rejoin submit for
... -> failed (...)` + menu line; the user picks/forgets first →
`startup auto-rejoin stood down for ... - <reason>`.

## 4. Upgrade behavior (v8.7.16 → v8.7.17)

- Identical installer flow to v8.7.16 (`/Applications`, agent repoint,
  required restart). Nothing to configure.
- The first session after upgrading still needs **one manual join** (the
  launch wipe of older builds already deleted the stored profile); every
  restart after that reconnects by itself.
- To stop the automatic reconnect for a network, use **Forget** — it
  deletes the persisted profile for real.

## 5. Build on the Mac

```
./Create_Build.tool          # release kext + app + installer
./Create_Build.tool debug    # debug build
./Create_Build.tool -v       # verbose compiler output
```

Products in `Build/Release/`: `Wireless USB macOS-RTL-WIFI.pkg`,
`macOS-RTL-WIFI.kext`, `RTLWiFiBar.app`, `Support Donate.html`.

## 6. Verification performed on this build

- Full Linux-sandbox build simulation (`scripts/sim_build.sh`):
  mock-toolchain app build + package assembly + the /Applications
  assertions of v8.7.16 **plus the new section-6 v8.7.17 assertions** —
  all green.
- Packaged-zip probe (`package_v8717.py`): zip integrity, every
  `MANIFEST.sha256` hash, all v8.7.8 → v8.7.16 fix markers (still
  present), the new v8.7.17 markers (launch wipe gone, armed
  auto-rejoin, survey gating, stand-down, give-up, lastUsed stamping,
  new messages, version stamps app 8.7.17 / kext 1.0.9) — all green.
