# 1.0.20 (diagnostic only)
RTL8188EU: publishes the efuse image as IORegistry property RTWEfuse88E (xtal, TX power tables, board options, MAC, VID/PID) and logs it.
Read with: ioreg -l -w0 | grep RTWEfuse88E
Purpose: eBay RTL8188EU (0bda:0179, MAC 00:7e:56:6d:f7:d7) scans fine but the AP never answers authentication -> TX/clock suspect.
