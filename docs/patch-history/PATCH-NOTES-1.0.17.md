# Kext 1.0.17 - diagnostics only (RTL8821CU/8822BU/8822C path)

No behaviour change. After the boot-time bring-up ("start") and at the first
AUTH state of a join ("auth") the kext publishes a register dump as IORegistry
properties RTWSnap_start / RTWSnap_auth.

Read with:  ioreg -l -w0 | grep -o 'RTWSnap_[a-z]* = "[^"]*"' > ~/Desktop/snap-<case>.txt

Capture two cases and send both files:
  A) cold plug at login (dongle plugged AFTER login, join works)
  B) after a reboot with the dongle already plugged (join fails)
The register diff between A and B shows what the warm chip state keeps.
