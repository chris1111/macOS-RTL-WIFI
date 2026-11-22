# 1.0.43 (kext 1.0.40, App 8.7.25)
- Log 62d2a0d1: le retry du join (1.0.39) a fonctionne (reboot 1: JoinEnableMS=2865, BringUpRetries=1, FailSub=9 = fw_download ETIMEDOUT, connexion en 3 s). Reboot 3: "dongle IS on USB but the driver is not loaded" = echec du bootstrap a l'ATTACH (kext non charge).
- Kext: attach88 reessaie le bootstrap 3 fois (300 ms, power-off + effacement last_error) pour le 8821CU. Diag RTWDiagAttachBringUpRetries.
