# Kext 1.0.31 + App 8.7.21
Measured on RTL8822BU: bring-up 1.6 s, then 8.3 s scanning 36 channels (network on ch 157), auth/assoc ~0.01 s.
Kext: join scans the target's channel first (hint taken from the live survey before quiesce; consumed on first pass, full sweep fallback).
App: auto-rejoin settle gate 15 s -> 5 s and a survey right when it expires (was: wait for the next 20 s tick).
