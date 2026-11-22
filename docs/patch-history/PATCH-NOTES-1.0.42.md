# 1.0.42 (kext 1.0.39, App 8.7.25)
- Log c9a28756: RTWDiagStart88FailStage=2 (bootstrap88), Err=60 (ETIMEDOUT), JoinEnableMS=175. Le 1er bring-up (scan) reussit; le 2e (re-bring-up du join, apres quiesce) expire dans bootstrap88 et verrouille core_error + firmware.last_error -> NotReady jusqu'au replug.
- Kext: executeJoinTail reessaie le bring-up 2 fois (300 ms) pour le 8821CU en effacant core_error et firmware.last_error; attente FW-ready elargie (4000 x 250 us au lieu de 1000 x 10 us).
- Diag: RTWDiagStart88FailSub (sous-etape bootstrap88: 2 identity, 3 mac_prepare, 4 boot_identify, 5 power_on, 6 recheck/residual, 8 0x1a0, 9 fw_download, 10 board, 11 features), RTWDiagJoinBringUpRetries.
