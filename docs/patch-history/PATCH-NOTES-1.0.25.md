# 1.0.25 (diagnostic only) - everything in one run
Adds to 1.0.24 (EAPOL TX verdict): per-pipe TX frame counts, mgmt vs data TX counts, EAPOL TX details (pipe/qsel/rate/length/QoS), TX report call count and last length (shows unparsed reports),
RX mgmt/data/unicast-to-us/broadcast counts, DHCP episode replies/discovers.
App 8.7.20 copies all RTWDiag* into RTLWiFiBar.log at the next flap/drop/failed join.
