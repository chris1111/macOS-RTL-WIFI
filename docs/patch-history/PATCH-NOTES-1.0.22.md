# 1.0.22
- RTL8188EU: the WPA 4-way handshake watchdog window is 30 s instead of 15 s (722N log: link up, no deauth, local reset after 10-17 s, no DHCP lease). Other families unchanged.
- Removed the RTWTx88E_* register reads from the state transition (they were diagnostic only and added USB traffic inside the transition). RTWEfuse88E (read once at attach) stays.
- Kept from 1.0.18/1.0.19: residual always-on bit clearing for 8812BU/8821CU (0x4c b24, 0x70 b26) and the RTWSnap_* diagnostics.
