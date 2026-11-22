# 1.0.46 (kext 1.0.43, App 8.7.25)
- Log 63485b21 (kext 1.0.42): 2 reboots sur 3 -> "Adapter chip did not start". CoreError=60, Start88FailStage=2, Sub=9 (fw_download ETIMEDOUT). Le wedge survient AVANT toute connexion: le 1er bring-up est declenche par le scan de l'app (enableStation), pas par executeJoinTail; le retry du 1.0.39 ne le couvrait pas.
- Kext: le retry (3x, 300 ms, efface core_error + firmware.last_error) est deplace DANS enableStation pour le 8821CU (couvre scan, join, rescan). Retire de executeJoinTail.
- Firmware.c: poll() generique 1000x10us -> 4000x50us.
