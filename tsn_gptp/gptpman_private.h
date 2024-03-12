/*
 * Copyright (c) 2023 Texas Instruments Incorporated
 * Copyright (c) 2023 Excelfore Corporation (https://excelfore.com)
 *
 * All rights reserved not granted herein.
 * Limited License.
 *
 * Texas Instruments Incorporated grants a world-wide, royalty-free,
 * non-exclusive license under copyrights and patents it now or hereafter
 * owns or controls to make, have made, use, import, offer to sell and sell ("Utilize")
 * this software subject to the terms herein. With respect to the foregoing patent
 * license, such license is granted solely to the extent that any such patent is necessary
 * to Utilize the software alone. The patent license shall not apply to any combinations which
 * include this software, other than combinations with devices manufactured by or for TI ("TI Devices").
 * No hardware patent is licensed hereunder.
 *
 * Redistributions must preserve existing copyright notices and reproduce this license (including the
 * above copyright notice and the disclaimer and (if applicable) source code license limitations below)
 * in the documentation and/or other materials provided with the distribution
 *
 * Redistribution and use in binary form, without modification, are permitted provided that the following
 * conditions are met:
 *
 * * No reverse engineering, decompilation, or disassembly of this software is permitted with respect to any
 * software provided in binary form.
 * * any redistribution and use are licensed by TI for use only with TI Devices.
 * * Nothing shall obligate TI to provide you with source code for the software licensed and provided to you in object code.
 *
 * If software source code is provided to you, modification and redistribution of the source code are permitted
 * provided that the following conditions are met:
 *
 * * any redistribution and use of the source code, including any resulting derivative works, are licensed by
 * TI for use only with TI Devices.
 * * any redistribution and use of any object code compiled from the source code and any resulting derivative
 * works, are licensed by TI for use only with TI Devices.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of its suppliers may be used to endorse or
 * promote products derived from this software without specific prior written permission.
 *
 * DISCLAIMER.
 *
 * THIS SOFTWARE IS PROVIDED BY TI AND TI"S LICENSORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TI AND TI"S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef GPTPMAN_PRIVATE_H_
#define GPTPMAN_PRIVATE_H_

#include "gptpnet.h"
#include "gptpclock.h"
#include "mdeth.h"
#include "mind.h"
#include "md_pdelay_req_sm.h"
#include "md_pdelay_resp_sm.h"
#include "md_sync_receive_sm.h"
#include "md_sync_send_sm.h"
#include "announce_interval_setting_sm.h"
#include "port_sync_sync_receive_sm.h"
#include "port_sync_sync_send_sm.h"
#include "port_announce_receive_sm.h"
#include "port_announce_information_sm.h"
#include "port_announce_information_ext_sm.h"
#include "port_state_selection_sm.h"
#include "port_state_setting_ext_sm.h"
#include "port_announce_transmit_sm.h"
#include "site_sync_sync_sm.h"
#include "clock_master_sync_send_sm.h"
#include "clock_slave_sync_sm.h"
#include "clock_master_sync_receive_sm.h"
#include "clock_master_sync_offset_sm.h"
#include "gptp_capable_transmit_sm.h"
#include "gptp_capable_receive_sm.h"
#include "sync_interval_setting_sm.h"
#include "link_delay_interval_setting_sm.h"
#include "one_step_tx_oper_setting_sm.h"
#include "md_announce_send_sm.h"
#include "md_announce_receive_sm.h"
#include "md_signaling_send_sm.h"
#include "md_signaling_receive_sm.h"
#include "gm_stable_sm.h"
#include "gptpman.h"
#include "md_abnormal_hooks.h"
#include "tsn_unibase/unibase_macros.h"
#include "tsn_uniconf/yangs/ieee1588-ptp-tt_access.h"
#include "gptpconf/gptpgcfg.h"
#include "gptp_perfmon.h"

// per-port data
typedef struct gptpsm_ptd{
	PerPortGlobal *ppglb;
	BmcsPerPortGlobal *bppglb;
	MDEntityGlobal *mdeglb;
	md_pdelay_req_data_t *mdpdreqd;
	md_pdelay_resp_data_t *mdpdrespd;
	md_sync_receive_data_t *mdsrecd;
	md_sync_send_data_t *mdssendd;
	announce_interval_setting_data_t *aisetd;
	port_sync_sync_receive_data_t *pssrecd;
	port_sync_sync_send_data_t *psssendd;
	port_announce_receive_data_t *parecd;
	port_announce_information_data_t *painfd;
	port_announce_information_ext_data_t *paiextd;
	port_announce_transmit_data_t *patransd;
	port_state_setting_ext_data_t *pssextd;
	bool cmldsLinkPortEnabled; // 11.2.16.1
	gptp_capable_transmit_data_t *gctransd;
	gptp_capable_receive_data_t *gcrecd;
	sync_interval_setting_data_t *sisetd;
	link_delay_interval_setting_data_t *ldisetd;
	one_step_tx_oper_setting_data_t *ostxopd;
	md_announce_send_data_t *mdansendd;
	md_announce_receive_data_t *mdanrecd;
	md_signaling_send_data_t *mdsigsendd;
	md_signaling_receive_data_t *mdsigrecd;
} gptpsm_ptd_t;

// per-time-aware-system data
typedef struct gptpsm_tasd{
	gptpsm_ptd_t *ptds;
	PerTimeAwareSystemGlobal *tasglb;
	BmcsPerTimeAwareSystemGlobal *btasglb;
	site_sync_sync_data_t *sssd;
	clock_master_sync_send_data_t *cmssendd;
	clock_slave_sync_data_t *cssd;
	clock_master_sync_receive_data_t *cmsrecd;
	clock_master_sync_offset_data_t *cmsoffsetd;
	port_state_selection_data_t *pssd;
	gm_stable_data_t *gmsd;
	PerPortGlobal **ppglbl;
	BmcsPerPortGlobal **bppglbl;
} gptpsm_tasd_t;

/*
  tasds[0] (0 is always domainIndex=0)
    ptds[0],ptds[1],...,ptds[max_ports]
  tasds[1] (domainIndex is tasds[1].domainIndex)
    ptds[0],ptds[1],...,ptds[max_ports]
  tasds[2] (domainIndex is tasds[2].domainIndex)
    ptds[0],ptds[1],...,ptds[max_ports]
  ...
  portIndex is enumerated, but domain number is not.
*/

// per-entire-system data
struct gptpman_data {
	gptpsm_tasd_t *tasds;
	gptpnet_data_t *gpnetd;
	int max_ports;
	int max_domains;
	uint8_t gptpInstanceIndex; // index number when multiple tsn_gptpd runs, normally 0
};

#endif
