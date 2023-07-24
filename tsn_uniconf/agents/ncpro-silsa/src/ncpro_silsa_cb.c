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
/**
 * @file        ncpro_silsa_cb.c
 * @author      Allan Co<allanrodrigo.co@excelfore.com>
 * @copyright   Copyright (C) 2023 Excelfore Corporation
 *
 * @brief       Contains netconfd-pro hooks callbacks sample implementations.
 *              The callbacks constents are mostly copied from YumaWorks
 *              sample codes.
 */

/*=============================================================================
 * Include Files
 *============================================================================*/

#include "ncpro_silsa_cb.h"
#include "ucclient.h"
#include <yumapro/sil-sa/sil_sa.h>

/*=============================================================================
 * Macros and Constants
 *============================================================================*/

/* None */

/*=============================================================================
 * Typedefs, Structures, and Enums
 *============================================================================*/

/* None */

/*=============================================================================
 * Function Prototypes
 *============================================================================*/

/*=============================================================================
 * Global Variables
 *============================================================================*/

/*=============================================================================
 * Function Definitions
 *============================================================================*/

/********************************************************************
* FUNCTION hooks_validate_complete_cb
*
*  SIL-SA Validate Complete callback.
*
*
* RETURNS:
*   status
********************************************************************/
status_t hooks_validate_complete_cb (const xmlChar *transaction_id_val)
{
    if (!transaction_id_val) {
        log_error("\ntransaction_id value not set");
        return ERR_INTERNAL_VAL;
    }

    const xmlChar *user = sil_sa_get_username();
    const xmlChar *client_addr = sil_sa_get_client_addr();
    status_t res = NO_ERR;

    log_debug2("\n\n********************************************");
    log_debug2("\nEnter hooks_validate_complete_cb callback\n");
    log_debug2("\ntransaction_id -- %s\n", transaction_id_val);
    log_debug2("\nuser_id -- %s\n", user);
    log_debug2("\nclient_addr -- %s\n", client_addr);
    log_debug2("\n********************************************\n");
    return res;

}  /* hooks_validate_complete_cb */

/********************************************************************
* FUNCTION hooks_apply_complete_cb
*
*  SIL-SA Apply Complete callback.
*
*
* RETURNS:
*   status
********************************************************************/
status_t hooks_apply_complete_cb (const xmlChar *transaction_id_val)
{
    if (!transaction_id_val) {
        log_error("\ntransaction_id value not set");
        return ERR_INTERNAL_VAL;
    }

    const xmlChar *user = sil_sa_get_username();
    const xmlChar *client_addr = sil_sa_get_client_addr();
    status_t res = NO_ERR;

    log_debug2("\n********************************************");
    log_debug2("\nEnter hooks_apply_complete_cb callback\n");
    log_debug2("\ntransaction_id -- %s\n", transaction_id_val);
    log_debug2("\nuser_id -- %s\n", user);
    log_debug2("\nclient_addr -- %s\n", client_addr);
    log_debug2("\n********************************************\n");
    return res;

}  /* hooks_apply_complete_cb */

/********************************************************************
* FUNCTION hooks_commit_complete_cb
*
*  SIL-SA Commit Complete callback.
*
*
* RETURNS:
*   status
********************************************************************/
status_t hooks_commit_complete_cb (const xmlChar *transaction_id_val,
                                   agt_commit_type_t commit_type)
{
    if (!transaction_id_val) {
        log_error("\ntransaction_id value not set");
        return ERR_INTERNAL_VAL;
    }

    const xmlChar *user = sil_sa_get_username();
    const xmlChar *client_addr = sil_sa_get_client_addr();
    status_t res = NO_ERR;
    status_t res2 = NO_ERR;

    if (commit_type == AGT_COMMIT_TYPE_REPLAY) {
        return res;
    }

    log_debug2("\n\n********************************************");
    log_debug2("\nEnter hooks_commit_complete_cb callback");
    log_debug2("\ntransaction_id -- %s", transaction_id_val);
    log_debug2("\nuser_id -- %s\n", user);
    log_debug2("\nclient_addr -- %s", client_addr);
    log_debug2("\ncommit type -- %s (%u)",
             AGT_COMMIT_TYPE_NONE==commit_type ? "None":
             AGT_COMMIT_TYPE_NORMAL==commit_type ? "Normal" :
             AGT_COMMIT_TYPE_REPLAY==commit_type ? "Replay" :
             "Unknown", (uint32_t)commit_type);

    const xmlChar *defpath =
        (const xmlChar *)"/testsystem";

    val_value_t *testval =
        sil_sa_get_data(NCX_CFGID_CANDIDATE,
                        defpath,
                        &res2);
    if (testval) {
        log_debug2("\n+++ Got testval %s (%s)", VAL_NAME(testval), get_error_string(res));

        val_value_t *child = val_get_first_child(testval);
        if (child) {
            if (child->btyp && typ_is_simple(child->btyp)) {
                xmlChar *valstr = val_make_sprintf_string(child);
                if (valstr == NULL) {
                    return ERR_INTERNAL_MEM;
                }

                log_debug2("child:   %s='%s'\n", VAL_NAME(child), valstr);

                /* FORCE ERROR */
                if (!xml_strcmp(valstr,
                        (const xmlChar *)"commit-completeness-test1")) {

                    res = ERR_NCX_OPERATION_NOT_SUPPORTED;
                    log_debug2("\n------ REPORTING AN ERROR (%s)\n",
                        get_error_string(res));
                }

                m__free(valstr);
            }
        }
    }

    log_debug2("\n********************************************\n");

    /* if no error, send notification to uniconf to notify other applications */
    if (NO_ERR==res) {
        ucclient_db_notify();
    }
    return res;

}  /* hooks_commit_complete_cb */

/********************************************************************
* FUNCTION hooks_rollback_complete_cb
*
*  SIL-SA Rollback Complete callback.
*
*
* RETURNS:
*   status
********************************************************************/
status_t hooks_rollback_complete_cb (const xmlChar *transaction_id_val)
{
    if (!transaction_id_val) {
        log_error("\ntransaction_id value not set");
        return ERR_INTERNAL_VAL;
    }

    const xmlChar *user = sil_sa_get_username();
    const xmlChar *client_addr = sil_sa_get_client_addr();
    status_t res = NO_ERR;

    log_debug2("\n\n********************************************");
    log_debug2("\nEnter hooks_rollback_complete_cb callback");
    log_debug2("\ntransaction_id -- %s", transaction_id_val);
    log_debug2("\nuser_id -- %s", user);
    log_debug2("\nclient_addr -- %s", client_addr);
    log_debug2("\n********************************************\n");
    return res;

}  /* hooks_rollback_complete_cb */

