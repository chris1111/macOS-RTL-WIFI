# Kext 1.0.19 - diagnostics round 3 (rtw88 path: 8822BU, 8821CU)

- New snapshot RTWSnap_stall: taken the first time the station falls back from AUTH to SCAN
  (AP did not answer the auth request). Full dump now also covers 0x700-0xbfc (MAC ext + BB page A).
- pre/post power-on snapshots are short (SYS + MAC/TXDMA) to cut boot time.
- Residual-bit clearing: 0x4c b24 and 0x70 b26 only (0xb4 removed: set in good boots too);
  now also applied to RTL8821CU (cheap dongle snapshot shows the same dirty bits before power-on).
- Read with: ioreg -l -w0 | grep -E "RTWSnap_|RTWResidual" > file
