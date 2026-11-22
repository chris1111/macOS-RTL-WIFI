# Tested dongles (release 1.2.2)

Test machines: HP ProDesk 600 G3 (macOS Tahoe, OpenCore) and Dell OptiPlex 790
(macOS Ventura, OpenCore). Test = reboot once, let the saved network
auto-rejoin, check the log and a valid IP (green dot).

| # | Dongle | Chip family | Status |
|---|--------|-------------|--------|
| 1 | TRENDnet mini | RTL8822BU | OK |
| 2 | Cudy | RTL8822BU | OK |
| 3 | TP-Link Archer T4U | RTL8822BU | OK |
| 4 | TP-Link Archer T2U Plus | RTL8821AU | OK (20 MHz) |
| 5 | Edimax EW-7611UCB (7392:a611) | RTL8723BU | OK (turn Bluetooth off if bluetoothd grabs the device) |
| 6 | MLINK 96581 | RTL8821CU | OK |
| 7 | Generic RTL8811CU (0bda:c811) | RTL8821CU | OK |
| 8 | TP-Link TL-WN823N (2357:0109) | RTL8192EU | OK |
| 9 | TP-Link TL-WN822N | RTL8192CU/EU | OK |
| 10 | TP-Link TL-WN821N (2357:0107) | RTL8192EU | OK on ProDesk and OptiPlex |
| 11 | eBay generic (0bda:0179) | RTL8188EU | Wi-Fi OK, **no DHCP** (manual IP works) |
| 12 | TP-Link TL-WN725N | RTL8188EU | Wi-Fi OK, **no DHCP** (manual IP works) |
| 13 | TP-Link TL-WN722N (2357:010c) | RTL8188EU | Wi-Fi OK, **no DHCP** (manual IP works) |

Typical join time after a reboot: about 1-3 s.
