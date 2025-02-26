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
 * @file        nconf_ucclient.c
 *
 * @brief       Netconf Uniconf Client Implementation.
 */

/*=============================================================================
 * Include Files
 *============================================================================*/

#include <stdint.h>
#include <string.h>
#include <yangs/yang_node.h>
#include <yangs/yang_modules.h>
#include <yangs/yang_db_runtime.h>
#include <uc_dbal.h>
#include <uc_notice.h>
#include "nconf_ucclient.h"
#include "nconf_msgdef.h"
#include "nconf_memutils.h"
#include "nconf_types.h"
#include "nconf_xmlutils.h"
#include "ub_getmem.h"
#include "unibase_macros.h"

/*=============================================================================
 * Macros and Constants
 *============================================================================*/

#define NCONF_MAX_UCCFG_VSIZE_LIMIT (256U)
#define NCONF_MAX_UCCFG_LINE_LEN    (256U)

#define NCONF_MODULE_SCHEMA_SEARCH_FMT \
    "/ietf-netconf-monitoring/netconf-state/schemas/schema" \
    "|identifier:%s|/namespace/"

#define NCONF_MODULE_SCHME_GET_FMT \
    "/ietf-netconf-monitoring/netconf-state/schemas/schema" \
    "|identifier:%s|version:%s|format:%s|/namespace"

/*=============================================================================
 * Typedefs, Structures, and Enums
 *============================================================================*/

typedef struct _ucclient_db_access {
    uint8_t kss[UC_MAX_KV_DEPTH];
    void *kvs[UC_MAX_KV_DEPTH+1];
    uc_range *range;
} nconf_db_access_t;

struct _ucclient_hdl {
    uc_dbald *dbald;
    yang_db_runtime_dataq_t *ydrd;
    uc_notice_data_t *ucntd;
    nconf_docptr_t dbtree;
    nconf_db_access_t dbacc;
    nconf_uccl_txfunc_t txfunc;
    uint8_t callmode;
    uint8_t dbhdlref;
    char *schemadir;
};

typedef struct _instkeylist {
    nconf_kval_t vk[UC_MAX_KV_DEPTH];
    uint8_t vknum;
} nconf_instkeylist_t;

typedef struct _instinfo {
    nconf_instkeylist_t instkey;
    char *name;
    char *value;
} nconf_instinfo_t;

typedef struct _ucclcfg_args {
    nconf_instinfo_t instNode;
    nconf_cfginfo_t *cfg;
} nconf_ucclcfg_args_t;

typedef int (*nconf_ucclient_addfunc_t)(uc_dbald *, uc_hwald *);

/*=============================================================================
 * Function Prototypes
 *============================================================================*/

static int nconf_ucclient_hdl_init(struct _ucclient_hdl **hdl, bool ucthread);
static int nconf_ucclient_dbal_open(struct _ucclient_hdl *hdl, char *dbname);
static int nconf_ucclient_runtime_init(struct _ucclient_hdl *hdl);
static int nconf_ucclient_notice_init(struct _ucclient_hdl *hdl, char *dbname);
static int nconf_ucclient_not_ignore_leaf(char *nodename);
static int nconf_ucclient_cmp_instance_keys(nconf_instkeylist_t *instKeys,
                                            nconf_nodeptr_t container);
static nconf_nodeptr_t nconf_ucclient_get_existing_node(nconf_nodeptr_t parent,
                                                        nconf_instkeylist_t *instKeys,
                                                        char *xpathSegment);
static nconf_nodeptr_t nconf_ucclient_get_new_node(struct _ucclient_hdl *hdl,
                                                   nconf_ucclcfg_args_t *prm,
                                                   nconf_nodeptr_t parent,
                                                   char *xpathSegment);
static int nconf_ucclient_set_container_key_nodes(struct _ucclient_hdl *hdl,
                                                  nconf_ucclcfg_args_t *prm,
                                                  nconf_nodeptr_t container);
static char* nconf_ucclient_tokenize_xpath(char *xpath, char delim, char **saveptr);
static nconf_nodeptr_t nconf_ucclient_get_module_node(struct _ucclient_hdl *hdl,
                                                      nconf_ucclcfg_args_t *prm);
static int nconf_ucclient_update_nsparams(struct _ucclient_hdl *hdl,
                                          nconf_ucclcfg_args_t *prm,
                                          nconf_nodeptr_t dstnode);
static int nconf_ucclient_set_node_contents(struct _ucclient_hdl *hdl,
                                            nconf_ucclcfg_args_t *prm,
                                            nconf_nodeptr_t node);
static int nconf_ucclient_set_strtype_node_contents(nconf_ucclcfg_args_t *prm,
                                                    nconf_nodeptr_t node);
static int nconf_ucclient_set_generic_node_contents(struct _ucclient_hdl *hdl,
                                                    nconf_ucclcfg_args_t *prm,
                                                    nconf_nodeptr_t node);
static char *nconf_ucclient_get_vstr(struct _ucclient_hdl *hdl,
                                     nconf_ucclcfg_args_t *prm,
                                     uint32_t idx, nconf_nodeptr_t node);
static char *nconf_ucclient_get_nsprefix(struct _ucclient_hdl *hdl,
                                         nconf_ucclcfg_args_t *prm,
                                         uint32_t idx, nconf_nodeptr_t node);
static int nconf_ucclient_insert_new_cfg_nodes(struct _ucclient_hdl *hdl,
                                               nconf_ucclcfg_args_t *prm);
static int nconf_ucclient_get_instance_keys(nconf_ucclcfg_args_t *prm,
                                            char *xpathSegment);
static int nconf_ucclient_update_config_cache(struct ub_list *cfglist,
                                              char *cfgpath, int vtype,
                                              void *value, uint32_t vsize);
static void nconf_ucclient_release_config_cache(nconf_cfginfo_t *cfg);
static int nconf_ucclient_cmp_dbpath(char *filter, char *dbpath);
static int nconf_ucclient_cmp_kvs(uint8_t *vp, void **kvs, uint8_t *kss);
static bool nconf_ucclient_is_yang_config(uc_dbald *dbald, char *cfgstr);
static int nconf_ucclient_getvknum(uc_dbald *dbald, uint8_t *aps, bool *pisdiff);
static int nconf_ucclient_getvktype(uc_dbald *dbald, uint8_t *aps, uint8_t vkindex,
                                    uint8_t *vkey, uint8_t *vtype);
static int nconf_ucclient_getvkstr(uc_dbald *dbald, uint8_t *aps, uint8_t vkindex,
                                   char **rstr);
static int nconf_ucclient_process_dbkeys(nconf_uccl_t dbhdl, void *key, uint32_t ksize,
                                         void *value, uint32_t vsize, char *filter,
                                         struct ub_list *cfglist);
static int nconf_ucclient_get_schema_namespace(nconf_uccl_t dbhdl, char *module,
                                               char *version, char *format);
static int nconf_ucclient_schema_search(nconf_uccl_t dbhdl, char *module,
                                        char **pversion, char **pformat);
static int nconf_ucclient_validate_schema_params(char *dbpath, char **pversion,
                                                 char **pformat);
static int nconf_ucclient_get_dbpath_schema_keys(char *dbpath, char **pversion,
                                                 char **pformat);
static bool nconf_ucclient_is_nodehdr_sent(nconf_uccl_t hdl, char *nodename);
static void nconf_ucclient_add_to_hdrsent(nconf_uccl_txfunc_t *txfunc, char *nodename);
static char* nconf_ucclient_get_path_instance(char *nodepath);
static void nconf_ucclient_delete_child_node(nconf_uccl_t hdl, char *nodename);
static void nconf_ucclient_send_child_nodes(nconf_uccl_t hdl, nconf_nodeptr_t parent,
                                            char *lastHdrSent, char *nextSegment);
static bool nconf_ucclient_is_nodekey_sent(nconf_uccl_t hdl, nconf_nodeptr_t child);
static void nconf_ucclient_send_node_instance(nconf_uccl_t hdl, char *instpath);
static void nconf_ucclient_send_end_tags(nconf_uccl_t hdl, char *next_nodepath);
static void nconf_ucclient_send_completed_nodes(nconf_uccl_t dbhdl, char *nodepath,
                                                char **retinst);
static void nconf_ucclient_clear_traversed_list(struct ub_list *cfglist,
                                                struct ub_list_node *node);
static void nconf_ucclient_acquire_dbal(nconf_uccl_t dbhdl);
static void nconf_ucclient_release_dbal(nconf_uccl_t dbhdl);

/*=============================================================================
 * Global Variables
 *============================================================================*/

UB_SD_GETMEM_DEF(NCONF_UCCLIENT_HANDLE, sizeof(struct _ucclient_hdl), 1);
UB_SD_GETMEM_DEF(NCONF_CFG_CACHE_DATAINST, sizeof(nconf_cfginfo_t),
                 NCONF_MAX_UCCFG_INFO_NUM);
UB_SD_GETMEM_DEF(NCONF_CFG_CACHE_LISTNODE, sizeof(struct ub_list_node),
                 NCONF_MAX_UCCFG_INFO_NUM);
UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

/*=============================================================================
 * Function Definitions
 *============================================================================*/

int nconf_ucclient_init(nconf_ucclopt_t *ucclopt, nconf_uccl_t *dbhdl)
{
    struct _ucclient_hdl *hdl=NULL;
    int ret=nconf_ucclient_hdl_init(&hdl, ucclopt->ucthread);
    if (0==ret) {
        ret=nconf_ucclient_dbal_open(hdl, ucclopt->dbname);
    }
    if (0==ret) {
        ret=nconf_ucclient_runtime_init(hdl);
    }
    if (0==ret) {
        ret=nconf_ucclient_notice_init(hdl, ucclopt->dbname);
    }
    if (0==ret) {
        ydbi_access_init(hdl->dbald, hdl->ucntd);
        hdl->schemadir=ucclopt->schemadir;
        *dbhdl=hdl;
    } else {
        nconf_ucclient_cleanup(hdl);
    }
    return ret;
}

int nconf_ucclient_reg_txfunc(nconf_uccl_t dbhdl, nconf_uccl_txfunc_t *txfunc)
{
    int ret=NCONF_RPC_ERROR_TAG_OPERATION_FAILED;
    if (NULL != dbhdl) {
        memset(&dbhdl->txfunc, 0, sizeof(dbhdl->txfunc));
        dbhdl->txfunc.txnode=txfunc->txnode;
        dbhdl->txfunc.txhdr=txfunc->txhdr;
        dbhdl->txfunc.txftr=txfunc->txftr;
        dbhdl->txfunc.uargs=txfunc->uargs;
        ret=NCONF_RPC_ERROR_TAG_NO_ERROR;
    } else {
        UB_LOG(UBL_ERROR, "%s:invalid dbhdl\n", __func__);
    }
    return ret;
}

void nconf_ucclient_unreg_txfunc(nconf_uccl_t dbhdl)
{
    if (NULL != dbhdl) {
        dbhdl->txfunc.txnode=NULL;
        dbhdl->txfunc.txhdr=NULL;
        dbhdl->txfunc.txftr=NULL;
        dbhdl->txfunc.uargs=NULL;
    }
}

nconf_fshdl_t nconf_ucclient_get_schema_hdl(nconf_uccl_t dbhdl, char *module,
                                            char *version, char *format)
{
    nconf_fshdl_t schemahdl=NULL;
    char *ver=version;
    char *fmt=format;
    int res=-1;
    if (NULL != version && NULL != format) {
        res=nconf_ucclient_get_schema_namespace(dbhdl, module, ver, fmt);
    } else {
        res=nconf_ucclient_schema_search(dbhdl, module, &ver, &fmt);
    }
    if (res >= 0) {
        char *tmpbuf=(char*)nconf_memalloc(NCONF_MAX_UCCFG_LINE_LEN);
        if(ub_assert_fatal(NULL!=tmpbuf, __func__, "alloc error")){return NULL;}
        memset(tmpbuf, 0, NCONF_MAX_UCCFG_LINE_LEN);
        snprintf(tmpbuf, NCONF_MAX_UCCFG_LINE_LEN-1, "%s/%s.%s",
                 dbhdl->schemadir, module, fmt);
        schemahdl=nconf_fopen((const char*)tmpbuf, "r");
        if (NULL==schemahdl) {
            UB_LOG(UBL_ERROR, "%s:failed to open schema: [%s]\n", __func__, tmpbuf);
        }
        nconf_memfree(tmpbuf);
        if (ver != version) { nconf_memfree(ver); }
        if (fmt != format) { nconf_memfree(fmt); }
    } else {
        UB_LOG(UBL_ERROR, "%s:schema not in db [%s]\n", __func__, module);
    }
    return schemahdl;
}

int nconf_ucclient_dbtree_init(nconf_uccl_t dbhdl)
{
    int ret=NCONF_RPC_ERROR_TAG_OPERATION_FAILED;
    dbhdl->dbtree=nconf_xmlutils_new_doc_w_root("1.0", "data");
    if (NULL!=dbhdl->dbtree) { ret=NCONF_RPC_ERROR_TAG_NO_ERROR; }
    return ret;
}

uint8_t nconf_ucclient_get_nodepath_vknum(nconf_uccl_t dbhdl, char *nodepath)
{
    uint8_t aps[UC_MAX_AP_DEPTH];
    uint8_t vknum=0, apscnt=0;
    char *rstr=nconf_strdup(nodepath);
    if (NULL!=rstr) {
        nconf_ucclient_acquire_dbal(dbhdl);
        apscnt=yang_node_get_node_enums(dbhdl->dbald, rstr, aps, sizeof(aps));
        if (apscnt>0) {
            vknum=nconf_ucclient_getvknum(dbhdl->dbald, aps, NULL);
        } else {
            UB_LOG(UBL_ERROR, "%s:invalid nodepath [%s]\n", __func__, nodepath);
        }
        nconf_ucclient_release_dbal(dbhdl);
        nconf_memfree(rstr);
    }
    return vknum;
}

char* nconf_ucclient_get_capabilities(nconf_uccl_t dbhdl, uint32_t *pvsize)
{
    const char line[]="/ietf-netconf-monitoring/netconf-state/capabilities/capability";
    void *value=NULL;
    uint32_t vsize=0;
    nconf_ucclient_acquire_dbal(dbhdl);
    int res=yang_db_runtime_get_oneline(dbhdl->ydrd, line, &value, &vsize);
    if (res >= 0) {
        *pvsize=vsize;
    } else {
        UB_LOG(UBL_ERROR, "%s:failed to get capabilities [%d]\n", __func__, res);
    }
    nconf_ucclient_release_dbal(dbhdl);
    return (char*)value;
}

char* nconf_ucclient_get_nodepath_vkstr(nconf_uccl_t dbhdl, char *nodepath, uint8_t index)
{
    uint8_t aps[UC_MAX_AP_DEPTH];
    char *rstr=NULL, *vkstr=NULL;
    uint8_t apscnt=0;
    int ret=0;
    rstr=nconf_strdup(nodepath);
    if (NULL!=rstr) {
        nconf_ucclient_acquire_dbal(dbhdl);
        apscnt=yang_node_get_node_enums(dbhdl->dbald, rstr, aps, sizeof(aps));
        if (apscnt>0) {
            ret=nconf_ucclient_getvkstr(dbhdl->dbald, aps, index, &vkstr);
            if(0!=ret) {
                UB_LOG(UBL_ERROR, "%s:nconf_ucclient_getvkstr() failed\n", __func__);
            }
        }
        nconf_ucclient_release_dbal(dbhdl);
        nconf_memfree(rstr);
    }
    return vkstr;
}

uint8_t nconf_ucclient_get_nodepath_vktype(nconf_uccl_t dbhdl, char *nodepath, uint8_t index)
{
    uint8_t aps[UC_MAX_AP_DEPTH];
    uint8_t vkey, vtype=YANG_VTYPE_ENUM_END;
    char *rstr=NULL;
    uint8_t apscnt=0;
    int ret=0;
    rstr=nconf_strdup(nodepath);
    if (NULL!=rstr) {
        nconf_ucclient_acquire_dbal(dbhdl);
        apscnt=yang_node_get_node_enums(dbhdl->dbald, rstr, aps, sizeof(aps));
        if (apscnt>0) {
            ret=nconf_ucclient_getvktype(dbhdl->dbald, aps, index, &vkey, &vtype);
            if(0!=ret) {
                UB_LOG(UBL_ERROR, "%s:nconf_ucclient_getvktype() failed\n", __func__);
            }
        }
        nconf_ucclient_release_dbal(dbhdl);
        nconf_memfree(rstr);
    }
    return vtype;
}

uint8_t nconf_ucclient_get_config_vtype(nconf_uccl_t dbhdl, char *cfgstr)
{
    uint8_t apsd[UC_MAX_AP_DEPTH+2]={XL4_DATA_RW, YANG_VALUE_TYPES, 255,};
    uint8_t vtype=YANG_VTYPE_ENUM_END;
    uint32_t vsize=0U;
    uint8_t apsdcnt=0U;
    char *line=NULL;
    void *value=NULL;
    line=nconf_strdup(cfgstr);
    if (NULL != line) {
        nconf_ucclient_acquire_dbal(dbhdl);
        apsdcnt=yang_node_get_node_enums(dbhdl->dbald, line, &apsd[2], sizeof(apsd)-2);
        if (apsdcnt > 0) {
            int res=uc_dbal_get(dbhdl->dbald, apsd, apsdcnt+2, &value, &vsize);
            if (0==res) {
                vtype=*((uint8_t*)value);
            } else {
                UB_LOG(UBL_ERROR, "%s:failed to get config vtype\n", __func__);
            }
        }
        nconf_ucclient_release_dbal(dbhdl);
        nconf_memfree(line);
    }
    return vtype;
}

int nconf_ucclient_exec_config_read(nconf_uccl_t dbhdl, const char *cfgpath,
                                    void **value, uint32_t *vsize)
{
    nconf_ucclient_acquire_dbal(dbhdl);
    int res=yang_db_runtime_get_oneline(dbhdl->ydrd, cfgpath, value, vsize);
    if (res < 0) {
        UB_LOG(UBL_WARN, "%s:failed to read data in cfgpath=[%s]\n",
               __func__, cfgpath);
    }
    nconf_ucclient_release_dbal(dbhdl);
    return res;
}

int nconf_ucclient_exec_config_add(nconf_uccl_t dbhdl, void *addfunc)
{
    nconf_ucclient_addfunc_t ucaddfunc=(nconf_ucclient_addfunc_t)addfunc;
    int ret=-1;
    if (NULL != dbhdl && NULL != addfunc) {
        nconf_ucclient_acquire_dbal(dbhdl);
        ret=ucaddfunc(dbhdl->dbald, NULL);
        if (0!=ret) {
            UB_LOG(UBL_ERROR, "%s:addfunc failed [ret=%d]\n", __func__,  ret);
        }
        nconf_ucclient_release_dbal(dbhdl);
    } else {
        UB_LOG(UBL_ERROR, "%s:invalid parameter [h=%p, f=%p]\n",
               __func__, dbhdl, addfunc);
    }
    return ret;
}

int nconf_ucclient_get_config_from_db(nconf_uccl_t dbhdl, char *filter, struct ub_list *cfglist,
                                      nconf_ucclcfg_type_t type)
{
    void *key=NULL, *value=NULL;
    uint32_t ksize, vsize, j;
    int ret=NCONF_RPC_ERROR_TAG_NO_ERROR;
    for(j=0u;j<2u;j++){
        UB_LOG(UBL_DEBUGV, "%s:[filter=%s]\n", __func__, filter);
        nconf_ucclient_acquire_dbal(dbhdl);
        dbhdl->dbacc.range=yang_db_runtime_range_fromline(dbhdl->ydrd, filter,
                                    dbhdl->dbacc.kvs, dbhdl->dbacc.kss, (j==1));
        while(NULL != dbhdl->dbacc.range){
            if(uc_get_keyvalue_in_range(dbhdl->dbald, dbhdl->dbacc.range, &key,
                                        &ksize,&value,&vsize,UC_DBAL_FORWARD)){
                break;
            }
            ret=nconf_ucclient_process_dbkeys(dbhdl, key, ksize, value, vsize,
                                              filter, cfglist);
            if (NCONF_RPC_ERROR_TAG_NO_ERROR != ret) {
                UB_LOG(UBL_ERROR, "%s:db key processing failed\n", __func__);
                break;
            }
        }
        if (NULL != dbhdl->dbacc.range) {
            uc_get_range_release(dbhdl->dbald, dbhdl->dbacc.range);
            dbhdl->dbacc.range=NULL;
        }
        nconf_ucclient_release_dbal(dbhdl);

        /* skip ro items if not requested */
        if ((type & NCONF_UCCLIENT_CFG_TYPE_R0)==0) { break; }
    }
    return ret;
}

int nconf_ucclient_encode_config(nconf_uccl_t dbhdl, struct ub_list *cfglist)
{
    int ret=-1;
    char *last_inst=NULL;
    struct ub_list_node *node=ub_list_head(cfglist);
    while(node!=NULL){
        nconf_ucclcfg_args_t prm;
        memset(&prm, 0, sizeof(prm));
        prm.cfg=(nconf_cfginfo_t*)node->data;
        UB_LOG(UBL_DEBUGV, "%s:cfg=[%s]\n", __func__, prm.cfg->path);
        nconf_ucclient_send_completed_nodes(dbhdl, prm.cfg->pathnomod, &last_inst);
        ret=nconf_ucclient_insert_new_cfg_nodes(dbhdl, &prm);
        if (0 != ret) {
            UB_LOG(UBL_ERROR, "%s:Failed to insert [cfg=%s]\n",
                   __func__, prm.cfg->path);
            break;
        }
		struct ub_list_node *next=node->next;
		nconf_ucclient_config_info_release(node, NULL);
		node = next;
    }
    nconf_ucclient_clear_traversed_list(cfglist, node);
    if (NULL != last_inst) {
        nconf_ucclient_send_node_instance(dbhdl, last_inst);
        nconf_memfree(last_inst);
    }
    nconf_ucclient_send_end_tags(dbhdl, NULL);
    nconf_memfree(dbhdl->txfunc.hdrsent);
    dbhdl->txfunc.hdrsent=NULL;
    dbhdl->txfunc.hdrsz=0;
    return ret;
}

int nconf_ucclient_send_single_cfg_node(nconf_uccl_t dbhdl, char *prstr, int vtype,
                                        void *value, uint32_t vsize)
{
    int ret=-1;
    char *instpath=NULL, *topnodepth=NULL, *tmp=NULL;
    nconf_ucclcfg_args_t prm;
    nconf_cfginfo_t *cfginfo=(nconf_cfginfo_t*)
        UB_SD_GETMEM(NCONF_CFG_CACHE_DATAINST, sizeof(nconf_cfginfo_t));
    if(ub_assert_fatal((NULL!=cfginfo), __func__, "cfg info alloc error")){
        ret=NCONF_RPC_ERROR_TAG_TOO_BIG;
        goto end_oper;
    }

    cfginfo->path=nconf_strdup(prstr);
    if (NULL==cfginfo->path) { ret=NCONF_RPC_ERROR_TAG_TOO_BIG; goto end_oper; }

    cfginfo->pathnomod=strchr(&(cfginfo->path[1]), '/');
    if (NULL == cfginfo->pathnomod) { goto end_oper; }
    cfginfo->name=strrchr(cfginfo->path, '/');
    if (NULL != cfginfo->name) { cfginfo->name=cfginfo->name+1; }
    cfginfo->vtype=vtype;
    cfginfo->value=value;
    cfginfo->vsize=vsize;
    tmp=strchr(&cfginfo->pathnomod[1], '/');
    if (tmp != NULL) {
        tmp[0]='\0';
        topnodepth=nconf_strdup(cfginfo->pathnomod);
        tmp[0]='/';
    }

    memset(&prm, 0, sizeof(prm));
    prm.cfg=cfginfo;
    UB_LOG(UBL_DEBUGV, "%s:cfg=[%s]\n", __func__, prm.cfg->path);
    instpath=nconf_ucclient_get_path_instance(prm.cfg->pathnomod);
    if (NULL==instpath) {
        ret=NCONF_RPC_ERROR_TAG_TOO_BIG;
        goto end_oper;
    }

    ret=nconf_ucclient_insert_new_cfg_nodes(dbhdl, &prm);
    if (0==ret) {
        nconf_ucclient_send_node_instance(dbhdl, instpath);
        nconf_ucclient_send_end_tags(dbhdl, topnodepth);
    } else {
        UB_LOG(UBL_ERROR, "%s:Failed to insert [cfg=%s]\n", __func__, prm.cfg->path);
    }

end_oper:
    if (NULL != instpath) { nconf_memfree(instpath); }
    if (NULL != topnodepth) { nconf_memfree(topnodepth); }
    if (NULL != cfginfo){
        if (NULL != cfginfo->path){ nconf_memfree(cfginfo->path); }
        UB_SD_RELMEM(NCONF_CFG_CACHE_DATAINST, cfginfo);
    }
    return ret;
}

int nconf_ucclient_get_yang_module_num(nconf_uccl_t dbhdl, uint8_t *modkey_start)
{
    *modkey_start=XL4_EXTMOD_RW+1;
    return yang_node_mod_numofids(dbhdl->dbald);
}

int nconf_ucclient_get_yang_module_name(nconf_uccl_t dbhdl, uint8_t key, char *buf, int size)
{
    return yang_node_mod_get_string(dbhdl->dbald, key, buf, size);
}

void nconf_ucclient_config_read_release(nconf_uccl_t dbhdl, void *value)
{
    if (NULL != value) { UB_SD_RELMEM(YANGINIT_GEN_SMEM, value); }
    NCONF_UNUSED(dbhdl);
}

void nconf_ucclient_config_info_release(struct ub_list_node *node, void *uargs)
{
    if (NULL!=node) {
        nconf_ucclient_release_config_cache((nconf_cfginfo_t*)node->data);
        UB_SD_RELMEM(NCONF_CFG_CACHE_LISTNODE, node);
        NCONF_UNUSED(uargs);
    }
}

int nconf_ucclient_exec_config_update(nconf_uccl_t dbhdl, nconf_treeinfo_t *cfginfo)
{
    int res=-1;
    nconf_ucclient_acquire_dbal(dbhdl);
    for (uint8_t i=0; i<cfginfo->count; i++) {
        UB_LOG(UBL_INFO, "%s:npath  : [%s]\n", __func__, cfginfo->nodepaths[i]);
        if(cfginfo->nodepaths[i][strlen(cfginfo->nodepaths[i])-1]=='-'){
            res=yang_db_runtime_put_oneline(dbhdl->ydrd, cfginfo->nodepaths[i],
                                            NULL, YANG_DB_ONHW_NOACTION);
        } else {
            UB_LOG(UBL_INFO, "%s:nvalue : [%s]\n", __func__, (NULL!=cfginfo->nodevalues[i]) ?
                   cfginfo->nodevalues[i] : "<NULL>");
            res=yang_db_runtime_put_oneline(dbhdl->ydrd, cfginfo->nodepaths[i],
                                            cfginfo->nodevalues[i],
                                            YANG_DB_ONHW_NOACTION);
        }
        if (0==res) {
            res=yang_db_runtime_askaction(dbhdl->ydrd, dbhdl->ucntd);
            if (0!=res) {
                UB_LOG(UBL_WARN, "%s:db ask action failed [res=%d, i=%u]\n",
                       __func__, res, i);
            }
        } else {
            UB_LOG(UBL_ERROR, "%s:db put failed [res=%d, i=%u]\n",
                   __func__, res, i);
        }
    }
    nconf_ucclient_release_dbal(dbhdl);
    return res;
}

int nconf_ucclient_exec_config_save(nconf_uccl_t dbhdl)
{
    int ret=NCONF_RPC_ERROR_TAG_NO_ERROR;
    int res=ydbi_request_savedb(ydbi_access_handle());
    if(0!=res) {
        UB_LOG(UBL_ERROR, "%s:ydbi_request_savedb failed [res=%d]\n",
               __func__,  res);
        ret=NCONF_RPC_ERROR_TAG_OPERATION_FAILED;
    }
    NCONF_UNUSED(dbhdl);
    return ret;
}

void nconf_ucclient_release_caps_resource(nconf_uccl_t dbhdl, void *value)
{
    return nconf_ucclient_config_read_release(dbhdl, value);
}

void nconf_ucclient_vkstr_release(char *vkstr)
{
    UB_SD_RELMEM(YANGINIT_GEN_SMEM, vkstr);
}

void nconf_ucclient_dbtree_release(nconf_uccl_t dbhdl)
{
    if (NULL != dbhdl->dbtree) {
        nconf_xmlutils_free_doc(dbhdl->dbtree);
        dbhdl->dbtree=NULL;
    }
}

void nconf_ucclient_cleanup(nconf_uccl_t dbhdl)
{
    if (NULL != dbhdl) {
        ydbi_access_close();
        if(dbhdl->ucntd){ uc_notice_close(dbhdl->ucntd, dbhdl->callmode); }
        if(dbhdl->ydrd){ yang_db_runtime_close(dbhdl->ydrd); }
        if(dbhdl->dbald){ uc_dbal_close(dbhdl->dbald, dbhdl->callmode); }
        if(dbhdl->dbtree){ nconf_xmlutils_free_doc(dbhdl->dbtree); }
        UB_SD_RELMEM(NCONF_UCCLIENT_HANDLE, dbhdl);
    }
}

static int nconf_ucclient_hdl_init(struct _ucclient_hdl **hdl, bool ucthread)
{
    *hdl=(struct _ucclient_hdl*)
         UB_SD_GETMEM(NCONF_UCCLIENT_HANDLE, sizeof(struct _ucclient_hdl));
    if(ub_assert_fatal((NULL!=*hdl), __func__, "alloc error")){return -1;}
    memset(*hdl, 0, sizeof(struct _ucclient_hdl));
    if (ucthread) { (*hdl)->callmode=UC_CALLMODE_THREAD; }
    return 0;
}

static int nconf_ucclient_dbal_open(struct _ucclient_hdl *hdl, char *dbname)
{
    int ret=0;
    hdl->dbald=uc_dbal_open(dbname, "w", hdl->callmode);
    if(NULL==hdl->dbald) {
        UB_LOG(UBL_ERROR, "%s:uc_dbal_open() failed\n", __func__);
        ret=-1;
    }
    return ret;
}

static int nconf_ucclient_runtime_init(struct _ucclient_hdl *hdl)
{
    int ret=0;
    hdl->ydrd=yang_db_runtime_init(hdl->dbald, NULL);
    if(NULL==hdl->ydrd){
        UB_LOG(UBL_ERROR, "%s:yang_db_runtime_init() failed\n", __func__);
        ret=-1;
    }
    return ret;
}

static int nconf_ucclient_notice_init(struct _ucclient_hdl *hdl, char *dbname)
{
    int ret=0;
    hdl->ucntd=uc_notice_init(hdl->callmode, dbname);
    if(NULL==hdl->ucntd) {
        UB_LOG(UBL_ERROR, "%s:uc_notice_init() failed\n", __func__);
        ret=-1;
    }
    return ret;
}

static int nconf_ucclient_not_ignore_leaf(char *nodename)
{
    char *ignore_leaf_keyword[] = {
        "dummy",
        "valuekey",
        "nsasc"
    };
    uint32_t knum=sizeof(ignore_leaf_keyword)/sizeof(char*), i;
    int ret=1;
    for(i=0; i<knum; i++) {
        if(strcmp(ignore_leaf_keyword[i], nodename)==0) {
            ret=0;
            break;
        }
    }
    return ret;
}

static int nconf_ucclient_cmp_instance_keys(nconf_instkeylist_t *instKeys,
                                            nconf_nodeptr_t container)
{
    int ret=-1, found=0, i;
    for (i=0; i<instKeys->vknum; i++) {
        char *kncontent=instKeys->vk[i].val;
        nconf_nodeptr_t child=XMLUTILS_NODE_GET_CHILD(container);
        while (child != NULL) {
            char *chnode_name=XMLUTILS_NODE_GET_NAME(child);
            if (strcmp(chnode_name, instKeys->vk[i].key)==0) {
                char *chcontent=nconf_xmlutils_get_node_content(child);
                if (NULL != chcontent && strcmp(chcontent, kncontent)==0) {
                    nconf_xmlutils_free((uint8_t*)chcontent);
                    nconf_xmlutils_release_node(child);
                    found++;
                    break;
                }
                nconf_xmlutils_free((uint8_t*)chcontent);
            }
            nconf_nodeptr_t sibling=XMLUTILS_NODE_GET_SIBLING(child);
            nconf_xmlutils_release_node(child);
            child=sibling;
        }
    }
    if (instKeys->vknum==found) { ret=0; }
    return ret;
}

static nconf_nodeptr_t nconf_ucclient_get_existing_node(nconf_nodeptr_t parent,
                                                        nconf_instkeylist_t *instKeys,
                                                        char *xpathSegment)
{
    nconf_nodeptr_t ret_node=NULL;
    nconf_nodeptr_t child=XMLUTILS_NODE_GET_CHILD(parent);
    while (child != NULL) {
        char *child_name=XMLUTILS_NODE_GET_NAME(child);
        if (strcmp(child_name, xpathSegment) == 0) {
            if (!nconf_xmlutils_node_has_children(child)) {
                nconf_xmlutils_delete_child_node(parent, child);
            } else if (NULL != instKeys && instKeys->vknum > 0) {
                int ret=nconf_ucclient_cmp_instance_keys(instKeys, child);
                if (0==ret) { ret_node=child; break; }
            } else {
                ret_node=child;
                break;
            }
        }
        nconf_nodeptr_t sibling=XMLUTILS_NODE_GET_SIBLING(child);
        nconf_xmlutils_release_node(child);
        child=sibling;
    }
    return ret_node;
}

static nconf_nodeptr_t nconf_ucclient_get_new_node(struct _ucclient_hdl *hdl,
                                                   nconf_ucclcfg_args_t *prm,
                                                   nconf_nodeptr_t parent,
                                                   char *xpathSegment)
{
    nconf_nodeptr_t ret_node=NULL;
    int ret=0;

    ret_node=nconf_xmlutils_new_node(parent, xpathSegment, NULL);
    if(ret_node==NULL) {
        UB_LOG(UBL_ERROR, "%s:creating new node=[%s] failed\n",
               __func__, xpathSegment);
        goto erexit;
    }

    if (nconf_ucclient_update_nsparams(hdl, prm, ret_node) != 0) {
        UB_LOG(UBL_ERROR, "%s:failed to update ns params\n", __func__);
        nconf_xmlutils_free_node(ret_node);
        ret_node=NULL;
        goto erexit;
    }

    if (prm->instNode.instkey.vknum > 0) {
        ret=nconf_ucclient_set_container_key_nodes(hdl, prm, ret_node);
        if (0!=ret) {
            UB_LOG(UBL_ERROR, "%s:setting container key failed. node=[%s]\n",
                   __func__, xpathSegment);
            nconf_xmlutils_free_node(ret_node);
            ret_node=NULL;
            goto erexit;
        }
    }

erexit:
    return ret_node;
}

static int nconf_ucclient_set_container_key_nodes(struct _ucclient_hdl *hdl,
                                                  nconf_ucclcfg_args_t *prm,
                                                  nconf_nodeptr_t container)
{
    int ret=-1, i;
    for(i=0; i<prm->instNode.instkey.vknum; i++) {
        nconf_nodeptr_t ret_node=nconf_xmlutils_new_node(container,
            prm->instNode.instkey.vk[i].key, prm->instNode.instkey.vk[i].val);
        if(ret_node!=NULL) {
            ret=nconf_ucclient_update_nsparams(hdl, prm, ret_node);
            if (ret != 0) {
                UB_LOG(UBL_ERROR, "%s:failed to update ns params\n", __func__);
            }
            nconf_xmlutils_release_node(ret_node);
        } else {
            UB_LOG(UBL_ERROR, "%s:adding new node() failed\n", __func__);
        }
    }
    return ret;
}

static char* nconf_ucclient_tokenize_xpath(char *xpath, char delim, char **saveptr)
{
    char *token=NULL;
    char *tmp=NULL;

    if (xpath != NULL) {
        token=xpath;
    } else if (*saveptr == NULL) {
        goto erexit;
    } else {
        /* advance to the next token */
        token=(*saveptr)+1;
    }

    if (*token != '\0') {
        if (token[0]==delim) { token++; }
        tmp=token;
        token=strchr(tmp, delim);
        if (token) { *token = '\0'; }
        if (NULL != saveptr) { *saveptr=token; }
    }
erexit:
    return tmp;
}

static nconf_nodeptr_t nconf_ucclient_get_module_node(struct _ucclient_hdl *hdl,
                                                      nconf_ucclcfg_args_t *prm)
{
    nconf_nodeptr_t dbtree_rnode=NULL;
    nconf_nodeptr_t dbtree_child=NULL;
    nconf_nodeptr_t child=NULL;
    char *container_name=&(prm->cfg->pathnomod[1]);
    char *tmp=strchr(container_name, '/');

    if (NULL != tmp) { tmp[0]=0; }

    /* ignore some db nodes not related to yang */
    if(nconf_ucclient_not_ignore_leaf(container_name)==0) {
        goto endoper;
    }

    dbtree_rnode=nconf_xmlutils_get_root_node(hdl->dbtree);
    if(dbtree_rnode==NULL) {
        UB_LOG(UBL_ERROR, "%s:failed to get dbtree root node\n", __func__);
        goto endoper;
    }

    /* find the container from current dbtree or create new node if not existing */
    child=XMLUTILS_NODE_GET_CHILD(dbtree_rnode);
    while(child) {
        if (strcmp(XMLUTILS_NODE_GET_NAME(child), container_name)==0) {
            dbtree_child=child;
            break;
        }
        /* get next sibling */
        nconf_nodeptr_t sibling=XMLUTILS_NODE_GET_SIBLING(child);
        nconf_xmlutils_release_node(child);
        child=sibling;
    }

    if (NULL==dbtree_child) {
        dbtree_child=nconf_xmlutils_new_node(dbtree_rnode, container_name, NULL);
        if (NULL!=dbtree_child) {
            if (nconf_ucclient_update_nsparams(hdl,prm,dbtree_child)!= 0) {
                UB_LOG(UBL_ERROR, "%s:failed to update ns params\n", __func__);
            }
            if (hdl->txfunc.txhdr(dbtree_child, hdl->txfunc.uargs)>0) {
                nconf_ucclient_add_to_hdrsent(&hdl->txfunc,
                            XMLUTILS_NODE_GET_NAME(dbtree_child));
            } else {
                UB_LOG(UBL_ERROR, "%s:failed to send %s node header\n",
                       __func__, XMLUTILS_NODE_GET_NAME(dbtree_child));
            }
        } else {
            UB_LOG(UBL_ERROR, "%s:failed to allocate new node\n", __func__);
        }
    }

endoper:
    if (NULL != tmp) { tmp[0]='/'; }
    if (NULL != dbtree_rnode) { nconf_xmlutils_release_node(dbtree_rnode); }
    return dbtree_child;
}

static int nconf_ucclient_update_nsparams(struct _ucclient_hdl *hdl,
                                          nconf_ucclcfg_args_t *prm,
                                          nconf_nodeptr_t dstnode)
{
    char *module_name=&(prm->cfg->path[1]);
    char *tmp=&(prm->cfg->pathnomod[0]);
    char *nodepath=NULL;

    if (NULL != tmp) { tmp[0]=0; }

    nodepath=nconf_xmlutils_get_module_node_path(module_name, dstnode, 0);
    if (NULL != nodepath) {
        nconf_ucclient_acquire_dbal(hdl);
        const char *nodens=yang_node_get_npath_namespace(hdl->dbald, nodepath);
        if (NULL != nodens) {
            nconf_xmlutils_new_node_namespace(dstnode, (char*)nodens, NULL);
        }
        nconf_ucclient_release_dbal(hdl);
        nconf_xmlutils_free_node_path(nodepath);
    } else {
        UB_LOG(UBL_ERROR, "%s:failed to get node path\n", __func__)
    }

    if (NULL != tmp) { tmp[0]='/'; }
    return 0;
}

static int nconf_ucclient_set_node_contents(struct _ucclient_hdl *hdl,
                                            nconf_ucclcfg_args_t *prm,
                                            nconf_nodeptr_t node)
{
    int ret=0;
    if (yang_isstring_vtype(prm->cfg->vtype)) {
        ret=nconf_ucclient_set_strtype_node_contents(prm, node);
    } else {
        ret=nconf_ucclient_set_generic_node_contents(hdl, prm, node);
    }
    return ret;
}

static int nconf_ucclient_set_strtype_node_contents(nconf_ucclcfg_args_t *prm,
                                                    nconf_nodeptr_t node)
{
    nconf_nodeptr_t ref_node=nconf_xmlutils_copy_node(node, 1);
    nconf_nodeptr_t new_node=NULL, sibling=NULL;
    char *stringlist=(char*)prm->cfg->value;
    uint32_t remaining_len=prm->cfg->vsize;
    uint32_t curidx=0;
    int ret=0;
    while(remaining_len > 0) {
        uint32_t curlen=strlen(&stringlist[curidx])+1;
        char *vstr=NULL;
        int isUpdated=0;
        vstr=nconf_xmlutils_sanitize_value_string(&stringlist[curidx], &isUpdated);
        if(curidx > 0) {
            new_node=nconf_xmlutils_copy_node(ref_node, 1);
            nconf_xmlutils_set_node_content(new_node, vstr);
            sibling=nconf_xmlutils_add_sibling_node(node, new_node);
            if (NULL==sibling) {
                UB_LOG(UBL_ERROR, "%s:failed to add node siblings\n", __func__);
                nconf_xmlutils_release_node(new_node);
                break;
            }
            nconf_xmlutils_release_node(new_node);
            nconf_xmlutils_release_node(sibling);
        } else {
            nconf_xmlutils_set_node_content(node, vstr);
        }
        if (isUpdated) { nconf_xmlutils_release_sanitize_value(vstr); }
        if (curlen > remaining_len) {
            UB_LOG(UBL_WARN, "%s:curlen greater than remaining length\n", __func__);
            break;
        }
        /* update remaining length and current index */
        remaining_len-=curlen;
        curidx+=curlen;
    }
    nconf_xmlutils_free_node(ref_node);
    return ret;
}

static int nconf_ucclient_set_generic_node_contents(struct _ucclient_hdl *hdl,
                                                    nconf_ucclcfg_args_t *prm,
                                                    nconf_nodeptr_t node)
{
    nconf_nodeptr_t new_node=NULL, sibling=NULL;
    char *vstr=NULL;
    uint32_t vnum, i;
    int csize;
    int ret=0;
    csize=yang_sizeof_vtype(prm->cfg->vtype);
    if (-1 != csize) {
        vnum=prm->cfg->vsize/csize;
    } else {
        UB_LOG(UBL_WARN, "%s:can't get size of config=[%s] data\n",
               __func__, prm->cfg->name);
        vnum=1;
    }
    for(i=0; i<vnum; i++) {
        if (i > 0) {
            new_node=nconf_xmlutils_copy_node(node, 1);
            vstr=nconf_ucclient_get_vstr(hdl, prm, i, new_node);
            if (NULL!=vstr) {
                if (0 != vstr[0]) {
                    nconf_xmlutils_set_node_content(new_node, vstr);
                    sibling=nconf_xmlutils_add_sibling_node(node, new_node);
                    nconf_xmlutils_release_node(new_node);
                    nconf_xmlutils_release_node(sibling);
                }
                nconf_memfree(vstr);
            }
        } else {
            vstr=nconf_ucclient_get_vstr(hdl, prm, i, node);
            if (NULL!=vstr) {
                if (0 != vstr[0]) {
                    nconf_xmlutils_set_node_content(node, vstr);
                }
                nconf_memfree(vstr);
            }
        }
    }
    return ret;
}

static char *nconf_ucclient_get_vstr(struct _ucclient_hdl *hdl,
                                     nconf_ucclcfg_args_t *prm,
                                     uint32_t idx, nconf_nodeptr_t node)
{
    char *retbuf=NULL;
    char *vstr=yang_value_string(prm->cfg->vtype, prm->cfg->value,
                                 prm->cfg->vsize, idx, prm->cfg->name);
    if (NULL!=vstr) {
        uint16_t vallen=strlen(vstr)+1;
        char *nsprefix=nconf_ucclient_get_nsprefix(hdl, prm, idx, node);
        if (NULL != nsprefix && 0 != nsprefix[0]) {
            vallen+=strlen(nsprefix)+1; //+1 for ':'
        }
        retbuf=(char*)nconf_memalloc(vallen+1);
        if(ub_assert_fatal(NULL!=retbuf, __func__, "alloc error")){return NULL;}
        memset(retbuf, 0, vallen+1);
        if (NULL != nsprefix && 0 != nsprefix[0]) {
            snprintf(retbuf, vallen, "%s:%s", nsprefix, vstr);
        } else {
            strcpy(retbuf, vstr);
        }
    } else {
        UB_LOG(UBL_ERROR, "%s:config=[%s] value string conversion failed\n",
               __func__, prm->cfg->path);
    }
    return retbuf;
}

static char *nconf_ucclient_get_nsprefix(struct _ucclient_hdl *hdl,
                                         nconf_ucclcfg_args_t *prm,
                                         uint32_t idx, nconf_nodeptr_t node)
{
    char *nsprefix=yang_value_namespace(prm->cfg->vtype, prm->cfg->value, idx,
                                        prm->cfg->name);
    if (NULL != nsprefix && 0 != nsprefix[0]) {
        nconf_ucclient_acquire_dbal(hdl);
        char *modns=(char*)yang_node_get_prefix_namespace(hdl->dbald, nsprefix);
        if (NULL != modns) {
            char *nsasc=nconf_xmlutils_get_node_namespace_uri(node);
            /* if data type namespace is diff from node namespace,
             * need to defined the namespace */
            if (NULL != nsasc && strcmp(modns, nsasc) != 0) {
                nconf_xmlutils_new_node_namespace(node, modns, nsprefix);
            } else {
                nsprefix=NULL;
            }
        } else {
            UB_LOG(UBL_ERROR, "%s:invalid nsprefix=[%s]\n", __func__, nsprefix);
            nsprefix=NULL;
        }
        nconf_ucclient_release_dbal(hdl);
    }
    return nsprefix;
}

static int nconf_ucclient_insert_new_cfg_nodes(struct _ucclient_hdl *hdl,
                                               nconf_ucclcfg_args_t *prm)
{
    nconf_nodeptr_t rootnode=NULL;
    nconf_nodeptr_t parent_node=NULL;
    nconf_nodeptr_t prev_parent=NULL;
    nconf_nodeptr_t node=NULL;
    bool ignore_node=false;
    char *tmp=strchr(&prm->cfg->pathnomod[1], '/');
    char *pathSegment=NULL;
    char *containerSp=NULL;
    char *nodename=NULL;
    int ret=0;

    rootnode=nconf_ucclient_get_module_node(hdl, prm);
    if (NULL==rootnode) { return ret; } //ignore node

    /* set the initial parent node to the module's root node */
    parent_node=rootnode;

    // create nodes for each segment of the XPath expression
    pathSegment=nconf_ucclient_tokenize_xpath(tmp, '/', &containerSp);
    while (parent_node != NULL && pathSegment != NULL && 0==ret) {
        nconf_instkeylist_t *keylist=NULL;
        if (strstr(pathSegment, "|") != NULL) {
            ret=nconf_ucclient_get_instance_keys(prm, pathSegment);
            if (0 != ret) {
                if (NULL != containerSp) { *containerSp='/'; }
                UB_LOG(UBL_ERROR, "%s:getting node keys failed: %s\n",
                       __func__, prm->cfg->path);
                break;
            }
            nodename=prm->instNode.name;
            keylist=&prm->instNode.instkey;
        } else {
            nodename=pathSegment;
        }

        node=nconf_ucclient_get_existing_node(parent_node, keylist, nodename);
        if (NULL==node) {
            if(nconf_ucclient_not_ignore_leaf(nodename)) {
                node=nconf_ucclient_get_new_node(hdl, prm, parent_node, nodename);
                if (NULL==node) {
                    UB_LOG(UBL_ERROR, "%s:getting new node=[%s] failed\n",
                           __func__, nodename);
                    ret=-1;
                }
                ignore_node=false;
            } else {
                ignore_node=true;
                break;
            }
        }
        memset(&prm->instNode, 0, sizeof(prm->instNode));

        /* update for next instance */
        prev_parent=parent_node;
        parent_node=node;
        if (NULL != containerSp) { *containerSp='/'; }
        pathSegment=nconf_ucclient_tokenize_xpath(NULL, '/', &containerSp);

        /* release temporary resources */
        if (NULL != prev_parent) { nconf_xmlutils_release_node(prev_parent); }
    }

    if (0==ret && NULL != node && !ignore_node) {
        ret=nconf_ucclient_set_node_contents(hdl, prm, node);
        if (0 != ret) {
            UB_LOG(UBL_ERROR, "%s:Setting node content failed [cfg=%s]\n",
                   __func__, prm->cfg->path);
        }
    }
    if (NULL != node) { nconf_xmlutils_release_node(node); }
    return ret;
}

static int nconf_ucclient_get_instance_keys(nconf_ucclcfg_args_t *prm,
                                            char *xpathSegment)
{
    char *instKeys=NULL, *instKeySp=NULL, *valKeySp=NULL;
    int ret=-1;

    memset(&prm->instNode, 0, sizeof(prm->instNode));
    prm->instNode.name=nconf_ucclient_tokenize_xpath(xpathSegment, '|', &instKeySp);
    instKeys=nconf_ucclient_tokenize_xpath(NULL, '|', &instKeySp);
    while(NULL != instKeys) {
        prm->instNode.instkey.vk[prm->instNode.instkey.vknum].key=
            nconf_ucclient_tokenize_xpath(instKeys, ':', &valKeySp);
        if (prm->instNode.instkey.vk[prm->instNode.instkey.vknum].key==NULL) {
            memset(&prm->instNode, 0, sizeof(prm->instNode));
            if (NULL != instKeySp){ *instKeySp='|'; }
            if (NULL != valKeySp){ *valKeySp=':'; }
            UB_LOG(UBL_WARN, "%s:null key detected on xpathSegment=[%s]\n",
                   __func__, xpathSegment);
            break;
        }
        prm->instNode.instkey.vk[prm->instNode.instkey.vknum].val=
            nconf_ucclient_tokenize_xpath(NULL, '|', &valKeySp);
        if (prm->instNode.instkey.vk[prm->instNode.instkey.vknum].val==NULL) {
            memset(&prm->instNode, 0, sizeof(prm->instNode));
            if (NULL != instKeySp){ *instKeySp='|'; }
            if (NULL != valKeySp){ *valKeySp='|'; }
            UB_LOG(UBL_WARN, "%s:null value detected on xpathSegment=[%s]\n",
                   __func__, xpathSegment);
            break;
        }
        prm->instNode.instkey.vknum++;
        instKeys=nconf_ucclient_tokenize_xpath(NULL, '|', &instKeySp);
    }

    if (prm->instNode.instkey.vknum > 0) { ret=0; }
    return ret;
}

static int nconf_ucclient_update_config_cache(struct ub_list *cfglist,
                                              char *cfgpath, int vtype,
                                              void *value, uint32_t vsize)
{
    int ret=NCONF_RPC_ERROR_TAG_NO_ERROR;
    struct ub_list_node *node=NULL;
    void *valtmp=NULL;

    nconf_cfginfo_t *cfginfo=(nconf_cfginfo_t*)
        UB_SD_GETMEM(NCONF_CFG_CACHE_DATAINST, sizeof(nconf_cfginfo_t));
    if(ub_assert_fatal((NULL!=cfginfo), __func__, "cfg info alloc error")){
        ret=NCONF_RPC_ERROR_TAG_TOO_BIG;
        goto end_oper;
    }

    /* allocate memory to copy config value */
    valtmp=nconf_memalloc(vsize);
    if(ub_assert_fatal((NULL!=valtmp), __func__, "valtmp alloc error")){
        UB_SD_RELMEM(NCONF_CFG_CACHE_DATAINST, cfginfo);
        ret=NCONF_RPC_ERROR_TAG_TOO_BIG;
        goto end_oper;
    }
    (void)memset(valtmp, 0, vsize);
    (void)memcpy(valtmp, value, vsize);
    cfginfo->path=nconf_strdup(cfgpath);
    if(ub_assert_fatal((NULL!=cfginfo->path), __func__, "path alloc error")){
        nconf_memfree(valtmp);
        UB_SD_RELMEM(NCONF_CFG_CACHE_DATAINST, cfginfo);
        ret=NCONF_RPC_ERROR_TAG_TOO_BIG;
        goto end_oper;
    }
    cfginfo->pathnomod=strchr(&(cfginfo->path[1]), '/');
    if (NULL == cfginfo->pathnomod) {
        nconf_memfree(valtmp);
        UB_SD_RELMEM(NCONF_CFG_CACHE_DATAINST, cfginfo);
        ret=NCONF_RPC_ERROR_TAG_OPERATION_FAILED;
        goto end_oper;
    }
    cfginfo->name=strrchr(cfginfo->path, '/');
    if (NULL != cfginfo->name) { cfginfo->name=cfginfo->name+1; }
    cfginfo->vtype=vtype;
    cfginfo->value=valtmp;
    cfginfo->vsize=vsize;

    node=(struct ub_list_node*)
        UB_SD_GETMEM(NCONF_CFG_CACHE_LISTNODE, sizeof(struct ub_list_node));
    if(ub_assert_fatal(node!=NULL, __func__, "ub_list_node alloc failed")){
        nconf_memfree(valtmp);
        UB_SD_RELMEM(NCONF_CFG_CACHE_DATAINST, cfginfo);
        ret=NCONF_RPC_ERROR_TAG_TOO_BIG;
        goto end_oper;
    }
    (void)memset(node, 0, sizeof(struct ub_list_node));
    node->data=cfginfo;
    ub_list_append(cfglist, node);

end_oper:
    return ret;
}

static int nconf_ucclient_cmp_dbpath(char *filter, char *dbpath)
{
    int ret=0;
    char *tmp=strrchr(filter, '|');
    if (tmp != NULL) {
        size_t endpos=strlen(filter);
        if (filter[endpos-1] == '/') {
            filter[endpos-1]='\0';
            ret=strncmp(filter, dbpath, endpos-1);
            filter[endpos-1]='/';
        }
    }
    return ret;
}

static int nconf_ucclient_cmp_kvs(uint8_t *vp, void **kvs, uint8_t *kss)
{
    int i;
    for (i=0;i<UC_MAX_KV_DEPTH;i++){
        if(kss[i]==0){return 0;}
        if(kvs[i]==NULL){
            vp=&vp[vp[0]+1];
            continue;
        }
        if(kss[i]!=vp[0]){return 1;}
        if(memcmp(&vp[1], kvs[i], kss[i])!=0){return 1;}
        vp=&vp[vp[0]+1];
    }
    return -1;
}

static bool nconf_ucclient_is_yang_config(uc_dbald *dbald, char *cfgstr)
{
    char *q, *p=cfgstr;
    uint8_t modkey=255u;
    bool bret=false;
    char *tmp=strstr(cfgstr, "valuekey");
    if (NULL!=tmp) { goto end_oper; }
    tmp=strstr(cfgstr, "nsasc");
    if (NULL!=tmp) { goto end_oper; }
    if(*p=='/'){
        p++;
        q=strchr(p, '/');
        if(q!=NULL){ *q=0; }
        modkey=yang_node_mod_get_enum(dbald, p);
        if(q!=NULL){ *q='/'; }
    }
    if (modkey>XL4_EXTMOD_RW && modkey<XL4_DATA_RO){bret=true;}
end_oper:
    return bret;
}

static void nconf_ucclient_release_config_cache(nconf_cfginfo_t *cfg)
{
    if (NULL != cfg) {
        if (NULL != cfg->path){nconf_memfree(cfg->path);}
        if (NULL != cfg->value){nconf_memfree(cfg->value);}
        UB_SD_RELMEM(NCONF_CFG_CACHE_DATAINST, cfg);
    }
}

static int nconf_ucclient_getvknum(uc_dbald *dbald, uint8_t *aps, bool *pisdiff)
{
    uint8_t vknum=yang_db_runtime_getvknum(dbald, aps);
    if (0==vknum) {
        aps[0] |= XL4_DATA_RO; //retry with RO
        vknum=yang_db_runtime_getvknum(dbald, aps);
        if(vknum > 0) {
            if (NULL != pisdiff) { *pisdiff=true; }
        }
    }
    return vknum;
}

static int nconf_ucclient_getvktype(uc_dbald *dbald, uint8_t *aps, uint8_t vkindex,
                                    uint8_t *vkey, uint8_t *vtype)
{
    int ret=yang_db_runtime_getvkvtype(dbald, aps, vkindex, vkey, vtype);
    if (0!=ret) {
        aps[0] |= XL4_DATA_RO; //retry with RO
        ret=yang_db_runtime_getvkvtype(dbald, aps, vkindex, vkey, vtype);
    }
    return ret;
}

static int nconf_ucclient_getvkstr(uc_dbald *dbald, uint8_t *aps, uint8_t vkindex,
                                   char **rstr)
{
    int ret=yang_db_runtime_getvkstr(dbald, aps, vkindex, rstr);
    if (0!=ret) {
        aps[0] |= XL4_DATA_RO; //retry with RO
        ret=yang_db_runtime_getvkstr(dbald, aps, vkindex, rstr);
    }
    return ret;
}

static int nconf_ucclient_process_dbkeys(nconf_uccl_t dbhdl, void *key, uint32_t ksize,
                                         void *value, uint32_t vsize, char *filter,
                                         struct ub_list *cfglist)
{
    int ret=NCONF_RPC_ERROR_TAG_NO_ERROR, vtype;
    char *prstr=NULL;

    if(yang_db_runtime_getkeyvkstr(dbhdl->dbald, key, ksize, &prstr)){goto end_oper;}
    if(!nconf_ucclient_is_yang_config(dbhdl->dbald, prstr)){goto end_oper;}

    if(dbhdl->dbacc.kss[0]!=0) {
        /* if filter include leaf keys, skip nodes with different leak keys */
        if(nconf_ucclient_cmp_dbpath(filter, prstr)) {
            UB_LOG(UBL_DEBUGV, "%s:-> prstr=%s (skipped)\n", __func__, prstr);
            goto end_oper;
        }
    }
    vtype=yang_db_runtime_get_vtype(dbhdl->dbald, (uint8_t *)key);

    UB_LOG(UBL_DEBUGV, "%s:-> prstr: %s\n", __func__, prstr);
    if (vsize >= NCONF_MAX_UCCFG_VSIZE_LIMIT) {
        UB_LOG(UBL_DEBUGV, "%s:large vsize data: %u\n", __func__, vsize);
        ret=nconf_ucclient_send_single_cfg_node(dbhdl,prstr,vtype,value,vsize);
        if (NCONF_RPC_ERROR_TAG_NO_ERROR != ret) {
            UB_LOG(UBL_WARN, "%s:failed send cfg node prstr=[%s]\n",__func__,prstr);
        }
    } else {
        ret=nconf_ucclient_update_config_cache(cfglist,prstr,vtype,value,vsize);
        if (NCONF_RPC_ERROR_TAG_NO_ERROR != ret) {
            UB_LOG(UBL_WARN, "%s:cache update failed prstr=[%s]\n",__func__,prstr);
        }
    }

end_oper:
    UB_SD_RELMEM(YANGINIT_GEN_SMEM, prstr);
    return ret;
}

static int nconf_ucclient_get_schema_namespace(nconf_uccl_t dbhdl, char *module,
                                               char *version, char *format)
{
    char *tmpbuf=NULL;
    void *value=NULL;
    uint32_t vsize=0;
    int res=-1;

    tmpbuf=(char*)nconf_memalloc(NCONF_MAX_UCCFG_LINE_LEN);
    if(ub_assert_fatal(NULL!=tmpbuf, __func__, "alloc error")){return -1;}

    memset(tmpbuf, 0, NCONF_MAX_UCCFG_LINE_LEN);
    snprintf(tmpbuf,NCONF_MAX_UCCFG_LINE_LEN-1,NCONF_MODULE_SCHME_GET_FMT,
             module, version, format);

    res=nconf_ucclient_exec_config_read(dbhdl, tmpbuf, &value, &vsize);
    if (res >= 0) {
        UB_LOG(UBL_DEBUG, "%s:schema found [%s]\n",__func__,((char*)value));
        UB_SD_RELMEM(YANGINIT_GEN_SMEM, value);
    } else {
        UB_LOG(UBL_ERROR, "%s:schema not in db [%s]\n", __func__, module);
    }

    nconf_memfree(tmpbuf);
    return res;
}

static int nconf_ucclient_schema_search(nconf_uccl_t dbhdl, char *module,
                                        char **pversion, char **pformat)
{
    void *key=NULL, *value=NULL;
    uint32_t ksize, vsize, i;
    char *tmpbuf=NULL;
    int res=-1;

    tmpbuf=(char*)nconf_memalloc(NCONF_MAX_UCCFG_LINE_LEN);
    if(ub_assert_fatal(NULL!=tmpbuf, __func__, "alloc error")){return -1;}
    memset(tmpbuf, 0, NCONF_MAX_UCCFG_LINE_LEN);
    snprintf(tmpbuf, NCONF_MAX_UCCFG_LINE_LEN-1, NCONF_MODULE_SCHEMA_SEARCH_FMT,
             module);

    memset(dbhdl->dbacc.kvs, 0, sizeof(dbhdl->dbacc.kvs));
    memset(dbhdl->dbacc.kss, 0, sizeof(dbhdl->dbacc.kss));
    dbhdl->dbacc.range=yang_db_runtime_range_fromline(dbhdl->ydrd, tmpbuf,
                                dbhdl->dbacc.kvs, dbhdl->dbacc.kss, true);
    while(NULL != dbhdl->dbacc.range){
        char *prstr=NULL;
        if(uc_get_keyvalue_in_range(dbhdl->dbald, dbhdl->dbacc.range, &key,
                                    &ksize,&value,&vsize,UC_DBAL_FORWARD)){
            break;
        }
        if(dbhdl->dbacc.kss[0]!=0) {
            for(i=0;i<ksize;i++){
                if(((uint8_t *)key)[i]==255){break;}
            }
            if(nconf_ucclient_cmp_kvs(&((uint8_t *)key)[i+1],
                        dbhdl->dbacc.kvs, dbhdl->dbacc.kss)){
                continue;
            }
        }
        if(yang_db_runtime_getkeyvkstr(dbhdl->dbald, key, ksize, &prstr)){
            continue;
        }
        if(nconf_ucclient_validate_schema_params(prstr, pversion, pformat)){
            UB_SD_RELMEM(YANGINIT_GEN_SMEM, prstr);
            continue;
        }
        UB_SD_RELMEM(YANGINIT_GEN_SMEM, prstr);
        res=0; // schema found
        break;
    }

    if (NULL != dbhdl->dbacc.range) {
        uc_get_range_release(dbhdl->dbald, dbhdl->dbacc.range);
        dbhdl->dbacc.range=NULL;
    }
    nconf_ucclient_release_dbal(dbhdl);
    nconf_memfree(tmpbuf);
    return res;
}

static int nconf_ucclient_validate_schema_params(char *dbpath, char **pversion,
                                                 char **pformat)
{
    char *dbversion=NULL;
    char *dbformat=NULL;
    int res=nconf_ucclient_get_dbpath_schema_keys(dbpath,&dbversion,&dbformat);
    if (res==0) {
        char *dbversion_dup=NULL;
        char *dbformat_dup=NULL;
        res=-1;
        if (NULL != *pversion) {
            if(strcmp(*pversion, dbversion)!=0) { goto end_oper; }
        } else {
            dbversion_dup=nconf_strdup(dbversion);
            if(ub_assert_fatal(NULL!=dbversion_dup, __func__, "alloc error")){
                goto end_oper;
            }
        }
        if (NULL != *pformat) {
            if(strcmp(*pformat, dbformat)!=0) {
                if (NULL != dbversion_dup) { nconf_memfree(dbversion_dup); }
                goto end_oper;
            }
        } else {
            dbformat_dup=nconf_strdup(dbformat);
            if(ub_assert_fatal(NULL!=dbformat_dup, __func__, "alloc error")){
                if (NULL != dbversion_dup) { nconf_memfree(dbversion_dup); }
                goto end_oper;
            }
        }
        if (NULL != dbversion_dup) { *pversion=dbversion_dup; }
        if (NULL != dbformat_dup)  { *pformat=dbformat_dup; }
        res=0;
    } else {
        UB_LOG(UBL_ERROR,"%s:invalid dbpath=[%s]\n", __func__, dbpath);
    }
end_oper:
    return res;
}

static int nconf_ucclient_get_dbpath_schema_keys(char *dbpath, char **pversion,
                                                 char **pformat)
{
    char *dbversion=NULL, *dbformat=NULL;
    char *keyblock=NULL, *tmpptr=NULL;
    int res=-1;

    keyblock=strchr(dbpath, '|'); // get keyblock start
    if (NULL==keyblock) { goto end_oper; }

    tmpptr=strchr(keyblock, '/'); // get keyblock end
    if (NULL==tmpptr) { goto end_oper; }

    tmpptr[0]='\0';
    tmpptr=strchr(&keyblock[1], '|'); // get end of 'indentifier' keyblock
    if (NULL==tmpptr) { goto end_oper; }

    tmpptr=strchr(&tmpptr[1], ':'); // get end of key name in 'version' keyblock
    if (NULL==tmpptr) { goto end_oper; }

    dbversion=&tmpptr[1];
    tmpptr=strchr(dbversion, '|'); // get end of 'version' keyblock
    if (NULL==tmpptr) { goto end_oper; }

    tmpptr[0]='\0';
    tmpptr=strchr(&tmpptr[1], ':'); // get end of key name in 'format' keyblock
    if (NULL==tmpptr) { goto end_oper; }

    dbformat=&tmpptr[1];
    tmpptr=strchr(dbformat, '|');
    if (NULL==tmpptr) { goto end_oper; }

    tmpptr[0]='\0';
    *pversion=dbversion;
    *pformat=dbformat;
    res=0;

end_oper:
    return res;
}

static bool nconf_ucclient_is_nodehdr_sent(nconf_uccl_t hdl, char *nodename)
{
    bool found=false;
    char *segment=strchr(hdl->txfunc.hdrsent, '/');
    while(segment != NULL) {
        char *tmp=strchr(&segment[1], '/');
        if (tmp != NULL) { tmp[0]='\0'; }
        if (strcmp(&segment[1], nodename)==0) {
            if (tmp != NULL) { tmp[0]='/'; }
            found=true;
            break;
        }
        if (tmp != NULL) { tmp[0]='/'; }
        segment=tmp;
    }
    return found;
}

static void nconf_ucclient_add_to_hdrsent(nconf_uccl_txfunc_t *txfunc, char *nodename)
{
    char *oldptr=txfunc->hdrsent;
    size_t newsize=0;
    if (NULL != oldptr) {
        newsize=strlen(oldptr)+strlen(nodename)+2; //+2 for '/' and null pointer
    } else {
        newsize=strlen(nodename)+2; //+2 for '/' and null pointer
    }
    if (newsize > txfunc->hdrsz) {
        txfunc->hdrsent=(char*)nconf_memrealloc(oldptr, newsize);
        if (NULL !=txfunc->hdrsent) {
            memset(&txfunc->hdrsent[txfunc->hdrsz], 0, (newsize-txfunc->hdrsz));
            txfunc->hdrsz=newsize;
        }
    }
    if (NULL !=txfunc->hdrsent) {
        if (txfunc->hdrsent[0]!='\0') {
            strcat(txfunc->hdrsent, "/");
            strcat(txfunc->hdrsent, nodename);
        } else {
            sprintf(txfunc->hdrsent, "/%s", nodename);
        }
    }
}

static char* nconf_ucclient_get_path_instance(char *nodepath)
{
    char *retptr=NULL, *tmp=NULL;
    tmp=strrchr(nodepath, '/');
    if (NULL != tmp) { tmp[0]='\0'; }
    retptr=nconf_strdup(nodepath);
    if (NULL==retptr) {
        UB_LOG(UBL_ERROR, "%s:Failed duplicate node instance [inst=%s]\n",
               __func__, nodepath);
    }
    if (NULL != tmp) { tmp[0]='/'; } // revert changes
    return retptr;
}

static void nconf_ucclient_delete_child_node(nconf_uccl_t hdl, char *nodename)
{
    nconf_nodeptr_t parent=nconf_xmlutils_get_root_node(hdl->dbtree);
    if (NULL==parent) { return; } // dbtree not yet initialized

    char *hdrsent=nconf_strdup(hdl->txfunc.hdrsent);
    if (NULL != hdrsent) {
        char *pathSegment=NULL;
        char *containerSp=NULL;
        char *temp=strrchr(hdrsent, '/');
        if (NULL != temp) { temp[0]='\0'; }

        pathSegment=nconf_ucclient_tokenize_xpath(hdrsent, '/', &containerSp);
        while (pathSegment != NULL) {
            nconf_nodeptr_t child=NULL;
            char *keyblock=strchr(pathSegment, '|');
            if (NULL != keyblock) { keyblock[0]='\0'; }
            child=nconf_xmlutils_get_node_child_by_name(parent, pathSegment);
            if (NULL==child) {
                UB_LOG(UBL_ERROR, "%s:child node not found: %s\n",
                       __func__, pathSegment);
                if (NULL != keyblock) { keyblock[0]='|'; }
                break;
            }
            if (NULL != keyblock) { keyblock[0]='|'; }
            /* update for next instance */
            nconf_xmlutils_release_node(parent);
            parent=child;
            pathSegment=nconf_ucclient_tokenize_xpath(NULL, '/', &containerSp);
        }
        nconf_nodeptr_t instnode=
            nconf_xmlutils_get_node_child_by_name(parent, nodename);
        if (NULL != instnode) {
            nconf_xmlutils_delete_child_node(parent, instnode);
            nconf_xmlutils_release_node(instnode);
        } else {
            UB_LOG(UBL_ERROR, "%s:node=<%s> not found in <%s>\n",
                   __func__, nodename, XMLUTILS_NODE_GET_NAME(parent));
        }
        if (NULL != temp) { temp[0]='/'; }
        nconf_memfree(hdrsent);
    }
    nconf_xmlutils_release_node(parent);
}

static void nconf_ucclient_send_child_nodes(nconf_uccl_t hdl, nconf_nodeptr_t parent,
                                            char *lastHdrSent, char *nextSegment)
{
    nconf_nodeptr_t child=NULL;
    char *hdrtmp=NULL, *segtmp=NULL;
    /* if node header is recently sent and it contains a key block
     * temoporarily remove the key block checking if already sent */
    if (NULL != lastHdrSent) {
        hdrtmp=strchr(lastHdrSent, '|');
        if (NULL != hdrtmp) { hdrtmp[0]='\0'; }
    }
    if (NULL != nextSegment) {
        segtmp=strchr(nextSegment, '|');
        if (NULL != segtmp) { segtmp[0]='\0'; }
    }
    child=XMLUTILS_NODE_GET_CHILD(parent);
    while (child != NULL) {
        nconf_nodeptr_t sibling=XMLUTILS_NODE_GET_SIBLING(child);
        char *chname=XMLUTILS_NODE_GET_NAME(child);
        if(!nconf_ucclient_is_nodekey_sent(hdl, child)) {
            if (NULL != nextSegment && strcmp(chname, nextSegment)==0) {
                UB_LOG(UBL_DEBUGV, "%s:skipping next segment node=<%s>\n",
                       __func__, chname);
            } else if (hdl->txfunc.txnode(child, hdl->txfunc.uargs) > 0) {
                UB_LOG(UBL_DEBUGV, "%s:->txnode: %s\n", __func__, chname);
                /* after sending, the instnode can be deleted from parent node
                 * after getting its sibling */
                nconf_xmlutils_delete_child_node(parent, child);
            } else {
                UB_LOG(UBL_ERROR, "%s:failed to send instnode=<%s>\n",
                       __func__, chname);
                if (NULL != sibling) { nconf_xmlutils_release_node(sibling); }
                if (NULL != child)   { nconf_xmlutils_release_node(child);   }
                break;
            }
        }
        nconf_xmlutils_release_node(child);
        child=sibling;
    }
    if (NULL != hdrtmp) { hdrtmp[0]='|'; } // restore key block
    if (NULL != segtmp) { segtmp[0]='|'; } // restore key block
}

static bool nconf_ucclient_is_nodekey_sent(nconf_uccl_t hdl, nconf_nodeptr_t child)
{
    bool issent=false;
    char *kvpair=NULL;
    char *chcontent=nconf_xmlutils_get_node_content(child);
    if (NULL != chcontent) {
        kvpair=(char*)nconf_memalloc(NCONF_MAX_UCCFG_LINE_LEN);
        if(ub_assert_fatal(NULL!=kvpair, __func__, "alloc error")){return false;}
        memset(kvpair, 0, NCONF_MAX_UCCFG_LINE_LEN);
        snprintf(kvpair, NCONF_MAX_UCCFG_LINE_LEN-1, "%s:%s",
                XMLUTILS_NODE_GET_NAME(child), chcontent);
        nconf_xmlutils_free((uint8_t*)chcontent);
    }
    if (NULL != kvpair) {
        char *tmp=strstr(hdl->txfunc.hdrsent, kvpair);
        if (NULL != tmp) { issent=true; }
        nconf_memfree(kvpair);
    }
    return issent;
}

static void nconf_ucclient_send_node_instance(nconf_uccl_t hdl, char *instpath)
{
    nconf_nodeptr_t parent=nconf_xmlutils_get_root_node(hdl->dbtree);
    if (NULL==parent) { return; } // dbtree not yet initialized
    nconf_xmlutils_dump_nodeptr(parent);

    char *pathSegment=NULL;
    char *containerSp=NULL;
    char *lastHdrSent=NULL;

    pathSegment=nconf_ucclient_tokenize_xpath(instpath, '/', &containerSp);
    while (pathSegment != NULL) {
        nconf_nodeptr_t child=NULL;
        char *keyblock=strchr(pathSegment, '|');
        if (NULL != keyblock) { keyblock[0]='\0'; }
        child=nconf_xmlutils_get_node_child_by_name(parent, pathSegment);
        if (NULL==child) {
            UB_LOG(UBL_ERROR, "%s:[%s] node not found in %s\n",
                   __func__, pathSegment, XMLUTILS_NODE_GET_NAME(parent));
            if (NULL != keyblock) { keyblock[0]='|'; }
            break;
        }
        if (NULL != keyblock) { keyblock[0]='|'; }
        if (!nconf_ucclient_is_nodehdr_sent(hdl, pathSegment)) {
            if (hdl->txfunc.txhdr(child, hdl->txfunc.uargs)>0) {
                nconf_ucclient_add_to_hdrsent(&hdl->txfunc, pathSegment);
                lastHdrSent=strrchr(hdl->txfunc.hdrsent, '/');
            } else {
                UB_LOG(UBL_ERROR, "%s:failed to send %s node header\n",
                       __func__, pathSegment);
            }
        }

        /* update parent for next container in path */
        nconf_xmlutils_release_node(parent);
        parent=child;
        pathSegment=nconf_ucclient_tokenize_xpath(NULL, '/', &containerSp);
        if (NULL != pathSegment) {
            if (nconf_xmlutils_node_has_children(parent)) {
                nconf_ucclient_send_child_nodes(hdl, parent, lastHdrSent,
                                                pathSegment);
            }
        }
    }

    nconf_ucclient_send_child_nodes(hdl, parent, lastHdrSent, NULL);
    nconf_xmlutils_release_node(parent);
}

static void nconf_ucclient_reset_dbtree_nodes(nconf_uccl_t hdl)
{
    nconf_nodeptr_t rootnode=nconf_xmlutils_get_root_node(hdl->dbtree);
    nconf_xmlutils_delete_child_nodes_of_childs(rootnode);
    nconf_xmlutils_release_node(rootnode);
}

static void nconf_ucclient_send_end_tags(nconf_uccl_t hdl, char *next_nodepath)
{
    char *h1=hdl->txfunc.hdrsent;
    char *h2=next_nodepath;
    char *tmp=NULL;

    if (NULL != h2) {
        /* get the path difference with the next node path*/
        while (*h1 && *h2 && *h1 == *h2) { h1++; h2++; }
    } else {
        if (h1[0]=='/') { h1=&h1[1]; } // if h2 is NULL or empty path
    }

    /* get the last container where the difference occur */
    if (h1 && h1[0]!='\0') {
        char prevval=h1[0];
        if (prevval=='/') {
            h1=&h1[1];
        } else {
            h1[0]='\0';
            tmp=strrchr(hdl->txfunc.hdrsent, '/');
            h1[0]=prevval;
            h1=&tmp[1];
        }
    }

    while(h1 && h1[0]!='\0') {
        char *nodename=NULL;
        tmp=strrchr(h1, '/');
        if (NULL != tmp) {
            nodename=&tmp[1];
        } else {
            nodename=h1;
        }
        char *keyblock=strchr(nodename, '|');
        if (NULL != keyblock) { keyblock[0]='\0'; }
        if (hdl->txfunc.txftr(nodename, hdl->txfunc.uargs) > 0) {
            nconf_ucclient_delete_child_node(hdl, nodename);
            // remove from hdr sent list
            if (NULL != tmp) {
                tmp[0]='\0';
            } else {
                tmp=strrchr(hdl->txfunc.hdrsent, '/');
                if (NULL != tmp) { tmp[0]='\0'; }
                break;
            }
        } else {
            UB_LOG(UBL_ERROR, "%s:failed to send endtag=</%s>\n", __func__, h1);
        }
    }
}

static void nconf_ucclient_send_completed_nodes(nconf_uccl_t dbhdl, char *nodepath,
                                                char **retinst)
{
    char *last_inst=*retinst;
    if (NULL != last_inst) {
        char *newinst=nconf_ucclient_get_path_instance(nodepath);
        if (NULL==newinst) {
            UB_LOG(UBL_ERROR, "%s:failed to get path instance\n", __func__);
            return;
        }
        if (strcmp(last_inst, newinst) != 0) {
            nconf_ucclient_send_node_instance(dbhdl, last_inst);
            nconf_ucclient_send_end_tags(dbhdl, nodepath);
            nconf_memfree(last_inst);
            *retinst=newinst;
            UB_LOG(UBL_DEBUGV, "%s:instance updated: %s\n", __func__, *retinst);
            nconf_ucclient_reset_dbtree_nodes(dbhdl);
        } else {
            nconf_memfree(newinst);
        }
    } else {
        *retinst=nconf_ucclient_get_path_instance(nodepath);
        UB_LOG(UBL_DEBUGV, "%s:instance init: %s\n", __func__, *retinst);
        nconf_ucclient_reset_dbtree_nodes(dbhdl);
    }
}

static void nconf_ucclient_clear_traversed_list(struct ub_list *cfglist, struct ub_list_node *node)
{
    /* clear and release all remaining node in case list traversal
     *  was break at certain point */
    while(node!=NULL){
		struct ub_list_node *next=node->next;
		nconf_ucclient_config_info_release(node, NULL);
		node = next;
    }
    ub_list_init(cfglist);
}

static void nconf_ucclient_acquire_dbal(nconf_uccl_t dbhdl)
{
    dbhdl->dbhdlref++;
    UB_LOG(UBL_DEBUGV, "dbhdl->dbhdlref: +%d\n", dbhdl->dbhdlref);
}

static void nconf_ucclient_release_dbal(nconf_uccl_t dbhdl)
{
    if (dbhdl->dbhdlref > 0) {
        dbhdl->dbhdlref--;
        UB_LOG(UBL_DEBUGV, "dbhdl->dbhdlref: -%d\n", dbhdl->dbhdlref);
    }
    if (dbhdl->dbhdlref==0) { uc_dbal_releasedb(dbhdl->dbald); }
}

