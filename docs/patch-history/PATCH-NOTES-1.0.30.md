# Kext 1.0.30 + App 8.7.21
Reverted the 1.0.29 scan hint (the node tree is empty at join time: quiesce wipes it, so it never fired).
Measurement only: kext publishes RTWDiagJoinQuiesceMS, RTWDiagJoinEnableMS, RTWDiagJoinScanMS/AuthMS/AssocMS/RunMS (ms since join request). App logs driver diagnostics at 'connection established'.
