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

#ifndef _H_y_ieee802_dot1as_ptp
#define _H_y_ieee802_dot1as_ptp
/*
 * Copyright (c) 2008 - 2012, Andy Bierman, All Rights Reserved.
 * Copyright (c) 2012 - 2022, YumaWorks, Inc., All Rights Reserved.
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

/**
 * @file y_ieee802-dot1as-ptp.h
 * @brief YumaPro SIL-SA header for module ieee802-dot1as-ptp
 *

*** Generated by yangdump-sdk 21.10-12

<pre>

    YumaPro SIL-SA header
    bundle x4ucsilsa
    module ieee802-dot1as-ptp
    revision 2022-12-14
    prefix dot1as-ptp
    namespace urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp
    organization IEEE 802.1 Working Group
    Created: 2023-03-29T06:45:58Z

Online Developer Manual:
https://www.yumaworks.com/pub/21.10/dev/yumapro-dev-manual.html

    CLI parameters:
        defnames true
        deviation excelfore-interfaces
        deviation excelfore-gptp
        format yh
        indent 4
        module ietf-interfaces
        module ieee802-dot1q-bridge
        module ieee802-dot1q-tsn-config-uni
        module excelfore-interfaces
        module ieee1588-ptp
        module ieee802-dot1as-ptp
        module excelfore-gptp
        sil-bundle x4ucsilsa
        sil-edit2
        sil-get2
        sil-include tsn_uniconf/yangs/yang_db_access.h
        sil-include tsn_uniconf/yangs/yang_modules.h
        sil-sa
        unified true
</pre>

 */

#include <xmlstring.h>

#include "procdefs.h"
#include "dlq.h"
#include "ncxtypes.h"
#include "op.h"
#include "status.h"
#include "val.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup yang-library YANG Library
 *
 * Collection of modules supported by the server.
 * Generated by yangdump-sdk.
 */



/**
 * @addtogroup silsa-x4ucsilsa-ieee802-dot1as-ptp Module ieee802-dot1as-ptp in Bundle x4ucsilsa
 * @ingroup silsa-x4ucsilsa
 *
 * @{
 *
 * YANG instrumentation library.
 * Generated by yangdump-sdk.
 */



#define y_dot1as_ptp_M_dot1as_ptp (const xmlChar *)"ieee802-dot1as-ptp"

#define y_dot1as_ptp_R_dot1as_ptp (const xmlChar *)"2022-12-14"

#define y_dot1as_ptp_N_allowed_faults (const xmlChar *)"allowed-faults"
#define y_dot1as_ptp_N_allowed_lost_responses (const xmlChar *)"allowed-lost-responses"
#define y_dot1as_ptp_N_as_capable (const xmlChar *)"as-capable"
#define y_dot1as_ptp_N_current_log_announce_interval (const xmlChar *)"current-log-announce-interval"
#define y_dot1as_ptp_N_current_log_gptp_cap_interval (const xmlChar *)"current-log-gptp-cap-interval"
#define y_dot1as_ptp_N_current_log_pdelay_req_interval (const xmlChar *)"current-log-pdelay-req-interval"
#define y_dot1as_ptp_N_current_log_sync_interval (const xmlChar *)"current-log-sync-interval"
#define y_dot1as_ptp_N_current_one_step_tx_oper (const xmlChar *)"current-one-step-tx-oper"
#define y_dot1as_ptp_N_current_utc_offset (const xmlChar *)"current-utc-offset"
#define y_dot1as_ptp_N_current_utc_offset_valid (const xmlChar *)"current-utc-offset-valid"
#define y_dot1as_ptp_N_frequency_traceable (const xmlChar *)"frequency-traceable"
#define y_dot1as_ptp_N_gm_capable (const xmlChar *)"gm-capable"
#define y_dot1as_ptp_N_gm_timebase_indicator (const xmlChar *)"gm-timebase-indicator"
#define y_dot1as_ptp_N_gptp_cap_receipt_timeout (const xmlChar *)"gptp-cap-receipt-timeout"
#define y_dot1as_ptp_N_initial_log_announce_interval (const xmlChar *)"initial-log-announce-interval"
#define y_dot1as_ptp_N_initial_log_pdelay_req_interval (const xmlChar *)"initial-log-pdelay-req-interval"
#define y_dot1as_ptp_N_initial_log_sync_interval (const xmlChar *)"initial-log-sync-interval"
#define y_dot1as_ptp_N_initial_one_step_tx_oper (const xmlChar *)"initial-one-step-tx-oper"
#define y_dot1as_ptp_N_is_measuring_delay (const xmlChar *)"is-measuring-delay"
#define y_dot1as_ptp_N_last_gm_freq_change (const xmlChar *)"last-gm-freq-change"
#define y_dot1as_ptp_N_last_gm_phase_change (const xmlChar *)"last-gm-phase-change"
#define y_dot1as_ptp_N_leap59 (const xmlChar *)"leap59"
#define y_dot1as_ptp_N_leap61 (const xmlChar *)"leap61"
#define y_dot1as_ptp_N_mean_link_delay_thresh (const xmlChar *)"mean-link-delay-thresh"
#define y_dot1as_ptp_N_mgt_log_pdelay_req_interval (const xmlChar *)"mgt-log-pdelay-req-interval"
#define y_dot1as_ptp_N_mgt_one_step_tx_oper (const xmlChar *)"mgt-one-step-tx-oper"
#define y_dot1as_ptp_N_neighbor_rate_ratio (const xmlChar *)"neighbor-rate-ratio"
#define y_dot1as_ptp_N_one_step_receive (const xmlChar *)"one-step-receive"
#define y_dot1as_ptp_N_one_step_transmit (const xmlChar *)"one-step-transmit"
#define y_dot1as_ptp_N_one_step_tx_oper (const xmlChar *)"one-step-tx-oper"
#define y_dot1as_ptp_N_pdelay_truncated_timestamps (const xmlChar *)"pdelay-truncated-timestamps"
#define y_dot1as_ptp_N_port_statistics_ds (const xmlChar *)"port-statistics-ds"
#define y_dot1as_ptp_N_ptp_timescale (const xmlChar *)"ptp-timescale"
#define y_dot1as_ptp_N_rx_announce_count (const xmlChar *)"rx-announce-count"
#define y_dot1as_ptp_N_rx_follow_up_count (const xmlChar *)"rx-follow-up-count"
#define y_dot1as_ptp_N_rx_one_step_sync_count (const xmlChar *)"rx-one-step-sync-count"
#define y_dot1as_ptp_N_rx_pdelay_req_count (const xmlChar *)"rx-pdelay-req-count"
#define y_dot1as_ptp_N_rx_pdelay_resp_count (const xmlChar *)"rx-pdelay-resp-count"
#define y_dot1as_ptp_N_rx_pdelay_resp_follow_up_count (const xmlChar *)"rx-pdelay-resp-follow-up-count"
#define y_dot1as_ptp_N_rx_sync_count (const xmlChar *)"rx-sync-count"
#define y_dot1as_ptp_N_sync_receipt_timeout (const xmlChar *)"sync-receipt-timeout"
#define y_dot1as_ptp_N_sync_receipt_timeout_interval (const xmlChar *)"sync-receipt-timeout-interval"
#define y_dot1as_ptp_N_time_of_last_freq_change (const xmlChar *)"time-of-last-freq-change"
#define y_dot1as_ptp_N_time_source (const xmlChar *)"time-source"
#define y_dot1as_ptp_N_time_traceable (const xmlChar *)"time-traceable"
#define y_dot1as_ptp_N_tx_announce_count (const xmlChar *)"tx-announce-count"
#define y_dot1as_ptp_N_tx_follow_up_count (const xmlChar *)"tx-follow-up-count"
#define y_dot1as_ptp_N_tx_pdelay_req_count (const xmlChar *)"tx-pdelay-req-count"
#define y_dot1as_ptp_N_tx_pdelay_resp_count (const xmlChar *)"tx-pdelay-resp-count"
#define y_dot1as_ptp_N_tx_pdelay_resp_follow_up_count (const xmlChar *)"tx-pdelay-resp-follow-up-count"
#define y_dot1as_ptp_N_tx_sync_count (const xmlChar *)"tx-sync-count"
#define y_dot1as_ptp_N_use_mgt_compute_mean_link_delay (const xmlChar *)"use-mgt-compute-mean-link-delay"
#define y_dot1as_ptp_N_use_mgt_compute_rate_ratio (const xmlChar *)"use-mgt-compute-rate-ratio"
#define y_dot1as_ptp_N_use_mgt_log_announce_interval (const xmlChar *)"use-mgt-log-announce-interval"
#define y_dot1as_ptp_N_use_mgt_log_pdelay_req_interval (const xmlChar *)"use-mgt-log-pdelay-req-interval"
#define y_dot1as_ptp_N_use_mgt_log_sync_interval (const xmlChar *)"use-mgt-log-sync-interval"
#define y_dot1as_ptp_N_use_mgt_one_step_tx_oper (const xmlChar *)"use-mgt-one-step-tx-oper"



/**
 * @brief Phase 1: Initialize the ieee802-dot1as-ptp server instrumentation library.
 *
 * Called by server when module is loaded.
 *
 * @param modname requested module name to load
 * @param revision requested revision date of the module to load.
 * This may be NULL if the module has no revision statements.
 * @return return status. An error will cause the module load to fail.
 */
extern status_t y_ieee802_dot1as_ptp_init (
    const xmlChar *modname,
    const xmlChar *revision);


/**
 * @brief Phase 2: Initialize the ieee802-dot1as-ptp server instrumentation library.
 *
 * SIL init phase 2: non-config data structures.
 * Called after running config is loaded.
 *
 * @return return status. An error will cause the
 * server initialization to fail.
 */
extern status_t y_ieee802_dot1as_ptp_init2 (void);


/**
 * @brief Cleanup the ieee802-dot1as-ptp server instrumentation library.
 *
 * Called by server when module is unloaded.
 *
 */
extern void y_ieee802_dot1as_ptp_cleanup (void);


 /** @} */

#ifdef __cplusplus
} /* end extern 'C' */
#endif

#endif