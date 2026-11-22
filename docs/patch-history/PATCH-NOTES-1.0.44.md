# 1.0.44 (kext 1.0.41, App 8.7.25)
- Log cffc9136: reboot 2: puce OK (EnableMS=1094) mais AUTH sans reponse en boucle sur plusieurs BSS 5 GHz (hint 44, courant 161), scan complet 9-19 s entre chaque. Le 1.0.38 ne trouvait pas de BSS alternatif (l'arbre ne contient que le canal scanne).
- net80211/kext: executeJoinTail memorise le meilleur canal 2.4 GHz du SSID (ic_join_alt24, RTWDiagJoinAlt24Channel) avant le quiesce; un timeout AUTH/ASSOC sur un BSS 5 GHz met ic_avoid5 et saute sur ce canal; node_choose_bss prefere le 2.4 GHz tant que ic_avoid5; efface en RUN.
