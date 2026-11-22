# Kext 1.0.18 - residual always-on bits experiment (RTL8822BU path)

Field snapshots 2026-11-20 (TP-Link 2357:0115). Failing boot vs good boots:
  start: 0x4c b24, 0x70 b26, 0xb4 b24 set; 0x80 b31 set
  auth : 0x410/0x418/0x41c/0x47c/0x4e0 differ (AUTH config did not apply as in good boots)
The software power cycle (rtw88_boot_power_on) does not clear them; a physical
replug does. 1.0.18 clears 0x4c b24 / 0x70 b26 / 0xb4 b24 right after power-on
(only if they read set) and publishes RTWResidual ("none" or the registers hit).
New snapshots: RTWSnap_pre1/post1 (attach), pre2/post2 (start), start, auth.
