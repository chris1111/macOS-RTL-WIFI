/* SPDX-License-Identifier: GPL-2.0-only */
/* 8188E firmware rate control. The three little-endian bitmap bytes of
 * H2C_DM_MACID_CFG follow pinned lwfinger/rtl8188eu
 * rtl8188e_set_raid_cmd (hal/rtl8188e_cmd.c), and the MACID<<8|opmode
 * media status follows rtl8188e_set_FwMediaStatus_cmd as called from
 * core/rtw_mlme.c (STA mac_id 0, connect=1 / disconnect=0). The RAID
 * class values are the vendor RATR_INX_WIRELESS_* numbering selected by
 * networktype_to_raid (core/rtw_wlan_util.c). Re-derived reference, not
 * imported code: no lwfinger source line is copied. */
#include "rtw/RTL8188EUHT.h"
#include <string.h>

int rtw_88e_build_rate_config(uint16_t legacy,const uint8_t peer_mcs[2],
    int ht,int local_sgi20,int peer_sgi20,rtw_88e_rate_config *out)
{
    uint8_t mcs=0;
    unsigned i;
    if(!out) return 0;
    memset(out,0,sizeof(*out));
    if(!legacy || (legacy&~0x0fffU) ||
       (ht!=0 && ht!=1) ||
       (local_sgi20!=0 && local_sgi20!=1) ||
       (peer_sgi20!=0 && peer_sgi20!=1)) return 0;
    /* An HT peer always offers OFDM rates; without them the association
     * is not 802.11n and the firmware class would be undefined. A
     * negotiated HT node also always carries its MCS set. */
    if(ht && (!(legacy&0x0ff0U) || !peer_mcs)) return 0;
    if(ht) mcs=peer_mcs[0];
    out->rate_mask=legacy|((uint32_t)mcs<<RTW_88E_RA_MCS_SHIFT);
    out->ht=mcs!=0;
    out->sgi20=(uint8_t)(out->ht && local_sgi20 && peer_sgi20);
    if(out->ht)
        out->raid=(uint8_t)((legacy&0x0fU)?RTW_88E_RAID_NGB:RTW_88E_RAID_NG);
    else
        out->raid=(uint8_t)((legacy&0x0fU)?
            ((legacy&0x0ff0U)?RTW_88E_RAID_GB:RTW_88E_RAID_B):RTW_88E_RAID_G);
    for(i=0;i<24U;i++) if(out->rate_mask&((uint32_t)1U<<i))
        out->initial_rate=(uint8_t)i;
    return 1;
}

int rtw_88e_make_macid_config(const rtw_88e_rate_config *c,uint8_t out[3])
{
    rtw_88e_rate_config verified;
    uint8_t mcs[2]={0,0};
    if(!c || !out || !c->rate_mask || (c->rate_mask&0xff000000U) ||
       c->ht>1U || c->sgi20>1U || c->raid>6U || c->raid==2U ||
       (!c->ht && (c->sgi20 || (c->rate_mask&0x0fff000U) ||
                   (c->raid!=RTW_88E_RAID_GB && c->raid!=RTW_88E_RAID_G &&
                    c->raid!=RTW_88E_RAID_B))) ||
       (c->ht && (c->raid!=RTW_88E_RAID_NGB && c->raid!=RTW_88E_RAID_NG &&
                  c->raid!=RTW_88E_RAID_N))) return 0;
    mcs[0]=(uint8_t)(c->rate_mask>>RTW_88E_RA_MCS_SHIFT);
    if(!rtw_88e_build_rate_config((uint16_t)(c->rate_mask&0xfffU),mcs,
       c->ht,c->sgi20,c->sgi20,&verified) ||
       c->rate_mask!=verified.rate_mask || c->raid!=verified.raid ||
       c->ht!=verified.ht || c->sgi20!=verified.sgi20 ||
       c->initial_rate!=verified.initial_rate) return 0;
    out[0]=(uint8_t)c->rate_mask;
    out[1]=(uint8_t)(c->rate_mask>>8);
    out[2]=(uint8_t)(c->rate_mask>>16);
    return 1;
}

int rtw_88e_make_media_status(int connect,uint8_t macid,uint8_t out[2])
{
    if(!out || macid>31U || (connect!=0 && connect!=1)) return 0;
    out[0]=(uint8_t)connect;
    out[1]=macid;
    return 1;
}
