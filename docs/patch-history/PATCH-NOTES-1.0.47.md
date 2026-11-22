# 1.0.47 (kext 1.0.44, App 8.7.25)
- Log 1a2edccf: Archer T2U Plus = RTL8821AU (MAC 28:87:ba), "chip did not start": RTWDiagCoreError=5 (EIO) des le 1er scan, famille start8812().
- Kext: start8812() enregistre l'etape en echec (RTWDiagStart88FailStage, Sub=100 = famille 8812/8821AU; 1 core init, 2 alloc, 3 fw_inspect, 4 power_on, 5 dma_prepare, 6 download_fw, 7 mac_init, 8 phy_init, 9 phy_tables, 10 read 0x804, 11 set channel, 12 busInit, 13-15 coex, 16 newstate) + Err.
- enableStation: meme retry (3x, 300 ms) pour 8821AU/8812AU via start8812().
