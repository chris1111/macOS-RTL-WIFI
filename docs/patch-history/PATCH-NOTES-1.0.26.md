# 1.0.26 (diagnostic only)
1) RTWDiagCCMPSelfTest = 100 + code, run once at 8188EU attach: encrypts a known frame with the TX CCMP code, decrypts it with the RX code. 100 = pass; 101 setkey, 102 alloc, 103 encrypt failed, 104 wrong length, 105 bad CCMP header, 106 decrypt/MIC failed, 107 plaintext mismatch, 108 wrong length after decrypt.
2) RTWDiagTXProtHex0..2: first three protected (encrypted) data frames as handed to USB: 32-byte TX descriptor | first 48 bytes of the frame (802.11 header + CCMP header + payload start).
Both appear in RTLWiFiBar.log via App 8.7.20 at the next flap.
