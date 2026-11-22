# PATCH 1.0.10 — "Reliable Green NIC": Fix for boot-time recovery during the SCAN↔AUTH cycle

**Base:** Golden Build v8.7.17 (kext 1.0.9) — patch APPLIED within this source tree.
**Status:** Ready to compile (`./Create_Build.tool`).

## The Issue Resolved

Following a reboot, the connection process enters an infinite loop:

```
join progress for Network-Wifi: scanning for the network (state 1)
join progress for Network-Wifi: authenticating (state 2)     (~14 s)
join progress for Network-Wifi: scanning for the network (state 1)
...
```

It never reaches state 3; the 802.11 NIC remains "red" (inactive/disconnected) in Network Preferences. Rebooting, "forgetting" the network, switching USB ports, or restarting the router have no effect. However, physically unplugging and replugging the dongle resolves the issue every time (log 2026-11-19 06:57:59 — link established 3 seconds after re-plugging).

## The Root Cause (Identified in the Code)

`bootRescueTick()` already contains the necessary remedy—a "software re-plug" (a USB re-enumeration performed in software, which has the same effect as physically unplugging and replugging the device)—but its detection logic, specifically the condition `"station up but the requested join never authorized,"` fails to trigger in this scenario:

1. Its 120-second timer is anchored to `lastJoinRequestedMS_`, a timestamp that the application updates every time a connection request is resubmitted (~46 seconds) and upon every restart (the agent relaunches the application, which then triggers an automatic reconnection attempt upon startup). 2. It requires catching `ic_state` outside of the AUTH/ASSOC states, given that the cycle spends ~14 s in AUTH versus ~5 s in SCAN per round trip.

## Changes made (5 files)

| File | Change |
|---|---|
| `Driver/RTWUSBController.hpp` | New member flag `everReachedAuth_ = false` (line 308). |
| `Driver/RTWUSBController.cpp` — `stateTransition()` | `if(!result&&state==IEEE80211_S_AUTH)s->everReachedAuth_=true;` — records that the AUTH state was actually reached (meaning the network exists and was visible). |
| `Driver/RTWUSBController.cpp` — `bootRescueTick()` | New detection logic: a join request has been pending for ≥ 90 s (anchored to `firstJoinRequestedMS_`, unaffected by re-submissions), AUTH has been reached at least once, the connection was never successfully established, and the state is not RUN → trigger a software re-plug. The exclusion for `AUTH/ASSOC` states and the timer based on the most recent submission have been removed for this specific case. |
| `Driver/Info.plist` | `CFBundleShortVersionString` / `CFBundleVersion`: 1.0.9 → **1.0.10**. |
| `Driver/RtWlanUCompat.h` | `RTWLANU_DRIVER_VERSION`: "1.0.9" → **"1.0.10"** (the app will display `driver version 1.0.10`). |

Kernel log prefixes `[macOS-RTL-WIFI …]` have also been updated to 1.0.10.

## Unchanged safeguards

- Maximum of 3 re-plugs per attachment, 6 per boot, with a minimum interval of 20 s. - 12-minute window, applicable only after attachment.
- Never during a session where a carrier has already been active (`bootEverAuthorized_`).
- Never during sleep, stop, or teardown.
- If the network is simply absent (AUTH stage never reached): no re-plugging occurs.

## Compiling and Installing

```
cd macOS-RTL-WIFI-RTLWiFiBar-Patch-1.0.10
./Create_Build.tool
```

Outputs in `Build/Release/`: `Wireless USB macOS-RTL-WIFI.pkg`,
`macOS-RTL-WIFI.kext`, and `RTLWiFiBar.app`. Install the `.pkg` file, then restart.

**Important — ensure only one copy of the kext exists on the machine:** if the kext is also
injected via OpenCore, you must ALSO replace the copy located in `EFI/OC/Kexts`
with the `macOS-RTL-WIFI.kext` from this build. Having two different versions
in memory is a known pitfall for this project (see README v8.7.6).

## Verification After Installation

`RTLWiFiBar.log` should display `driver version 1.0.10`. Upon the next freeze,
the kernel log should show, in order:

```
log show --last 15m --predicate 'eventMessage CONTAINS "RtWlanU"' --info

[macOS-RTL-WIFI 1.0.10] boot rescue: station up but the requested join never authorized - scheduling software re-plug
[macOS-RTL-WIFI 1.0.10] boot rescue (...): software re-plug 1/3 (USB re-enumeration, ...)
[macOS-RTL-WIFI 1.0.10] re-enumerating the USB device now (same effect as unplug/replug)
```

Then, in `RTLWiFiBar.log`: a brief flurry of "NotReady"/reopen events from the client, followed by either
`join monitor: link is up ... 3 s after submit` (occurring automatically), or—if you click
on the network—the status should turn green in ~3 seconds.

## Workaround (without recompiling)

When a freeze occurs: unplug and replug the dongle, wait ~30 seconds, then
re-select the network—it should turn green in 3 seconds. Alternatively, leave the app open
without taking any action (no reboot, restart, or "Forget" operation) for ~5 minutes,
then re-select the network.