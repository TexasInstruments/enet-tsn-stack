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
 * @file        nconf_ucclient.h
 *
 * @brief       Netconf Uniconf Client Interface Header.
 */
#ifndef __NCONF_UCCLIENT_H__
#define __NCONF_UCCLIENT_H__

/*=============================================================================
 * Include Files
 *============================================================================*/

#include "nconf_types.h"
#include "nconf_fsutils.h"
#include "nconf_xmlutils.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*=============================================================================
 * Typedefs, Structures, and Enums
 *============================================================================*/

typedef enum {
    NCONF_UCCLIENT_CFG_TYPE_RW=(0x01U),
    NCONF_UCCLIENT_CFG_TYPE_R0=(0x10U),
    NCONF_UCCLIENT_CFG_TYPE_ALL=(NCONF_UCCLIENT_CFG_TYPE_RW|NCONF_UCCLIENT_CFG_TYPE_R0)
} nconf_ucclcfg_type_t;

typedef struct {
    char *dbname;
    char *schemadir;
    bool ucthread;
} nconf_ucclopt_t;

typedef struct {
    int (*txnode)(nconf_nodeptr_t nodeptr, void *uargs);
    int (*txhdr)(nconf_nodeptr_t nodeptr, void *uargs);
    int (*txftr)(char *nodename, void *uargs);
    char *hdrsent;
    size_t hdrsz;
    void *uargs;
} nconf_uccl_txfunc_t;

typedef struct _ucclient_cfginfo {
    char *path;
    char *pathnomod;
    char *name;
    int vtype;
    void *value;
    uint32_t vsize;
} nconf_cfginfo_t;

struct _ucclient_hdl;
typedef struct _ucclient_hdl * nconf_uccl_t;

/*=============================================================================
 * Uniconf DB Client APIs
 *============================================================================*/

int nconf_ucclient_init(nconf_ucclopt_t *ucclopt, nconf_uccl_t *pdbhdl);
int nconf_ucclient_dbtree_init(nconf_uccl_t dbhdl);
int nconf_ucclient_reg_txfunc(nconf_uccl_t dbhdl, nconf_uccl_txfunc_t *txfunc);
void nconf_ucclient_unreg_txfunc(nconf_uccl_t dbhdl);
nconf_fshdl_t nconf_ucclient_get_schema_hdl(nconf_uccl_t dbhdl, char *module,
                                            char *version, char *format);
nconf_nodeptr_t nconf_ucclient_search_dbtree(nconf_uccl_t dbhdl,
                                             nconf_filter_info_t *finfo,
                                             nconf_ucclcfg_type_t type);
nconf_nodeptr_t nconf_ucclient_generate_subtree(nconf_uccl_t dbhdl,
                                                nconf_treeinfo_t *subtree,
                                                nconf_ucclcfg_type_t type);
uint8_t nconf_ucclient_get_nodepath_vknum(nconf_uccl_t dbhdl, char *nodepath);
uint8_t nconf_ucclient_get_nodepath_vktype(nconf_uccl_t dbhdl, char *nodepath, uint8_t index);
char* nconf_ucclient_get_capabilities(nconf_uccl_t dbhdl, uint32_t *pvsize);
char* nconf_ucclient_get_nodepath_vkstr(nconf_uccl_t dbhdl, char *nodepath, uint8_t index);
int nconf_ucclient_exec_config_update(nconf_uccl_t dbhdl, nconf_treeinfo_t *cfginfo);
int nconf_ucclient_exec_config_read(nconf_uccl_t dbhdl, const char *cfgpath,
                                    void **value, uint32_t *vsize);
int nconf_ucclient_exec_config_save(nconf_uccl_t dbhdl);
int nconf_ucclient_exec_config_add(nconf_uccl_t dbhdl, void *addfunc);
int nconf_ucclient_get_config_from_db(nconf_uccl_t dbhdl, char *filter, struct ub_list *cfglist,
                                      nconf_ucclcfg_type_t type);
int nconf_ucclient_encode_config(nconf_uccl_t dbhdl, struct ub_list *cfglist);
int nconf_ucclient_send_single_cfg_node(nconf_uccl_t dbhdl, char *prstr, int vtype,
                                        void *value, uint32_t vsize);
int nconf_ucclient_get_yang_module_num(nconf_uccl_t dbhdl, uint8_t *modkey_start);
int nconf_ucclient_get_yang_module_name(nconf_uccl_t dbhdl, uint8_t key, char *buf, int size);
void nconf_ucclient_config_info_release(struct ub_list_node *node, void *uargs);
void nconf_ucclient_config_read_release(nconf_uccl_t dbhdl, void *value);
void nconf_ucclient_release_caps_resource(nconf_uccl_t dbhdl, void *value);
void nconf_ucclient_vkstr_release(char *vkstr);
void nconf_ucclient_dbtree_release(nconf_uccl_t dbhdl);
void nconf_ucclient_cleanup(nconf_uccl_t dbhdl);

#ifdef __cplusplus
}
#endif

#endif /* __NCONF_UCCLIENT_H__ */
