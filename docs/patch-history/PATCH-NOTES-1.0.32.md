# Kext 1.0.32
After a reboot the first AUTH on 5 GHz sometimes gets no answer; net80211 waited 5 s x 3 tries = 15 s before falling back (seen 23:20 log: auth 15 s, then fell back to ch 6). AUTH/ASSOC retry wait is now 1 s (3 s worst case). Probe-request timer untouched.
