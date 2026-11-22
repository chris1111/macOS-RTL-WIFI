# 1.0.37 (kext 1.0.36, App 8.7.24)
- Kext: diagnostic de la PREMIERE panne USB verrouillee (RTWDiagFaultCode/Site/Detail/AfterAttachMS/IcState/Running), copie dans RTLWiFiBar.log. 0xe00002d8 = kIOReturnNotReady : la panne est deja verrouillee avant le join.
  Sites: 1/2 vendor lecture/ecriture (detail=registre), 3 TX, 4 RX recovery, 5-10 RX completion, 11 terminate, 12/13 bulk-out firmware/H2C (detail=length).
