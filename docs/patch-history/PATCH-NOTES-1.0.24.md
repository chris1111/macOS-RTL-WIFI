# 1.0.24 (diagnostic only)
Broadcast DHCP needs no ACK, so RTWDiagTXDhcpRepOK (1.0.23) only proves the firmware transmitted it. 1.0.24 requests the firmware TX report for unicast EAPOL frames (8188EU, handshake M2), which DO need an ACK, and publishes RTWDiagTXEapol{Reports,Pending,RepOK,RepRetryOver,RepLifetime,RepRetries,RepRate}.
retry_over>0 => the AP never ACKs our data frames; RepOK>0 and retry_over==0 => the AP hears them.
These probes are excluded from AMRR rate-control samples.
