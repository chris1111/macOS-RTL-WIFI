# 1.0.40 (kext 1.0.38, App 8.7.25)
- Log 3dc40b34: apres reboot, jusqu'a 46 s pour se connecter: AUTH sur le canal 5 GHz (hint 157) sans reponse (3x1 s), retour au scan complet (~9 s), le 5 GHz est re-choisi, boucle ~11 s par tour.
- net80211: apres un timeout AUTH/ASSOC, le BSSID fautif est memorise (ic_bad_bssid); node_choose_bss ne l'utilise que si aucun autre BSS du SSID; le scan suivant va directement sur le canal de l'autre BSS (ic_join_chan_hint). Efface au passage en RUN.
- executeJoinTail: le hint de canal deprecie le BSSID fautif.
