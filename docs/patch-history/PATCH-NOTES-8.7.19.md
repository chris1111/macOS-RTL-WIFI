# App 8.7.19 - settle gate for the startup / post-rescue auto-rejoin

Kext unchanged (1.0.16). One app-side change in App/RTLWiFiBar/AppDelegate.m:
the auto-rejoin no longer submits until the driver has been reachable for
kAutoRejoinSettleSeconds (15 s). The wait does not count toward the
five-survey give-up, and the clock restarts whenever the user client is lost
(rescue re-plug), so the post-rescue rejoin waits too.

Evidence (RTLWiFiBar.log 2026-11-20): AC dongle 1c:61:b4:83:3f:8c joined in 10 s
when picked ~40 s after a cold plug (09:50), but the reboot auto-rejoin
(09:53:57, 1.5 s after attach) and the post-rescue rejoin (09:56:34, 70 ms after
the client reopened) both cycled scan<->auth until "the AP never answered".
The N dongle 50:3e:aa:bf:e1:68 joins in 4 s either way.

Log line to expect: `startup auto-rejoin: waiting 15 s for the dongle to settle ...`
then the usual `startup auto-rejoin: ... is in the survey - joining`.
If the AC dongle still fails after the gate, the cause is chip state, not
timing -> see the ioreg request in the chat.
