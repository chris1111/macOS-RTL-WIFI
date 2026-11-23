# Known issues (release 1.2.4)

## Archer T4U (RTL8822BU): USB fault after about 2 hours (under investigation)
- Field log 2026-11-22 (ProDesk, Tahoe, kext 1.1.1): joined on 5 GHz channel 44
  at 06:48; the access point sent a deauth (reason 4, inactivity) at 08:36 after
  1 h 47 min; at 08:51 a USB I/O error followed by a USB timeout (fault site 12,
  2 h 03 min after attach) wedged the driver. The app restarted its client and
  rejoined in 3 s.
- Not reproduced yet. Next step: one kernel log (`log show`) around the fault.

## RTLWiFiBar
- Links that end after 30 s or more are rejoined automatically (max 6 per hour);
  a flap shorter than 30 s still needs a manual Rejoin.
- Ventura: the connection dropped when opening Network preferences. Fixed in
  kext 1.1.1 (verified on the TL-WN823N).
- After Forget + rejoin, some access points keep a stale association and the
  first rejoin is slow; a reboot with auto-rejoin is fast.
