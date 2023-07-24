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

#ifndef _H_u_ietf_interfaces
#define _H_u_ietf_interfaces
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
 * @file u_ietf-interfaces.h
 * @brief User SIL-SA header for module ietf-interfaces
 *

*** Generated by yangdump-sdk 21.10-12

<pre>

    User SIL-SA header
    bundle x4ucsilsa
    module ietf-interfaces
    revision 2018-02-20
    prefix if
    namespace urn:ietf:params:xml:ns:yang:ietf-interfaces
    organization IETF NETMOD (Network Modeling) Working Group
    Created: 2023-03-29T06:45:58Z

Online Developer Manual:
https://www.yumaworks.com/pub/21.10/dev/yumapro-dev-manual.html

    CLI parameters:
        defnames true
        deviation excelfore-interfaces
        deviation excelfore-gptp
        format uh
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


/* Feature ietf-interfaces:arbitrary-names
 * Comment out #define to disable */
#define u_if_F_arbitrary_names 1

/* Feature ietf-interfaces:pre-provisioning
 * Comment out #define to disable */
#define u_if_F_pre_provisioning 1

/* Feature ietf-interfaces:if-mib
 * Comment out #define to disable */
#define u_if_F_if_mib 1

#include <xmlstring.h>

#include "procdefs.h"
#include "agt.h"
#include "agt_cb.h"
#include "dlq.h"
#include "ncxtypes.h"
#include "op.h"
#include "rpc.h"
#include "ses.h"
#include "status.h"
#include "val.h"
#include "y_ietf-interfaces.h"
#include "u_ieee802-dot1q-bridge.h"
#include "u_ieee802-dot1q-tsn-config-uni.h"
#include "u_excelfore-interfaces.h"
#include "u_ieee1588-ptp.h"
#include "u_ieee802-dot1as-ptp.h"
#include "u_excelfore-gptp.h"

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
 * @addtogroup silsa-x4ucsilsa-ietf-interfaces Module ietf-interfaces in Bundle x4ucsilsa
 * @ingroup silsa-x4ucsilsa
 *
 * @{
 *
 * YANG instrumentation library.
 * Generated by yangdump-sdk.
 */



/* list /interfaces/interface */
typedef struct y_if_T_interface_ {
    dlq_hdr_t qhdr;
    xmlChar *v_name;
    boolean v_enabled;
    xmlChar *v_oper_status;

#ifdef u_if_F_if_mib
    int32 v_if_index;
#endif /* u_if_F_if_mib */
    xmlChar *v_phys_address;
    uint64 v_speed;
    // y_dot1q_T_bridge_port ieee802_dot1q_bridge_bridge_port;
    xmlChar *excelfore_interfaces_duplex;
} y_if_T_interface;

/* container /interfaces */
typedef struct y_if_T_interfaces_ {
    dlq_hdr_t v_interface;
} y_if_T_interfaces;


/**
 * @brief Get database object callback for leaf oper-status (getcb_fn2_t)\n
 * Path: leaf /interfaces/interface/oper-status\n
 *
 * Fill in 'get2cb' response fields.
 *
 * @param get2cb GET2 control block for the callback.
 * @param k_if_name Ancestor key leaf 'name' in list 'interface'\n
 * Path: /if:interfaces/if:interface/if:name
 * @return return status of the callback.
 */
extern status_t u_if_oper_status_get (
    getcb_get2_t *get2cb,
    const xmlChar *k_if_name);


#ifdef u_if_F_if_mib

/**
 * @brief Get database object callback for leaf if-index (getcb_fn2_t)\n
 * Path: leaf /interfaces/interface/if-index\n
 *
 * Fill in 'get2cb' response fields.
 *
 * @param get2cb GET2 control block for the callback.
 * @param k_if_name Ancestor key leaf 'name' in list 'interface'\n
 * Path: /if:interfaces/if:interface/if:name
 * @return return status of the callback.
 */
extern status_t u_if_if_index_get (
    getcb_get2_t *get2cb,
    const xmlChar *k_if_name);

#endif /* u_if_F_if_mib */


/**
 * @brief Get database object callback for leaf phys-address (getcb_fn2_t)\n
 * Path: leaf /interfaces/interface/phys-address\n
 *
 * Fill in 'get2cb' response fields.
 *
 * @param get2cb GET2 control block for the callback.
 * @param k_if_name Ancestor key leaf 'name' in list 'interface'\n
 * Path: /if:interfaces/if:interface/if:name
 * @return return status of the callback.
 */
extern status_t u_if_phys_address_get (
    getcb_get2_t *get2cb,
    const xmlChar *k_if_name);


/**
 * @brief Get database object callback for leaf speed (getcb_fn2_t)\n
 * Path: leaf /interfaces/interface/speed\n
 *
 * Fill in 'get2cb' response fields.
 *
 * @param get2cb GET2 control block for the callback.
 * @param k_if_name Ancestor key leaf 'name' in list 'interface'\n
 * Path: /if:interfaces/if:interface/if:name
 * @return return status of the callback.
 */
extern status_t u_if_speed_get (
    getcb_get2_t *get2cb,
    const xmlChar *k_if_name);


/**
 * @brief Edit database object callback (agt_cb_fn_t)\n
 * Path: list /interfaces/interface
 *
 * @param scb session control block making the request
 * @param msg message in progress for this edit request
 * @param cbtyp callback type for this callback
 * @param editop the parent edit-config operation type,
 * which is also used for all other callbacks
 * that operate on objects.
 * @param newval container object holding the proposed changes
 * to apply to the current config, depending on
 * the editop value. Will not be NULL.
 * @param curval current container values from the "<running>"
 * or "<candidate>" configuration, if any. Could be NULL
 * for create and other operations.
 * @param k_if_name Local key leaf 'name' in list 'interface'\n
 * Path: /if:interfaces/if:interface/if:name
 * @return return status for the phase.
 */
extern status_t u_if_interface_edit (
    ses_cb_t *scb,
    rpc_msg_t *msg,
    agt_cbtyp_t cbtyp,
    op_editop_t editop,
    val_value_t *newval,
    val_value_t *curval,
    const xmlChar *k_if_name);


/**
 * @brief Edit database object callback (agt_cb_fn_t)\n
 * Path: container /interfaces
 *
 * @param scb session control block making the request
 * @param msg message in progress for this edit request
 * @param cbtyp callback type for this callback
 * @param editop the parent edit-config operation type,
 * which is also used for all other callbacks
 * that operate on objects.
 * @param newval container object holding the proposed changes
 * to apply to the current config, depending on
 * the editop value. Will not be NULL.
 * @param curval current container values from the "<running>"
 * or "<candidate>" configuration, if any. Could be NULL
 * for create and other operations.
 * @return return status for the phase.
 */
extern status_t u_if_interfaces_edit (
    ses_cb_t *scb,
    rpc_msg_t *msg,
    agt_cbtyp_t cbtyp,
    op_editop_t editop,
    val_value_t *newval,
    val_value_t *curval);


/**
 * @brief Phase 1: Initialize the ietf-interfaces server instrumentation library.
 *
 * Called by server when module is loaded.
 *
 * @param modname requested module name to load
 * @param revision requested revision date of the module to load.
 * This may be NULL if the module has no revision statements.
 * @return return status. An error will cause the module load to fail.
 */
extern status_t u_ietf_interfaces_init (
    const xmlChar *modname,
    const xmlChar *revision);


/**
 * @brief Phase 2: Initialize the ietf-interfaces server instrumentation library.
 *
 * SIL init phase 2: non-config data structures.
 * Called after running config is loaded.
 *
 * @return return status. An error will cause the
 * server initialization to fail.
 */
extern status_t u_ietf_interfaces_init2 (void);


/**
 * @brief Cleanup the ietf-interfaces server instrumentation library.
 *
 * Called by server when module is unloaded.
 *
 */
extern void u_ietf_interfaces_cleanup (void);


 /** @} */

#ifdef __cplusplus
} /* end extern 'C' */
#endif

#endif