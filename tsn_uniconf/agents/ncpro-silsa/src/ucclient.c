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
 * @file        ucclient.c
 * @author      Allan Co<allanrodrigo.co@excelfore.com>
 * @copyright   Copyright (C) 2023 Excelfore Corporation
 *
 * @brief       Contains Excelfore UNICONF client APIs interfaces definitions
 */

/*=============================================================================
 * Include Files
 *============================================================================*/

#include <tsn_unibase/unibase.h>
#include <tsn_unibase/unibase_binding.h>
#include <tsn_unibase/ub_getmem.h>
#include <tsn_uniconf/yangs/yang_modules.h>
#include <tsn_uniconf/yangs/yang_db_runtime.h>
#include <tsn_uniconf/yangs/yang_db_access.h>
#include <tsn_uniconf/yangs/ieee1588-ptp.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include "ucclient.h"

/*=============================================================================
 * Macros and Constants
 *============================================================================*/

#define UCCLIENT_GEN_SMEM       ucclient_gen_smem
#define MAX_APS_NUM             32
#define MAX_KVS_NUM             32
#define IGNORE_LEAF_KEYWOARD    "dummy"

/*=============================================================================
 * Typedefs, Structures, and Enums
 *============================================================================*/

typedef struct ucclient_cfg_param {
    uint8_t kss[UC_MAX_VALUEKEYS];
    kvs_t kvs[UC_MAX_VALUEKEYS+1]; // +1 for NULL termination
    uint8_t *aps;
    void *value;
    uint32_t vsize;
    char *rstr;
    char *uccfgstr;
    char *uccfgnomod;
    int vtype;
    uint8_t kvi;
    xmlNodePtr kvnode;
} ucclient_cfg_param_t;

/*=============================================================================
 * Function Prototypes
 *============================================================================*/

static char* ucclient_strdup(char *src);
static void ucclient_free(void *ptr);
static char* ucclient_tokenize_xpath(char *xpath, char delim, char **saveptr);
static int ucclient_search_container_keyval(xmlNodePtr container, char *vkstr, char *vstr);
static int ucclient_get_container_vknum(ucclient_cfg_param_t *prm);
static int ucclient_cmp_container_vkval(ucclient_cfg_param_t *prm, xmlNodePtr container);
static int ucclient_set_container_vkval(ucclient_cfg_param_t *prm, xmlNodePtr container);
static xmlNodePtr ucclient_get_existing_node(ucclient_cfg_param_t *prm);
static xmlNodePtr ucclient_get_new_node(ucclient_cfg_param_t *prm, xmlChar *xpathSegment);
static xmlNodePtr ucclient_get_main_container_node(ucclientopt_t *ucopt, char *container_name);
static int ucclient_insert_new_cfg_nodes(ucclientopt_t *ucopt, ucclient_cfg_param_t *prm);
static int ucclient_insert_config(ucclientopt_t *ucopt, ucclient_cfg_param_t *prm);
static int ucclient_xmldoc_init(ucclientopt_t *ucopt);

/*=============================================================================
 * Global Variables
 *============================================================================*/

UB_SD_GETMEM_DEF_EXTERN(UCCLIENT_GEN_SMEM);

static uc_dbald                 *dbald=NULL;
static yang_db_runtime_dataq_t  *ydrd=NULL;
static uc_notice_data_t         *ucntd=NULL;
static xmlXPathContextPtr       xpathCtx=NULL;
static xmlXPathObjectPtr        xpathObj=NULL;
static uint8_t                  notify_aps[MAX_APS_NUM+1]={255};
static void*                    notify_kvs[MAX_KVS_NUM+1]={NULL};
static uint8_t                  notify_kss[MAX_KVS_NUM+1]={0};
static bool                     notify_aps_set=false;

/*=============================================================================
 * Function Definitions
 *============================================================================*/

static char* ucclient_strdup(char *src)
{
    size_t sz=0;
    char *dst=NULL;

    sz=strlen(src)+1;
    dst=(char*)UB_SD_GETMEM(UCCLIENT_GEN_SMEM, sz);
    if(ub_assert_fatal(dst, __func__, "malloc error")){return NULL;}

    memset(dst, 0, sz);
    memcpy(dst, src, sz);
    return dst;
}

static void ucclient_free(void *ptr)
{
    UB_SD_RELMEM(UCCLIENT_GEN_SMEM, ptr);
}

static char* ucclient_tokenize_xpath(char *xpath, char delim, char **saveptr)
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

static int ucclient_search_container_keyval(xmlNodePtr container, char *vkstr, char *vstr)
{
    xmlNodePtr child=NULL;
    xmlChar *tmp_val=NULL;
    char *nstr=strrchr(vkstr, '/');
    int ret=-1;

    if (NULL != nstr) {
        nstr=&nstr[1];
        for (child=container->children; child != NULL; child=child->next) {
            if (xmlStrcmp(child->name, BAD_CAST nstr)==0) {
                tmp_val=xmlNodeGetContent(child);
                if (xmlStrcmp(tmp_val, BAD_CAST vstr)==0) {
                    ret=0;
                    break;
                }
            }
        }
    }

    return ret;
}

static int ucclient_get_container_vknum(ucclient_cfg_param_t *prm)
{
    uint8_t aps[16];
    char *rstr=NULL;
    uint8_t vknum=0;

    rstr=ucclient_strdup(prm->uccfgstr);
    (void)yang_modules_get_node_enums(NULL, rstr, aps, sizeof(aps));

    vknum=yang_db_runtime_getvknum(dbald, aps);

    if (NULL != rstr) { ucclient_free(rstr); rstr=NULL; }
    return vknum;
}

static int ucclient_cmp_container_vkval(ucclient_cfg_param_t *prm, xmlNodePtr container)
{
    uint8_t aps[16];
    char *rstr=NULL;
    char *vkstr=NULL;
    char *vstr=NULL;
    uint8_t vkey;
    uint8_t vtype;
    uint8_t vknum;
    uint8_t i,k;
    int ret=0;

    rstr=ucclient_strdup(prm->uccfgstr);
    (void)yang_modules_get_node_enums(NULL, rstr, aps, sizeof(aps));

    vknum=yang_db_runtime_getvknum(dbald, aps);
    for (i=0u; i<vknum; i++) {
        ret=yang_db_runtime_getvkstr(dbald, NULL, aps, i, &vkstr);
        if(0!=ret) {
            UB_LOG(UBL_ERROR, "%s:yang_db_runtime_getvkstr() failed\n", __func__);
            break;
        }
        ret=yang_db_runtime_getvkvtype(dbald, aps, i, &vkey, &vtype);
        if(0==ret) {
            k=prm->kvi+i;
            vstr=yang_value_string(vtype, prm->kvs[k], prm->kss[k], 0, prm->uccfgstr);
            if (NULL!=vstr && 0!=vstr[0]) {
                ret=ucclient_search_container_keyval(container, vkstr, vstr);
            }
        } else {
            UB_LOG(UBL_ERROR, "%s:yang_db_runtime_getvkvtype() failed\n", __func__);
        }
        UB_SD_RELMEM(YANGINIT_GEN_SMEM, vkstr); vkstr=NULL;
        if(0!=ret) { break; }
    }
    return ret;
}

static int ucclient_set_container_vkval(ucclient_cfg_param_t *prm, xmlNodePtr container)
{
    uint8_t aps[16];
    char *rstr=NULL;
    char *vkstr=NULL;
    char *vstr=NULL;
    uint8_t vkey;
    uint8_t vtype;
    uint8_t vknum;
    uint8_t i,k;
    int ret=0;

    rstr=ucclient_strdup(prm->uccfgstr);
    (void)yang_modules_get_node_enums(NULL, rstr, aps, sizeof(aps));

    vknum=yang_db_runtime_getvknum(dbald, aps);
    for (i=0u; i<vknum; i++) {
        ret=yang_db_runtime_getvkstr(dbald, NULL, aps, i, &vkstr);
        if(0!=ret) {
            UB_LOG(UBL_ERROR, "%s:yang_db_runtime_getvkstr() failed\n", __func__);
            break;
        }
        ret=yang_db_runtime_getvkvtype(dbald, aps, i, &vkey, &vtype);
        if(0==ret) {
            k=prm->kvi;
            vstr=yang_value_string(vtype, prm->kvs[k], prm->kss[k], 0, prm->uccfgstr);
            if (NULL!=vstr && 0!=vstr[0]) {
                char *nstr=strrchr(vkstr, '/');
                if (NULL != nstr) {
                    nstr=&nstr[1];
                    (void) xmlNewChild(container, NULL, BAD_CAST nstr, BAD_CAST vstr);
                    prm->kvi++;
                }
            }
        } else {
            UB_LOG(UBL_ERROR, "%s:yang_db_runtime_getvkvtype() failed\n", __func__);
        }
        UB_SD_RELMEM(YANGINIT_GEN_SMEM, vkstr); vkstr=NULL;
        if(0!=ret) { break; }
    }
    return ret;
}

static xmlNodePtr ucclient_get_existing_node(ucclient_cfg_param_t *prm)
{
    char xpathStr[256];
    xmlNodePtr ret_node=NULL;
    xmlNodePtr tmp_node=NULL;
    uint8_t vknum=0;
    int ret=0;
    int i;

    memset(xpathStr, 0, sizeof(xpathStr));
    sprintf(xpathStr, "/config%s", prm->uccfgnomod);

    if (NULL==prm->kvnode) {
        /* Evaluate xpath expression */
        xpathObj=xmlXPathEvalExpression(BAD_CAST xpathStr, xpathCtx);
        if(xpathObj==NULL) {
            UB_LOG(UBL_ERROR, "%s:path eval failed for xpath=[%s]\n", __func__, xpathStr);
            goto erexit;
        }

        for(i=0; i<xpathObj->nodesetval->nodeNr; i++) {
            tmp_node=xpathObj->nodesetval->nodeTab[ i ];
            vknum=ucclient_get_container_vknum(prm);
            if (vknum > 0) {
                ret=ucclient_cmp_container_vkval(prm, tmp_node);
                if (0==ret) {
                    ret_node=tmp_node;
                    prm->kvnode=ret_node;
                    prm->kvi+=vknum;
                    break;
                }
            } else {
                ret_node=tmp_node;
                break;
            }
        }
    } else {
        char *tmp=strrchr(xpathStr, '/')+1;
        for(tmp_node=prm->kvnode->children; tmp_node != NULL; tmp_node=tmp_node->next) {
            if (xmlStrcmp(tmp_node->name, BAD_CAST tmp)) {
                continue;
            }
            vknum=ucclient_get_container_vknum(prm);
            if (vknum > 0) {
                ret=ucclient_cmp_container_vkval(prm, tmp_node);
                if (0==ret) {
                    ret_node=tmp_node;
                    prm->kvnode=ret_node;
                    prm->kvi+=vknum;
                    break;
                }
            } else {
                ret_node=tmp_node;
                prm->kvnode=ret_node;
                break;
            }
        }
    }

erexit:
    return ret_node;
}

static xmlNodePtr ucclient_get_new_node(ucclient_cfg_param_t *prm, xmlChar *xpathSegment)
{
    xmlNodePtr ret_node=NULL;
    int ret=0;

    ret_node=xmlNewNode(NULL, BAD_CAST xpathSegment);
    if(ret_node==NULL) {
        UB_LOG(UBL_ERROR, "%s:creating new node=[%s] failed\n", __func__, xpathSegment);
        goto erexit;
    }

    ret=ucclient_set_container_vkval(prm, ret_node);
    if (0!=ret) {
        UB_LOG(UBL_ERROR, "%s:setting container vkval failed. node=[%s] failed\n",
               __func__, xpathSegment);
        goto erexit;
    }
    prm->kvnode=ret_node;

erexit:
    return ret_node;
}

static xmlNodePtr ucclient_get_main_container_node(ucclientopt_t *ucopt, char *container_name)
{
    xmlNodePtr root_node=NULL;
    xmlNodePtr child=NULL;

    root_node=xmlDocGetRootElement(ucopt->uccfgxml);
    for(child=root_node->children; child != NULL; child=child->next) {
        if (xmlStrcmp(child->name, BAD_CAST container_name)==0) {
            break;
        }
    }

    if (NULL==child) {
        child=xmlNewChild(root_node, NULL, BAD_CAST container_name, NULL);
    }

    /* Create xpath evaluation context */
    xpathCtx = xmlXPathNewContext(ucopt->uccfgxml);
    if(xpathCtx==NULL) {
        UB_LOG(UBL_ERROR, "%s:unable to create new XPath context\n", __func__);
        goto erexit;
    }

erexit:
    return child;
}

static int ucclient_set_node_contents(ucclient_cfg_param_t *prm, xmlNodePtr node)
{
    xmlNodePtr new_node=NULL;
    char *vstr=NULL;
    uint32_t vnum, i;
    int csize;
    int ret=0;

    csize=yang_sizeof_vtype(prm->vtype);
    if (-1 != csize) {
        vnum=prm->vsize/csize;
    } else {
        vnum=1;
    }

    for(i=0; i<vnum; i++) {
        vstr=yang_value_string(prm->vtype, prm->value, prm->vsize, i, prm->uccfgstr);
        if (NULL!=vstr && 0!=vstr[0]) {
            if(i > 0) {
                new_node=xmlCopyNode(node, 1);
                xmlNodeSetContent(new_node, BAD_CAST vstr);
                xmlAddSibling(node, new_node);
            } else {
                xmlNodeSetContent(node, BAD_CAST vstr);
            }
        } else {
            UB_LOG(UBL_ERROR, "%s:config=[%s] value string conversion failed\n",
                   __func__, prm->uccfgstr);
        }
    }
    return ret;
}

static int ucclient_insert_new_cfg_nodes(ucclientopt_t *ucopt, ucclient_cfg_param_t *prm)
{
    xmlNodePtr root_node=NULL;
    xmlNodePtr parent_node=NULL;
    xmlNodePtr node=NULL;
    xmlChar *xpathSegment=NULL;
    bool ignore_node=false;
    char *tmp=strchr(&prm->uccfgnomod[1], '/');
    char *saveptr=NULL;
    char *prevptr=NULL;
    int ret=0;

    tmp[0]=0;
    root_node=ucclient_get_main_container_node(ucopt, &(prm->uccfgnomod[1]));
    tmp[0]='/';

    // create nodes for each segment of the XPath expression
    xpathSegment = BAD_CAST ucclient_tokenize_xpath(tmp, '/', &saveptr);
    while (xpathSegment != NULL) {
        if(NULL != prevptr) { prevptr[0]='/'; }
        node=ucclient_get_existing_node(prm);
        if (NULL==node) {
            if(xmlStrcmp(xpathSegment, BAD_CAST IGNORE_LEAF_KEYWOARD)) {
                node=ucclient_get_new_node(prm, xpathSegment);
                if (NULL==node) {
                    UB_LOG(UBL_ERROR, "%s:getting new node=[%s] failed\n",
                           __func__, xpathSegment);
                    break;
                }
                /* add the new node to the parent node and proceed */
                if (parent_node != NULL) {
                    xmlAddChild(parent_node, node);
                } else {
                    xmlAddChild(root_node, node);
                }
                ignore_node=false;
            } else {
                ignore_node=true;
            }
        }

        parent_node=node;
        prevptr=saveptr;
        xpathSegment = BAD_CAST ucclient_tokenize_xpath(NULL, '/', &saveptr);
        if (xpathObj) {
            xmlXPathFreeObject(xpathObj);
            xpathObj=NULL;
        }
    }

    if (!ignore_node) {
        ret=ucclient_set_node_contents(prm, node);
        if (0 != ret) {
            UB_LOG(UBL_ERROR, "%s:Setting node content failed [cfg=%s]\n",
                   __func__, prm->uccfgstr);
        }
    }

    if (xpathObj) {
        xmlXPathFreeObject(xpathObj);
        xpathObj=NULL;
    }
    if (xpathCtx) {
        xmlXPathFreeContext(xpathCtx);
        xpathCtx=NULL;
    }
    return ret;
}

static int ucclient_insert_config(ucclientopt_t *ucopt, ucclient_cfg_param_t *prm)
{
    int ret=0;

    prm->uccfgstr=ucclient_strdup(prm->rstr);
    prm->uccfgnomod=strchr(&(prm->uccfgstr[1]), '/');
    ret=ucclient_insert_new_cfg_nodes(ucopt, prm);

    if (NULL != prm->uccfgstr) {
        ucclient_free(prm->uccfgstr);
        prm->uccfgstr=NULL;
    }
    return ret;
}

static int ucclient_xmldoc_init(ucclientopt_t *ucopt)
{
    xmlNodePtr config;

    config = xmlNewNode(NULL, BAD_CAST "config");
    if (NULL == config) {
        UB_LOG(UBL_ERROR, "%s:allocating new node failed\n", __func__);
        goto erexit;
    }

    ucopt->uccfgxml = xmlNewDoc(BAD_CAST "1.0");
    if (NULL == ucopt->uccfgxml) {
        UB_LOG(UBL_ERROR, "%s:allocating new doc failed\n", __func__);
        goto erexit;
    }
    xmlDocSetRootElement(ucopt->uccfgxml, config);

    return 0;
erexit:
    if (config) { xmlFreeNode(config); }
    if (ucopt->uccfgxml) { xmlFreeDoc(ucopt->uccfgxml); ucopt->uccfgxml=NULL; }
    return -1;
}

int ucclient_init(ucclientopt_t *ucopt)
{
    unibase_init_para_t init_para;
    int ret=0;

    /* initialized excelfore's unibase library */
    ubb_default_initpara(&init_para);
    init_para.ub_log_initstr=UBL_OVERRIDE_ISTR("4,ubase:45,cbase:45,uconf:66,ucclt:66", "UBL_UCCLIENT");
    unibase_init(&init_para);

    /* open uniconf db */
    dbald = uc_dbal_open(ucopt->dbname, "w", ucopt->ucmode);
    if(!dbald) {
        UB_LOG(UBL_ERROR, "%s:uc_dbal_open() failed\n", __func__);
        goto erexit;
    }

    ydrd=yang_db_runtime_init(NULL, dbald, NULL);
    if(!ydrd){
        UB_LOG(UBL_ERROR, "%s:yang_db_runtime_init() failed\n", __func__);
        goto erexit;
    }

    ucntd=uc_notice_init(ucopt->ucmode);
    if(!ucntd) {
        UB_LOG(UBL_ERROR, "%s:uc_notice_init() failed\n", __func__);
        goto erexit;
    }

    ret=ucclient_xmldoc_init(ucopt);
    if (0 != ret) {
        UB_LOG(UBL_ERROR, "%s:ucclient_initconfig() failed\n", __func__);
        goto erexit;
    }

    return 0;
erexit:
    ucclient_cleanup(ucopt);
    return -1;
}

int ucclient_get_initial_config(ucclientopt_t *ucopt)
{
    uint8_t key1[1]={XL4_EXTMOD_RW};
    uint8_t key2[1]={XL4_DATA_RO-1};
    ucclient_cfg_param_t prm;
    uc_range *range=NULL;
    void *key=NULL;
    uint32_t ksize;
    int ret=0;

    range=uc_get_range(dbald, key1, 1, key2, 1);
    while(true){
        memset(&prm, 0, sizeof(prm));
        if(uc_get_key_in_range(dbald, range, &key, &ksize, UC_DBAL_NOMOVE)){
            break;
        }
        if(uc_get_value_in_range(dbald, range, &prm.value, &prm.vsize, UC_DBAL_FORWARD)){
            break;
        }
        if(yang_modules_get_node_string(NULL, &prm.rstr, key)){
            continue;
        }
        if(yang_db_extract_key(key, ksize, &prm.aps, prm.kvs, prm.kss)!=0){
            UB_SD_RELMEM(YANGINIT_GEN_SMEM, prm.rstr);
            continue;
        }
        prm.vtype=yang_db_runtime_get_vtype(dbald, key);
        UB_LOG(UBL_INFO, "%s:prm->rstr: [%s]\n", __func__, prm.rstr);
        ret=ucclient_insert_config(ucopt, &prm);
        if (0 != ret) {
            UB_LOG(UBL_ERROR, "%s:Failed to insert [rstr=%s]\n", __func__, prm.rstr);
            UB_SD_RELMEM(YANGINIT_GEN_SMEM, prm.rstr);
            break;
        }
        yang_db_extract_key_free(prm.aps, prm.kvs, prm.kss);
        UB_SD_RELMEM(YANGINIT_GEN_SMEM, prm.rstr);
    }
    uc_get_range_release(dbald, range);
    uc_dbal_releasedb(dbald);
    xmlDocFormatDump(stdout, ucopt->uccfgxml, 1);
    return ret;
}

int ucclient_db_action(yang_db_access_para_t *dbpara)
{
    int ret=0;

    ret=yang_db_action(dbald, NULL, dbpara);
    if (0 != ret) {
        UB_LOG(UBL_ERROR, "%s:Failed [ret=%d]\n", __func__, ret);
    }

    return ret;
}

void ucclient_db_release(void)
{
    uc_dbal_releasedb(dbald);
}

int ucclient_db_set_notify_aps(uint8_t *aps, void **kvs, uint8_t *kss)
{
    char *rstr=NULL;
    uint8_t i=0,j=0;
    int ret=-1;
    if (!notify_aps_set) {
        notify_aps_set=true;
        memset(notify_aps, 255, sizeof(notify_aps));
        memset(notify_kvs, 0, sizeof(notify_kvs));
        memset(notify_kss, 0, sizeof(notify_kss));
        while(i < MAX_APS_NUM && aps[i]!=255) {
            notify_aps[i]=aps[i];
            i++;
        }
        while(kvs!=NULL && j < MAX_KVS_NUM && kvs[j]!=NULL) {
            notify_kvs[j]=kvs[j];
            notify_kss[j]=kss[j];
            j++;
        }
        if(yang_modules_get_node_string(NULL, &rstr, notify_aps)){
            UB_LOG(UBL_WARN, "%s:failed to get notify_aps node string\n", __func__);
        } else {
            UB_LOG(UBL_INFO, "%s: aps for notification set cfg=[%s]\n", __func__, rstr);
        }
    } else {
        UB_LOG(UBL_ERROR, "%s:only one notify aps can be set at a time\n", __func__);
    }
    return ret;
}

void ucclient_db_notify(void)
{
    char *rstr=NULL;
    uint8_t i=0;
    if (notify_aps_set) {
        if(yang_modules_get_node_string(NULL, &rstr, notify_aps)){
            UB_LOG(UBL_ERROR, "%s:failed to get notify_aps node string\n", __func__);
            return;
        }
        UB_LOG(UBL_INFO, "%s: pushing notification for cfg=[%s] update\n", __func__, rstr);
        uc_nc_askaction_push(ucntd, dbald, notify_aps, notify_kvs, notify_kss);
        UB_SD_RELMEM(YANGINIT_GEN_SMEM, rstr);
        while (notify_kvs[i] != NULL) {
            UB_SD_RELMEM(YANGINIT_GEN_SMEM, notify_kvs[i]);
            i++;
        }
        notify_aps_set=false;
    } else {
        UB_LOG(UBL_WARN, "%s:no aps set, ignoring notify request\n", __func__);
    }
}

void ucclient_cleanup(ucclientopt_t *ucopt)
{
    if(ydrd) { yang_db_runtime_close(ydrd); }
    if (dbald){ uc_dbal_close(dbald, ucopt->ucmode); }
}

