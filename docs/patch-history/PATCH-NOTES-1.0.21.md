# 1.0.21 (diagnostic only)
RTL8188EU: adds RTWTx88E_auth / RTWTx88E_stall IORegistry properties (USB OUT endpoint count, max packet sizes, AC->pipe map, CR, TRXDMA_CTRL, TXPAUSE, RCR, BCN_CTRL, MSR, BSSID, MAC, TXDMA status).
Read: ioreg -l -w0 | grep RTWTx88E
Keeps RTWEfuse88E from 1.0.20. eBay efuse was healthy (id 8129, xtal 0x23, plausible TX power).
