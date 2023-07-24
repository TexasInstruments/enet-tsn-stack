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
 * @file y_ietf-interfaces.c
 * @brief YumaPro SIL-SA module for module ietf-interfaces
 *

*** Generated by yangdump-sdk 21.10-12

<pre>

    YumaPro SIL-SA module
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
        format yc
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

  Short Name Mappings
    admin_status = /interfaces/interface/admin-status
    admin_status_1 = /interfaces-state/interface/admin-status
    description = /interfaces/interface/description
    discontinuity_time = /interfaces/interface/statistics/discontinuity-time
    discontinuity_time_1 = /interfaces-state/interface/statistics/discontinuity-time
    enabled = /interfaces/interface/enabled
    higher_layer_if = /interfaces/interface/higher-layer-if
    higher_layer_if_1 = /interfaces-state/interface/higher-layer-if
    if_index = /interfaces/interface/if-index
    if_index_1 = /interfaces-state/interface/if-index
    in_broadcast_pkts = /interfaces/interface/statistics/in-broadcast-pkts
    in_broadcast_pkts_1 = /interfaces-state/interface/statistics/in-broadcast-pkts
    in_discards = /interfaces/interface/statistics/in-discards
    in_discards_1 = /interfaces-state/interface/statistics/in-discards
    in_errors = /interfaces/interface/statistics/in-errors
    in_errors_1 = /interfaces-state/interface/statistics/in-errors
    in_multicast_pkts = /interfaces/interface/statistics/in-multicast-pkts
    in_multicast_pkts_1 = /interfaces-state/interface/statistics/in-multicast-pkts
    in_octets = /interfaces/interface/statistics/in-octets
    in_octets_1 = /interfaces-state/interface/statistics/in-octets
    in_unicast_pkts = /interfaces/interface/statistics/in-unicast-pkts
    in_unicast_pkts_1 = /interfaces-state/interface/statistics/in-unicast-pkts
    in_unknown_protos = /interfaces/interface/statistics/in-unknown-protos
    in_unknown_protos_1 = /interfaces-state/interface/statistics/in-unknown-protos
    interface = /interfaces/interface
    interface_1 = /interfaces-state/interface
    interfaces = /interfaces
    interfaces_state = /interfaces-state
    last_change = /interfaces/interface/last-change
    last_change_1 = /interfaces-state/interface/last-change
    link_up_down_trap_enable = /interfaces/interface/link-up-down-trap-enable
    lower_layer_if = /interfaces/interface/lower-layer-if
    lower_layer_if_1 = /interfaces-state/interface/lower-layer-if
    name = /interfaces/interface/name
    name_1 = /interfaces-state/interface/name
    oper_status = /interfaces/interface/oper-status
    oper_status_1 = /interfaces-state/interface/oper-status
    out_broadcast_pkts = /interfaces/interface/statistics/out-broadcast-pkts
    out_broadcast_pkts_1 = /interfaces-state/interface/statistics/out-broadcast-pkts
    out_discards = /interfaces/interface/statistics/out-discards
    out_discards_1 = /interfaces-state/interface/statistics/out-discards
    out_errors = /interfaces/interface/statistics/out-errors
    out_errors_1 = /interfaces-state/interface/statistics/out-errors
    out_multicast_pkts = /interfaces/interface/statistics/out-multicast-pkts
    out_multicast_pkts_1 = /interfaces-state/interface/statistics/out-multicast-pkts
    out_octets = /interfaces/interface/statistics/out-octets
    out_octets_1 = /interfaces-state/interface/statistics/out-octets
    out_unicast_pkts = /interfaces/interface/statistics/out-unicast-pkts
    out_unicast_pkts_1 = /interfaces-state/interface/statistics/out-unicast-pkts
    phys_address = /interfaces/interface/phys-address
    phys_address_1 = /interfaces-state/interface/phys-address
    speed = /interfaces/interface/speed
    speed_1 = /interfaces-state/interface/speed
    statistics = /interfaces/interface/statistics
    statistics_1 = /interfaces-state/interface/statistics
    type = /interfaces/interface/type
    type_1 = /interfaces-state/interface/type
</pre>

 */

#include <xmlstring.h>

#include "procdefs.h"
#include "agt.h"
#include "agt_action.h"
#include "agt_cb.h"
#include "agt_cfg.h"
#include "agt_rpc.h"
#include "agt_sil_lib.h"
#include "agt_timer.h"
#include "agt_util.h"
#include "agt_val.h"
#include "cfg.h"
#include "dlq.h"
#include "getcb.h"
#include "ncx.h"
#include "ncx_feature.h"
#include "ncxmod.h"
#include "ncxtypes.h"
#include "obj.h"
#include "rpc.h"
#include "sil_sa.h"
#include "status.h"
#include "val.h"
#include "val_child.h"
#include "val_util.h"
#include "xml_val.h"

#include "tsn_uniconf/yangs/yang_db_access.h"
#include "tsn_uniconf/yangs/yang_modules.h"


/* SIL bundle: x4ucsilsa */
#include "y_ietf-interfaces.h"
#include "u_ietf-interfaces.h"
#include "y_ieee802-dot1q-bridge.h"
#include "u_ieee802-dot1q-bridge.h"
#include "y_ieee802-dot1q-tsn-config-uni.h"
#include "u_ieee802-dot1q-tsn-config-uni.h"
#include "y_excelfore-interfaces.h"
#include "u_excelfore-interfaces.h"
#include "y_ieee1588-ptp.h"
#include "u_ieee1588-ptp.h"
#include "y_ieee802-dot1as-ptp.h"
#include "u_ieee802-dot1as-ptp.h"
#include "y_excelfore-gptp.h"
#include "u_excelfore-gptp.h"


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


/* module static variables */
static ncx_module_t *ietf_interfaces_mod;
static obj_template_t *interfaces_obj;
static val_value_t *interfaces_val;

/**
 * @brief Initialize module static variables
 *
 * Called by server when module is loaded.
 */
static void y_if_init_static_vars (void)
{
    ietf_interfaces_mod = NULL;
    interfaces_obj = NULL;
    interfaces_val = NULL;

} /* y_if_init_static_vars */


/**
 * @brief Get database object callback for leaf oper-status (getcb_fn2_t)\n
 * Path: leaf /interfaces/interface/oper-status\n
 *
 * Fill in 'get2cb' response fields.
 *
 * @param scb session control block making the request.
 * @param msg message in progress for this retrieval request.
 * @param get2cb GET2 control block for the callback.
 * @return return status of the callback.
 */
static status_t oper_status_get (
    ses_cb_t *scb,
    xml_msg_hdr_t *msg,
    getcb_get2_t *get2cb)
{

    if (LOGDEBUG) {
        log_debug("\nEnter oper_status_get");
    }

    (void)scb;
    (void)msg;

    val_value_t *keyval = NULL;

    /* ancestor key ietf-interfaces:name */
    const xmlChar *k_if_name = 0;
    keyval = getcb_find_key_lvl(get2cb,
        y_if_M_if,
        y_if_N_name,
        (uint32)3);
    if (keyval) {
        k_if_name = VAL_STRING(keyval);
    }

    return u_if_oper_status_get(
        get2cb,
        k_if_name);

} /* oper_status_get */


#ifdef u_if_F_if_mib

/**
 * @brief Get database object callback for leaf if-index (getcb_fn2_t)\n
 * Path: leaf /interfaces/interface/if-index\n
 *
 * Fill in 'get2cb' response fields.
 *
 * @param scb session control block making the request.
 * @param msg message in progress for this retrieval request.
 * @param get2cb GET2 control block for the callback.
 * @return return status of the callback.
 */
static status_t if_index_get (
    ses_cb_t *scb,
    xml_msg_hdr_t *msg,
    getcb_get2_t *get2cb)
{

    if (LOGDEBUG) {
        log_debug("\nEnter if_index_get");
    }

    (void)scb;
    (void)msg;

    val_value_t *keyval = NULL;

    /* ancestor key ietf-interfaces:name */
    const xmlChar *k_if_name = 0;
    keyval = getcb_find_key_lvl(get2cb,
        y_if_M_if,
        y_if_N_name,
        (uint32)3);
    if (keyval) {
        k_if_name = VAL_STRING(keyval);
    }

    return u_if_if_index_get(
        get2cb,
        k_if_name);

} /* if_index_get */

#endif /* u_if_F_if_mib */


/**
 * @brief Get database object callback for leaf phys-address (getcb_fn2_t)\n
 * Path: leaf /interfaces/interface/phys-address\n
 *
 * Fill in 'get2cb' response fields.
 *
 * @param scb session control block making the request.
 * @param msg message in progress for this retrieval request.
 * @param get2cb GET2 control block for the callback.
 * @return return status of the callback.
 */
static status_t phys_address_get (
    ses_cb_t *scb,
    xml_msg_hdr_t *msg,
    getcb_get2_t *get2cb)
{

    if (LOGDEBUG) {
        log_debug("\nEnter phys_address_get");
    }

    (void)scb;
    (void)msg;

    val_value_t *keyval = NULL;

    /* ancestor key ietf-interfaces:name */
    const xmlChar *k_if_name = 0;
    keyval = getcb_find_key_lvl(get2cb,
        y_if_M_if,
        y_if_N_name,
        (uint32)3);
    if (keyval) {
        k_if_name = VAL_STRING(keyval);
    }

    return u_if_phys_address_get(
        get2cb,
        k_if_name);

} /* phys_address_get */


/**
 * @brief Get database object callback for leaf speed (getcb_fn2_t)\n
 * Path: leaf /interfaces/interface/speed\n
 *
 * Fill in 'get2cb' response fields.
 *
 * @param scb session control block making the request.
 * @param msg message in progress for this retrieval request.
 * @param get2cb GET2 control block for the callback.
 * @return return status of the callback.
 */
static status_t speed_get (
    ses_cb_t *scb,
    xml_msg_hdr_t *msg,
    getcb_get2_t *get2cb)
{

    if (LOGDEBUG) {
        log_debug("\nEnter speed_get");
    }

    (void)scb;
    (void)msg;

    val_value_t *keyval = NULL;

    /* ancestor key ietf-interfaces:name */
    const xmlChar *k_if_name = 0;
    keyval = getcb_find_key_lvl(get2cb,
        y_if_M_if,
        y_if_N_name,
        (uint32)3);
    if (keyval) {
        k_if_name = VAL_STRING(keyval);
    }

    return u_if_speed_get(
        get2cb,
        k_if_name);

} /* speed_get */


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
 * @return return status for the phase.
 */
static status_t interface_edit (
    ses_cb_t *scb,
    rpc_msg_t *msg,
    agt_cbtyp_t cbtyp,
    op_editop_t editop,
    val_value_t *newval,
    val_value_t *curval)
{
    status_t res = NO_ERR;
    val_value_t *errorval = (curval) ? curval : newval;
    uint32 cur_errs = agt_get_error_count(msg);

    /* local key ietf-interfaces:name */
    const xmlChar *k_if_name =
        VAL_STRING(sil_sa_get_key(msg, 1));

    if (LOGDEBUG) {
        log_debug("\nEnter interface_edit callback for %s phase",
            agt_cbtype_name(cbtyp));
    }

    res = u_if_interface_edit(scb, msg, cbtyp, editop, newval, curval,
        k_if_name);

    if ((res != NO_ERR) && (cur_errs == agt_get_error_count(msg))) {
        agt_record_error(
            scb,
            &msg->mhdr,
            NCX_LAYER_CONTENT,
            res,
            NULL,
            (errorval) ? NCX_NT_VAL : NCX_NT_NONE,
            errorval,
            (errorval) ? NCX_NT_VAL : NCX_NT_NONE,
            errorval);
    }
    return res;

} /* interface_edit */


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
static status_t interfaces_edit (
    ses_cb_t *scb,
    rpc_msg_t *msg,
    agt_cbtyp_t cbtyp,
    op_editop_t editop,
    val_value_t *newval,
    val_value_t *curval)
{
    status_t res = NO_ERR;
    val_value_t *errorval = (curval) ? curval : newval;
    uint32 cur_errs = agt_get_error_count(msg);

    if (LOGDEBUG) {
        log_debug("\nEnter interfaces_edit callback for %s phase",
            agt_cbtype_name(cbtyp));
    }

    res = u_if_interfaces_edit(scb, msg, cbtyp, editop, newval, curval);

    if (res == NO_ERR && cbtyp == AGT_CB_COMMIT) {
        res = agt_check_cache(&interfaces_val, newval, curval, editop);
    }


    if ((res != NO_ERR) && (cur_errs == agt_get_error_count(msg))) {
        agt_record_error(
            scb,
            &msg->mhdr,
            NCX_LAYER_CONTENT,
            res,
            NULL,
            (errorval) ? NCX_NT_VAL : NCX_NT_NONE,
            errorval,
            (errorval) ? NCX_NT_VAL : NCX_NT_NONE,
            errorval);
    }
    return res;

} /* interfaces_edit */


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
status_t y_ietf_interfaces_init (
    const xmlChar *modname,
    const xmlChar *revision)
{
    status_t res = NO_ERR;

    if (LOGDEBUG) {
        log_debug("\nEnter y_ietf_interfaces_init");
    }


    y_if_init_static_vars();

    /* change if custom handling done */
    if (xml_strcmp(modname, y_if_M_if)) {
        return ERR_NCX_UNKNOWN_MODULE;
    }

    if (revision && xml_strcmp(revision, y_if_R_if)) {
        return ERR_NCX_WRONG_VERSION;
    }

    /* Feature: ietf-interfaces:arbitrary-names */
    res = ncx_set_feature_enable(
        y_if_M_if,
        (const xmlChar *)"arbitrary-names",
#ifdef u_if_F_arbitrary_names
        true);
#else
        false);
#endif
    if (res != NO_ERR) {
        return res;
    }

    /* Feature: ietf-interfaces:pre-provisioning */
    res = ncx_set_feature_enable(
        y_if_M_if,
        (const xmlChar *)"pre-provisioning",
#ifdef u_if_F_pre_provisioning
        true);
#else
        false);
#endif
    if (res != NO_ERR) {
        return res;
    }

    /* Feature: ietf-interfaces:if-mib */
    res = ncx_set_feature_enable(
        y_if_M_if,
        (const xmlChar *)"if-mib",
#ifdef u_if_F_if_mib
        true);
#else
        false);
#endif
    if (res != NO_ERR) {
        return res;
    }


    res = ncxmod_load_module(
        y_if_M_if,
        y_if_R_if,
        agt_get_savedevQ(),
        &ietf_interfaces_mod);
    if (res != NO_ERR) {
        return res;
    }

    interfaces_obj = ncx_find_object(
        ietf_interfaces_mod,
        y_if_N_interfaces);
    if (interfaces_obj == NULL) {
        return ERR_NCX_DEF_NOT_FOUND;
    }

    res = agt_cb_register_edit2_callback(
        y_if_M_if,
        (const xmlChar *)"/if:interfaces",
        y_if_R_if,
        interfaces_edit);
    if (res != NO_ERR) {
        return res;
    }

    res = agt_cb_register_edit2_callback(
        y_if_M_if,
        (const xmlChar *)"/if:interfaces/if:interface",
        y_if_R_if,
        interface_edit);
    if (res != NO_ERR) {
        return res;
    }

    res = agt_cb_register_get_callback(
        y_if_M_if,
        (const xmlChar *)"/if:interfaces/if:interface/if:oper-status",
        y_if_R_if,
        oper_status_get);
    if (res != NO_ERR) {
        return res;
    }


#ifdef u_if_F_if_mib
    if (ncx_feature_enabled_str(
        y_if_M_if,
        y_if_R_if,
        (const xmlChar *)"if-mib")) {
        res = agt_cb_register_get_callback(
            y_if_M_if,
            (const xmlChar *)"/if:interfaces/if:interface/if:if-index",
            y_if_R_if,
            if_index_get);
        if (res != NO_ERR) {
            return res;
        }
    } /* u_if_F_if_mib */
#endif /* u_if_F_if_mib */

    res = agt_cb_register_get_callback(
        y_if_M_if,
        (const xmlChar *)"/if:interfaces/if:interface/if:phys-address",
        y_if_R_if,
        phys_address_get);
    if (res != NO_ERR) {
        return res;
    }

    res = agt_cb_register_get_callback(
        y_if_M_if,
        (const xmlChar *)"/if:interfaces/if:interface/if:speed",
        y_if_R_if,
        speed_get);
    if (res != NO_ERR) {
        return res;
    }

    res = u_ietf_interfaces_init(modname, revision);
    return res;

} /* y_ietf_interfaces_init */


/**
 * @brief Phase 2: Initialize the ietf-interfaces server instrumentation library.
 *
 * SIL init phase 2: non-config data structures.
 * Called after running config is loaded.
 *
 * @return return status. An error will cause the
 * server initialization to fail.
 */
status_t y_ietf_interfaces_init2 (void)
{
    status_t res = NO_ERR;

    if (LOGDEBUG) {
        log_debug("\nEnter y_ietf_interfaces_init2");
    }


    res = u_ietf_interfaces_init2();

    return res;

} /* y_ietf_interfaces_init2 */


/**
 * @brief Cleanup the ietf-interfaces server instrumentation library.
 *
 * Called by server when module is unloaded.
 *
 */
void y_ietf_interfaces_cleanup (void)
{

    if (LOGDEBUG) {
        log_debug("\nEnter y_ietf_interfaces_cleanup");
    }


    agt_cb_unregister_callbacks(
        y_if_M_if,
        (const xmlChar *)"/if:interfaces");

    agt_cb_unregister_callbacks(
        y_if_M_if,
        (const xmlChar *)"/if:interfaces/if:interface");

    u_ietf_interfaces_cleanup();

} /* y_ietf_interfaces_cleanup */


 /** @} */

/* END SIL-SA y_ietf-interfaces.c */