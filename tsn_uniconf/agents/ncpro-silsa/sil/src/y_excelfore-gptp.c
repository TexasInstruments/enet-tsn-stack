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
 * @file y_excelfore-gptp.c
 * @brief YumaPro SIL-SA module for module excelfore-gptp
 *

*** Generated by yangdump-sdk 21.10-12

<pre>

    YumaPro SIL-SA module
    bundle x4ucsilsa
    module excelfore-gptp
    revision 2022-11-25
    prefix xl4gptp
    namespace urn:ieee:vendor:802.1AS:yang:xl4gptp
    organization Excelfore Japan K.K.
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
    instance_domain_map = /ptp/instance-domain-map
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
 * @addtogroup silsa-x4ucsilsa-excelfore-gptp Module excelfore-gptp in Bundle x4ucsilsa
 * @ingroup silsa-x4ucsilsa
 *
 * @{
 *
 * YANG instrumentation library.
 * Generated by yangdump-sdk.
 */


/* module static variables */
static ncx_module_t *excelfore_gptp_mod;

/**
 * @brief Initialize module static variables
 *
 * Called by server when module is loaded.
 */
static void y_xl4gptp_init_static_vars (void)
{
    excelfore_gptp_mod = NULL;

} /* y_xl4gptp_init_static_vars */


/**
 * @brief Phase 1: Initialize the excelfore-gptp server instrumentation library.
 *
 * Called by server when module is loaded.
 *
 * @param modname requested module name to load
 * @param revision requested revision date of the module to load.
 * This may be NULL if the module has no revision statements.
 * @return return status. An error will cause the module load to fail.
 */
status_t y_excelfore_gptp_init (
    const xmlChar *modname,
    const xmlChar *revision)
{
    status_t res = NO_ERR;

    if (LOGDEBUG) {
        log_debug("\nEnter y_excelfore_gptp_init");
    }


    y_xl4gptp_init_static_vars();

    /* change if custom handling done */
    if (xml_strcmp(modname, y_xl4gptp_M_xl4gptp)) {
        return ERR_NCX_UNKNOWN_MODULE;
    }

    if (revision && xml_strcmp(revision, y_xl4gptp_R_xl4gptp)) {
        return ERR_NCX_WRONG_VERSION;
    }

    res = ncxmod_load_module(
        y_xl4gptp_M_xl4gptp,
        y_xl4gptp_R_xl4gptp,
        agt_get_savedevQ(),
        &excelfore_gptp_mod);
    if (res != NO_ERR) {
        return res;
    }

    res = u_excelfore_gptp_init(modname, revision);
    return res;

} /* y_excelfore_gptp_init */


/**
 * @brief Phase 2: Initialize the excelfore-gptp server instrumentation library.
 *
 * SIL init phase 2: non-config data structures.
 * Called after running config is loaded.
 *
 * @return return status. An error will cause the
 * server initialization to fail.
 */
status_t y_excelfore_gptp_init2 (void)
{
    status_t res = NO_ERR;

    if (LOGDEBUG) {
        log_debug("\nEnter y_excelfore_gptp_init2");
    }


    res = u_excelfore_gptp_init2();

    return res;

} /* y_excelfore_gptp_init2 */


/**
 * @brief Cleanup the excelfore-gptp server instrumentation library.
 *
 * Called by server when module is unloaded.
 *
 */
void y_excelfore_gptp_cleanup (void)
{

    if (LOGDEBUG) {
        log_debug("\nEnter y_excelfore_gptp_cleanup");
    }


    u_excelfore_gptp_cleanup();

} /* y_excelfore_gptp_cleanup */


 /** @} */

/* END SIL-SA y_excelfore-gptp.c */