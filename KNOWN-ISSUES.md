# Known issues (release 1.1.0)

## RTL8188EU family (rows 11-13 in DONGLES.md): DHCP never completes
- Association and WPA2 handshake succeed; ping to the router works with a
  manual IP, including 1200-byte packets, so CCMP TX/RX and large frames work.
- With DHCP, tcpdump on the interface shows our DISCOVER every ~8 s and no
  server reply, although other traffic (about 150 packets) is received.
- Not the TX path: routing broadcast-DA frames through the firmware-rate path
  (experiment 1.0.58) changed nothing. Auto-heal then resets the link
  ("local reset"), which is a consequence, not the cause.
- Realtek's own RtWlanU kexts (closed source) get an IP on the same dongles.
- Next steps: over-the-air capture (second device in monitor mode); compare
  the 8188E firmware variants shipped by Realtek (rtl8188Efw / EfwS / Efwww).
- Workaround: set a manual IPv4 address.

## RTLWiFiBar
- No automatic rejoin after a mid-session link flap (Forget + rejoin needed).
- Ventura: the connection was seen dropping when opening Network preferences
  (not yet reproduced with a log).
- After Forget + rejoin, some access points keep a stale association and the
  first rejoin is slow; a reboot with auto-rejoin is fast.
