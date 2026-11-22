# 1.0.45 (kext 1.0.42, App 8.7.25)
- Log 0f6a08f9 (kext 1.0.41): meme avec le saut 2.4 GHz (HintChannel=1), AUTH sans reponse ~40 s sur les deux bandes; connexion finale sur ch 44. Donc pas un probleme de bande.
- Hypothese: apres un reboot a chaud, la puce garde son MAC actif et l'AP garde l'ancienne association; il ignore le nouvel AUTH tant qu'elle n'a pas expire.
- net80211: un DEAUTH (AUTH_LEAVE) est envoye avant chaque AUTH SCAN->AUTH.
