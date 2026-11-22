# 1.0.38 (kext 1.0.36, App 8.7.24)
- Constat: 0xe00002d8 = kIOReturnNotReady; usbError_ reste 0 -> le verrou est core_.core_error (pas l'USB).
  Suspect: coex88TimerAction (poll MCU/coex toutes les ~2 s) qui verrouille core_error + quiesce a la 1re erreur.
- Kext 8821CU: tolere 2 echecs de poll coex consecutifs (retry 200 ms), verrouille au 3e.
- Diagnostics: RTWDiagCoreError, RTWDiagCoexErr / CoexErrSite (1 ENXIO,2 core_error,3 SMPS timeout,4 coex_poll,5 coex_watchdog) / CoexErrAfterAttachMS / CoexTickErrors.
