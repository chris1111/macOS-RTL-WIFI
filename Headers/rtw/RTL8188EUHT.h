/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef RTW_88E_HT_H
#define RTW_88E_HT_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/* 8188E firmware H2C command IDs (pinned lwfinger/rtl8188eu
 * include/rtl8188e_cmd.h, enum RTL8188E_H2C_CMD_ID). The numbers match the
 * generation-2 RTL8192EU commands; the payload layouts do not. */
#define RTW_88E_H2C_MEDIA_STATUS 0x01U
#define RTW_88E_H2C_MACID_CONFIG 0x40U
#define RTW_88E_MACID_CONFIG_BYTES 3U
#define RTW_88E_MEDIA_STATUS_BYTES 2U
/* 8188E RA bitmap as the firmware sees it (24 bits reach the H2C):
 * CCK occupies bits 0..3, OFDM bits 4..11 and the single spatial stream
 * MCS0..7 bits 12..19. The firmware infers the mode class from the bitmap
 * itself; the vendor RAID nibble above bit 28 is not transmitted. */
#define RTW_88E_RA_MCS_SHIFT 12U
/* Vendor RATR_INX_WIRELESS_* class numbering (pinned lwfinger
 * include/ieee80211.h and networktype_to_raid): the bitmap class the
 * firmware TXDESC RATE_ID carries. */
#define RTW_88E_RAID_NGB 0U /* CCK+OFDM+HT. */
#define RTW_88E_RAID_NG 1U  /* OFDM+HT. */
#define RTW_88E_RAID_N 3U   /* HT only. */
#define RTW_88E_RAID_GB 4U
#define RTW_88E_RAID_G 5U
#define RTW_88E_RAID_B 6U
typedef struct {
    uint32_t rate_mask; /* bits 0..23 only. */
    uint8_t raid;       /* RATR_INX class, RTW_88E_RAID_*. */
    uint8_t sgi20, ht, initial_rate;
} rtw_88e_rate_config;
/* legacy_rates is the peer's negotiated hardware-index mask (CCK0..3,
 * OFDM4..11). peer_mcs[0] is the peer's single-stream MCS support; this
 * device transmits one spatial stream, so peer_mcs[1] is ignored.
 * ht_negotiated must reflect the completed association, not a beacon IE,
 * and short guard interval requires agreement of both sides. */
int rtw_88e_build_rate_config(uint16_t legacy_rates,
    const uint8_t peer_mcs[2], int ht_negotiated,
    int local_sgi20, int peer_sgi20, rtw_88e_rate_config *out);
/* Payloads exclude the H2C ID. MACID 0 is the AP peer for this STA port
 * (pinned lwfinger core/rtw_mlme_ext.c: WIFI_FW_STATION_STATE uses
 * mac_id 0). make_macid_config re-derives and verifies the configuration
 * before emitting the three little-endian bitmap bytes. */
int rtw_88e_make_macid_config(const rtw_88e_rate_config *c,
    uint8_t out[RTW_88E_MACID_CONFIG_BYTES]);
int rtw_88e_make_media_status(int connect, uint8_t macid,
    uint8_t out[RTW_88E_MEDIA_STATUS_BYTES]);
#ifdef __cplusplus
}
#endif
#endif
