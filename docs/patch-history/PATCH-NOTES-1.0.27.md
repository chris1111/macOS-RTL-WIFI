# Kext 1.0.27 (app unchanged, 8.7.20)
8188EU: DW1 bit6 (AGG_BREAK on 92C) was left set on every data frame that carried no TX-report request. On the 88E that bit belongs to the MACID field (invalid MACID 64) and AGG_BREAK is DW2 bit16. Now moved for all 8188EU frames.
