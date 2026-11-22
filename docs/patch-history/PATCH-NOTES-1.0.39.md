# 1.0.39 (kext 1.0.37, App 8.7.25)
- Constat 003fbc0c log: apres reboot la puce 8821CU "ne demarre pas" des le 1er scan (0xe00002d8 = NotReady, core_error verrouille par le 1er bring-up rate); revient apres changement de port.
- Kext: start88() enregistre l'etape qui echoue (RTWDiagStart88FailStage 1..13: 1 alloc, 2 bootstrap88, 3 mac_init, 4 boot tables, 5 radio init, 6 h2c seq, 8 station init, 9 set channel, 10 busInit, 11 startup, 12 coex start, 13 newstate), l'erreur (RTWDiagStart88FailErr, errno) et le nombre d'echecs.
- App: journalise "driver diagnostics at wedge" au moment ou le blocage est detecte.
