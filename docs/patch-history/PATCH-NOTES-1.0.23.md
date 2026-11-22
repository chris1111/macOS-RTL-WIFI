# 1.0.23 (diagnostic only)
Publishes the 8188EU firmware's own verdict for DHCP DISCOVER frames: RTWDiagTXDhcpRepOK / RepRetryOver / RepLifetime / RepRetries / RepRate.
Together with App 8.7.20's log dump this shows whether the AP ACKs our data frames (retry_over=0) or never does (retry_over>0).
