# App 8.7.20 (kext stays 1.0.22)
RTLWiFiBar now writes the kext's diagnostic IORegistry properties (RTWDiag*, RTWSnap_*, RTWEfuse88E, RTWResidual, RTWStart*) into RTLWiFiBar.log on every link flap, link drop and failed join:
  "driver diagnostics at flap: ..." / "at drop: ..." / "at join failure: ..."
Zero-valued counters are omitted. One reproduction -> one file; no separate ioreg commands.
