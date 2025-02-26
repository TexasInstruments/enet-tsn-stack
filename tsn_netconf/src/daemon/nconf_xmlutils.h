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
 * @file        nconf_xmlutils.h
 *
 * @brief       Netconf Common XML Utility Interface Header
 */
#ifndef __NCONF_XMLUTILS_H__
#define __NCONF_XMLUTILS_H__

/*=============================================================================
 * Include Files
 *============================================================================*/

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*=============================================================================
 * Macros and Constants
 *============================================================================*/

#define NCONF_MAX_XML_NODE_NUM           (10U)
#define XMLUTILS_NODE_GET_NAME(_node)    nconf_xmlutils_get_node_name(_node)
#define XMLUTILS_NODE_GET_CHILD(_node)   nconf_xmlutils_get_node_child(_node)
#define XMLUTILS_NODE_GET_SIBLING(_node) nconf_xmlutils_get_node_sibling(_node)
#define XMLUTILS_NODE_RELEASE_ALL_INST() nconf_xmlutils_release_all_nodeinst()

/*=============================================================================
 * Typedefs, Structures, and Enums
 *============================================================================*/

typedef struct _nconf_docptr * nconf_docptr_t;
typedef struct _nconf_nodeptr * nconf_nodeptr_t;
typedef bool (*nconf_keychecker_t)(const char *nodepath, uint8_t *kvidx);

typedef enum {
    NCONF_FILTER_TYPE_SUBTREE=0x00U,
    NCONF_FILTER_TYPE_XPATH,
    NCONF_FILTER_TYPE_MAX
} nconf_filter_type_t;

typedef enum {
    NCONF_TREEINFO_TYPE_FILTER=0x00U,
    NCONF_TREEINFO_TYPE_CONFIG,
    NCONF_TREEINFO_TYPE_MAX
} nconf_treeinfo_type_t;

typedef struct _subtree_nodes {
    char **nodepaths;
    char **nodevalues;
    uint8_t count;
} nconf_treeinfo_t;

typedef struct _filter {
    nconf_filter_type_t ftype;
    nconf_treeinfo_t **subtrees;
    uint8_t count;
} nconf_filter_info_t;

typedef struct _config {
    nconf_treeinfo_t **configs;
    uint8_t count;
} nconf_update_info_t;

typedef struct _kval {
    char *key;
    char *val;
} nconf_kval_t;

typedef struct _nodelist {
    nconf_nodeptr_t *node;
    uint32_t count;
} nconf_nodelist_t;

typedef struct _nodeinfo {
    char *name;
    char *value;
} nconf_nodeinfo_t;

typedef struct {
    nconf_docptr_t xmldoc;
    nconf_nodelist_t foundnodes;
    nconf_nodeinfo_t nodes[NCONF_MAX_XML_NODE_NUM];
    uint8_t nodesnum;
} nconf_xpath_args_t;

/*=============================================================================
 * XML Utility APIs
 *============================================================================*/

nconf_docptr_t nconf_xmlutils_new_doc(const char *version);
nconf_docptr_t nconf_xmlutils_new_doc_w_root(const char *version, char *name);
nconf_docptr_t nconf_xmlutils_parse_xml(uint8_t *xml, uint32_t xmllen);
nconf_docptr_t nconf_xmlutils_parse_xml_file(char *file);
nconf_nodeptr_t nconf_xmlutils_get_root_node(nconf_docptr_t doc);
nconf_nodeptr_t nconf_xmlutils_set_root_node(nconf_docptr_t doc, nconf_nodeptr_t root);
nconf_nodeptr_t nconf_xmlutils_new_node(nconf_nodeptr_t parent, char *name, char *value);
nconf_nodeptr_t nconf_xmlutils_get_node_child(nconf_nodeptr_t node);
nconf_nodeptr_t nconf_xmlutils_get_node_child_by_name(nconf_nodeptr_t parent, char *nodename);
nconf_nodeptr_t nconf_xmlutils_get_node_sibling(nconf_nodeptr_t node);
nconf_nodeptr_t nconf_xmlutils_get_node_parent(nconf_nodeptr_t node);
nconf_nodeptr_t nconf_xmlutils_get_node_top(nconf_nodeptr_t node);
nconf_nodeptr_t nconf_xmlutils_add_child_node(nconf_nodeptr_t parent, nconf_nodeptr_t child);
nconf_nodeptr_t nconf_xmlutils_copy_node(nconf_nodeptr_t node, int extended);
nconf_nodeptr_t nconf_xmlutils_copy_node_w_parent(nconf_nodeptr_t node, int extended);
nconf_nodeptr_t nconf_xmlutils_add_sibling_node(nconf_nodeptr_t curnode, nconf_nodeptr_t newnode);
nconf_nodeptr_t nconf_xmlutils_get_module_node(nconf_nodeptr_t node);
nconf_nodeptr_t nconf_xmlutils_get_modnode_w_modname(char *modname, nconf_nodeptr_t node);
char *nconf_xmlutils_get_node_name(nconf_nodeptr_t node);
char* nconf_xmlutils_get_node_content(nconf_nodeptr_t node);
char* nconf_xmlutils_get_node_prop(nconf_nodeptr_t node, char *propname);
char* nconf_xmlutils_get_ns_node_prop(nconf_nodeptr_t node, char *propname, char *nspace);
char* nconf_xmlutils_get_node_namespace_uri(nconf_nodeptr_t node);
char* nconf_xmlutils_get_node_path(nconf_nodeptr_t node, uint8_t level);
char* nconf_xmlutils_get_node_header(nconf_nodeptr_t nodeptr);
char* nconf_xmlutils_get_module_node_path(char *modname, nconf_nodeptr_t nodeptr, uint8_t level);
char* nconf_xmlutils_sanitize_value_string(char *valstr, int *isUpdated);
char* nconf_xmlutils_remove_keyinfo(char *nodepath);
bool nconf_xmlutils_node_has_children(nconf_nodeptr_t node);
void nconf_xmlutils_lib_init(void);
void nconf_xmlutils_set_node_prop(nconf_nodeptr_t node, char *name, char*value);
void nconf_xmlutils_copy_node_prop(nconf_nodeptr_t dst, nconf_nodeptr_t src);
void nconf_xmlutils_new_node_namespace(nconf_nodeptr_t node, char *nspace, char *prefix);
void nconf_xmlutils_copy_node_nsdefs(nconf_nodeptr_t dst, nconf_nodeptr_t src);
void nconf_xmlutils_set_node_nsasc(nconf_nodeptr_t node);
void nconf_xmlutils_set_node_content(nconf_nodeptr_t node, char *content);
void nconf_xmlutils_delete_child_node(nconf_nodeptr_t parent, nconf_nodeptr_t child);
void nconf_xmlutils_delete_all_child_nodes(nconf_nodeptr_t nodeptr);
void nconf_xmlutils_delete_child_nodes_of_childs(nconf_nodeptr_t nodeptr);
void nconf_xmlutils_free_node_path(char *nodepath);
void nconf_xmlutils_free_node(nconf_nodeptr_t node);
void nconf_xmlutils_release_node(nconf_nodeptr_t node);
void nconf_xmlutils_release_all_nodeinst(void);
void nconf_xmlutils_free_doc(nconf_docptr_t doc);
void nconf_xmlutils_free(uint8_t *buf);
void nconf_xmlutils_free_encode_buf(uint8_t *buf);
void nconf_xmlutils_free_nptr_encode_buf(uint8_t *buf);
void nconf_xmlutils_release_sanitize_value(char *valstr);
void nconf_xmlutils_dump_docptr(nconf_docptr_t doc);
void nconf_xmlutils_get_docptr_str(nconf_docptr_t doc, char **xmlstr, uint32_t *xmllen);
void nconf_xmlutils_get_nodeptr_str(nconf_nodeptr_t nptr, char **xmlstr, uint32_t *xmllen);
void nconf_xmlutils_dump_nodeptr(nconf_nodeptr_t nodeptr);
void nconf_xmlutils_xpath_release(nconf_xpath_args_t *param);
void nconf_xmlutils_release_search_handle(nconf_xpath_args_t *param);
void nconf_xmlutils_cleanup(void);
void nconf_xmlutils_release_tree_info(nconf_treeinfo_t *treeinfo);
nconf_treeinfo_t* nconf_xmlutils_generate_tree_info(nconf_nodeptr_t nodeptr,
                                                    nconf_keychecker_t iskeycb,
                                                    nconf_treeinfo_type_t type);
nconf_xpath_args_t* nconf_xmlutils_get_search_handle(void);
int nconf_xmlutils_xpath_search(nconf_xpath_args_t *param);

#ifdef __cplusplus
}
#endif

#endif /* __NCONF_XMLUTILS_H__ */
