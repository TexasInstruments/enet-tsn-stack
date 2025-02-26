/**
 * @file        nconf_xmlutils.c
 *
 * @brief       Netconf Common XML Utility Implementations using
 *              PugiXML
 */

/*=============================================================================
 * Include Files
 *============================================================================*/

#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <stack>
#include <vector>
#include <algorithm>
#include <map>
#include <pugixml.hpp>
#include "nconf_types.h"
#include "nconf_xmlutils.h"
#include "nconf_memutils.h"
#include "nconf_msgdef.h"

/*=============================================================================
 * Macros and Constants
 *============================================================================*/

#define NCONF_MAX_NODEPTR_NUM           (20U)

/*=============================================================================
 * Typedefs, Structures, and Enums
 *============================================================================*/

typedef std::map<std::string, std::string> nconf_ns_t;

struct config_strings {
    std::string path;
    std::string value;
    config_strings() : path(), value() {}
    config_strings(std::string p, std::string v) : path(p), value(v) {}
};
using NodeStack = std::stack<std::pair<pugi::xml_node, std::string>>;
using FilterVector = std::vector<std::string>;
using ConfigVector = std::vector<struct config_strings>;

struct _nconf_docptr {
    pugi::xml_document inst;
    pugi::xml_node rnode;

    _nconf_docptr() : inst(), rnode() {}
    ~_nconf_docptr() {}

    static void* operator new(size_t size, void* ptr) { (void)size; return ptr; }
    static void* operator new(size_t size) { return nconf_memalloc(size); }
    static void operator delete(void* ptr) { return nconf_memfree(ptr); }
};

struct _nconf_nodeptr {
    pugi::xml_node node;
    pugi::xml_document *refdoc;
    nconf_ns_t nsassoc;
    void *refdocmem;

    _nconf_nodeptr()
        : node(), refdoc(nullptr), nsassoc(), refdocmem(NULL) {}
    ~_nconf_nodeptr() {}

    static void* operator new(size_t size, void* ptr) { (void)size; return ptr; }
    static void* operator new(size_t size) { return nconf_memalloc(size); }
    static void operator delete(void* ptr) { return nconf_memfree(ptr); }
};

typedef struct _nodeptr_stack {
    pugi::xml_node node;
    struct _nodeptr_stack *next;

    _nodeptr_stack() : node(), next(NULL) {}
    ~_nodeptr_stack() {}

    static void* operator new(size_t size) { return nconf_memalloc(size); }
    static void operator delete(void* ptr) { return nconf_memfree(ptr); }
} nconf_nodeptr_stack_t;

struct node_compare {
    pugi::xml_node& refnode;
    bool operator()(pugi::xml_node node) const
    {
        bool bret=false;
        if (std::strcmp(node.name(), refnode.name())==0) {
            if (std::strcmp(node.text().get(), refnode.text().get())==0) {
                bret=true;
            }
        }
        return bret;
    }
    node_compare(pugi::xml_node& r) : refnode(r) {}
    ~node_compare() {}
    static void* operator new(size_t size) { return nconf_memalloc(size); }
    static void operator delete(void* ptr) { return nconf_memfree(ptr); }
};

class XmlUtils {
    public:
        static XmlUtils* instance();
        static void cleanup();
        std::string& get_docstr(void) { return docstr; }
        std::string& get_nodestr(void) { return nodestr; }
        nconf_nodeptr_t new_nodeinst(pugi::xml_document *refdoc=NULL);
        void release_nodeinst(nconf_nodeptr_t nodeinst);
        void release_all_nodeinst(void);
        static void* operator new(size_t size) { return nconf_memalloc(size); }
        static void operator delete(void* ptr) { return nconf_memfree(ptr); }
    private:
        XmlUtils() : nptrnum(0), docstr(), nodestr() {
            memset(nptr, 0, sizeof(nptr));
        }
        XmlUtils(XmlUtils const&) = delete;
        XmlUtils& operator=(XmlUtils const&) = delete;

        /* Node pointer list for processing */
        nconf_nodeptr_t nptr[NCONF_MAX_NODEPTR_NUM];
        uint8_t nptrnum;

        /* temporary document string */
        std::string docstr;
        std::string nodestr;

        /* singleton instance */
        static XmlUtils* s_util_inst;
};

/*=============================================================================
 * Function Prototypes
 *============================================================================*/

static void nconf_xmlutils_nptr_stack_push(nconf_nodeptr_stack_t **stack,
                                           pugi::xml_node node);
static pugi::xml_node nconf_xmlutils_nptr_stack_pop(nconf_nodeptr_stack_t **stack);
static char* nconf_xmlutils_create_xpath_expr(nconf_xpath_args_t *args);
static char* nconf_xmlutils_get_xpath_buf(nconf_xpath_args_t *args, uint32_t *len);
static uint32_t nconf_xmlutils_get_xpath_buf_size(nconf_xpath_args_t *args);
static uint32_t nconf_xmlutils_get_nodename_len(nconf_xpath_args_t *args, uint8_t index);
static bool nconf_xmlutils_has_predicate(nconf_xpath_args_t *args, uint8_t index, uint32_t *plen);
static uint32_t nconf_xmlutils_encode_xpath_expr(nconf_xpath_args_t *args,
                                                 char *buf, uint32_t bufsz);
static uint32_t nconf_xmlutils_create_xpath_base(nconf_xpath_args_t *args, uint8_t index,
                                                 char *buf, uint32_t bufsz);
static uint32_t nconf_xmlutils_add_xpath_predicate(nconf_xpath_args_t *args, uint8_t index,
                                                   char *buf, uint32_t bufsz);
static int nconf_xmlutils_set_found_nodes(nconf_xpath_args_t *param,
                                          pugi::xpath_node_set search_result);
static char* nconf_xmlutils_get_module_name(pugi::xml_node& node);
static int nconf_xmlutils_count_ampersands(const char *valstr);
static char* nconf_xmlutils_replaced_amp(char *instr, size_t inlen, size_t outlen);
static void* nconf_xmlutils_alloc(size_t size);
static void nconf_xmlutils_free(void *ptr);
template <typename VectorType>
static VectorType nconf_xmlutils_flatten_xmltree(nconf_nodeptr_t xmltree, char *modname,
                                                 nconf_keychecker_t iskeycb);
static void nconf_xmlutils_process_key_leafs(pugi::xml_node current_node, std::string& current_path,
                                             FilterVector& filters, NodeStack& node_stack);
static void nconf_xmlutils_process_key_leafs(pugi::xml_node current_node, std::string& current_path,
                                             ConfigVector& configs, NodeStack& node_stack);
static void nconf_xmlutils_process_normal_leafs(pugi::xml_node current_node,
                                                std::string& current_path,
                                                FilterVector& filters);
static void nconf_xmlutils_process_normal_leafs(pugi::xml_node current_node,
                                                std::string& current_path,
                                                ConfigVector& configs);
static void nconf_xmlutils_sort_filters(FilterVector& paths, nconf_keychecker_t iskeycb);
static void nconf_xmlutils_sort_configs(ConfigVector& cfginfos, nconf_keychecker_t iskeycb);
static void nconf_xmlutils_sort_path_key_info(std::string& path, nconf_keychecker_t iskeycb);
static bool nconf_xmlutils_sort_key_info(std::string& contpath, std::string& key_info,
                                         nconf_keychecker_t iskeycb);
static nconf_treeinfo_t* nconf_xmlutils_get_filter_treeinfo(FilterVector paths);
static nconf_treeinfo_t* nconf_xmlutils_get_config_treeinfo(ConfigVector cfginfo);
static bool nconf_xmlustils_cfg_path_comparator(struct config_strings& lhs,
                                                struct config_strings& rhs);
static std::string nconf_xmlutils_remove_keyinfo(const std::string& nodepath);
static void nconf_xmlutils_update_keyinfo(std::string& current_path,
                                          pugi::xml_node& current_node,
                                          FilterVector& paths);
static void nconf_xmlutils_update_keyinfo(std::string& current_path,
                                          pugi::xml_node& current_node,
                                          ConfigVector& cfginfos);
static void nconf_xmlutils_add_tree_info(FilterVector& paths, std::string npath);
static void nconf_xmlutils_add_tree_info(ConfigVector& cfginfo, std::string npath,
                                         std::string nval, std::string operation);
static void nconf_xmlutils_update_node_stack(NodeStack& node_stack,
                                             std::string& previous_path,
                                             std::string& updated_path);
static char* nconf_xmlutils_get_edit_operation(pugi::xml_node& leafnode);

/*=============================================================================
 * Global Variables
 *============================================================================*/

UB_SD_GETMEM_DEF(NCONF_XMLUTILS_SEARCH_ARGS, sizeof(nconf_xpath_args_t), 1U);

/*=============================================================================
 * Function Definitions
 *============================================================================*/

nconf_docptr_t nconf_xmlutils_new_doc(const char *version)
{
    nconf_docptr_t docptr = NULL;
    if (void* ptr = nconf_memalloc(sizeof(struct _nconf_docptr))) {
        docptr = new (ptr) _nconf_docptr();
        pugi::xml_node decl=docptr->inst.prepend_child(pugi::node_declaration);
        if (NULL != version) {
            decl.append_attribute("version")=version;
        } else {
            decl.append_attribute("version")="1.0";
        }
        decl.append_attribute("encoding")="UTF-8";
    } else {
        UB_LOG(UBL_ERROR, "%s:failed to create new doc ptr\n", __func__);
    }
    return docptr;
}

nconf_docptr_t nconf_xmlutils_new_doc_w_root(const char *version, char *name)
{
    nconf_docptr_t docptr = NULL;
    if (NULL != name) {
        docptr=nconf_xmlutils_new_doc(version);
        if (NULL!=docptr) {
            docptr->rnode=docptr->inst.append_child(name);
        }
    }
    return docptr;
}

nconf_docptr_t nconf_xmlutils_parse_xml(uint8_t *xml, uint32_t xmllen)
{
    nconf_docptr_t docptr = NULL;
    if (NULL != xml && 0 != xmllen) {
        if (void* ptr = nconf_memalloc(sizeof(struct _nconf_docptr))) {
            docptr = new (ptr) _nconf_docptr();
            pugi::xml_parse_result res=docptr->inst.load_buffer(xml, xmllen);
            if (res) {
                docptr->rnode=docptr->inst.first_child();
            } else {
                UB_LOG(UBL_ERROR, "%s:failed to load buffer\n", __func__);
                delete docptr;
                docptr=NULL;
            }
        } else {
            UB_LOG(UBL_ERROR, "%s:failed to create new doc ptr\n", __func__);
        }
    }
    return docptr;
}

nconf_docptr_t nconf_xmlutils_parse_xml_file(char *file)
{
    nconf_docptr_t docptr = NULL;
    if (NULL != file) {
        if (void* ptr = nconf_memalloc(sizeof(struct _nconf_docptr))) {
            docptr = new (ptr) _nconf_docptr();
            pugi::xml_parse_result res=docptr->inst.load_file(file);
            if (res) {
                docptr->rnode=docptr->inst.first_child();
            } else {
                UB_LOG(UBL_ERROR, "%s:failed to load xml file=[%s]\n",
                       __func__, file);
                delete docptr;
                docptr=NULL;
            }
        } else {
            UB_LOG(UBL_ERROR, "%s:failed to create new doc ptr\n", __func__);
        }
    }
    return docptr;
}

nconf_nodeptr_t nconf_xmlutils_get_root_node(nconf_docptr_t doc)
{
    nconf_nodeptr_t nodeptr=NULL;
    if (NULL != doc) {
        if (!doc->rnode.empty()) {
            nodeptr=XmlUtils::instance()->new_nodeinst(&doc->inst);
            if (NULL != nodeptr) {
                nodeptr->node=doc->rnode;
            }
        }
    }
    return nodeptr;
}

nconf_nodeptr_t nconf_xmlutils_set_root_node(nconf_docptr_t doc, nconf_nodeptr_t root)
{
    if (NULL != doc && NULL != root) {
        doc->rnode=doc->inst.append_copy(root->node);
    }
    return NULL;
}

nconf_nodeptr_t nconf_xmlutils_new_node(nconf_nodeptr_t parent, char *name, char *value)
{
    nconf_nodeptr_t nodeptr=NULL;
    if (NULL != name) {
        pugi::xml_document *refdoc=(NULL!=parent)?parent->refdoc:NULL;
        nodeptr=XmlUtils::instance()->new_nodeinst(refdoc);
        if (NULL != nodeptr) {
            if (NULL==parent) {
                nodeptr->node=nodeptr->refdoc->append_child(name);
            } else {
                nodeptr->node=parent->node.append_child(name);
            }
            if (NULL != nodeptr->node) {
                if (value != NULL) {
                    nodeptr->node.text().set(value);
                }
            } else {
                UB_LOG(UBL_ERROR, "%s: failed to append child=[%s]\n",
                       __func__, name);
                XmlUtils::instance()->release_nodeinst(nodeptr);
                nodeptr=NULL;
            }
        } else {
            UB_LOG(UBL_ERROR, "%s:failed to create new node instance=[%s]\n",
                   __func__, name);
        }
    }
    return nodeptr;
}

nconf_nodeptr_t nconf_xmlutils_get_node_child(nconf_nodeptr_t nodeptr)
{
    nconf_nodeptr_t retnodeptr=NULL;
    pugi::xml_node childnode=nodeptr->node.first_child();
    if (NULL != childnode && pugi::node_element==childnode.type()) {
        retnodeptr=XmlUtils::instance()->new_nodeinst(nodeptr->refdoc);
        if (NULL != retnodeptr) {
            retnodeptr->node=childnode;
        }
    }
    return retnodeptr;
}

nconf_nodeptr_t nconf_xmlutils_get_node_child_by_name(nconf_nodeptr_t parent, char *nodename)
{
    nconf_nodeptr_t retnodeptr=NULL;
    pugi::xml_node parentnode=parent->node;
    pugi::xml_node childnode=parentnode.child(nodename);
    if (NULL != childnode && pugi::node_element==childnode.type()) {
        retnodeptr=XmlUtils::instance()->new_nodeinst(parent->refdoc);
        if (NULL != retnodeptr) {
            retnodeptr->node=childnode;
        }
    }
    return retnodeptr;
}

nconf_nodeptr_t nconf_xmlutils_get_node_sibling(nconf_nodeptr_t nodeptr)
{
    nconf_nodeptr_t retnodeptr=NULL;
    if (NULL != nodeptr->node.next_sibling()) {
        retnodeptr=XmlUtils::instance()->new_nodeinst(nodeptr->refdoc);
        if (NULL != retnodeptr) {
            retnodeptr->node=nodeptr->node.next_sibling();
        }
    }
    return retnodeptr;
}

nconf_nodeptr_t nconf_xmlutils_get_node_parent(nconf_nodeptr_t nodeptr)
{
    nconf_nodeptr_t retnodeptr=NULL;
    pugi::xml_node parent=nodeptr->node.parent();
    if (NULL != parent && pugi::node_document!=parent.type()) {
        retnodeptr=XmlUtils::instance()->new_nodeinst(nodeptr->refdoc);
        if (NULL != retnodeptr) {
            retnodeptr->node=parent;
        }
    }
    return retnodeptr;
}

nconf_nodeptr_t nconf_xmlutils_get_node_top(nconf_nodeptr_t nodeptr)
{
    nconf_nodeptr_t retnodeptr=nodeptr;
    pugi::xml_node curnode=retnodeptr->node;
    if (NULL != curnode.parent() && pugi::node_document!=curnode.parent().type()) {
        retnodeptr=XmlUtils::instance()->new_nodeinst(nodeptr->refdoc);
        if (NULL != retnodeptr) {
            pugi::xml_node parent=curnode.parent();
            while(NULL != parent.parent() && pugi::node_document!=parent.parent().type()) {
                parent=parent.parent();
            }
            retnodeptr->node=parent;
        }
    }
    return retnodeptr;
}

nconf_nodeptr_t nconf_xmlutils_add_child_node(nconf_nodeptr_t parent, nconf_nodeptr_t child)
{
    nconf_nodeptr_t retnodeptr=XmlUtils::instance()->new_nodeinst(parent->refdoc);
    if (NULL != retnodeptr) {
        retnodeptr->node=parent->node.append_copy(child->node);
    }
    return retnodeptr;
}

nconf_nodeptr_t nconf_xmlutils_copy_node(nconf_nodeptr_t nodeptr, int extended)
{
    nconf_nodeptr_t retnodeptr=XmlUtils::instance()->new_nodeinst();
    if (NULL != retnodeptr) {
        retnodeptr->node=retnodeptr->refdoc->append_copy(nodeptr->node);
        NCONF_UNUSED(extended);
    }
    return retnodeptr;
}

nconf_nodeptr_t nconf_xmlutils_copy_node_w_parent(nconf_nodeptr_t nodeptr, int extended)
{
    nconf_nodeptr_t retnodeptr=XmlUtils::instance()->new_nodeinst();
    nconf_nodeptr_stack_t *stack=NULL;
    pugi::xml_node cur=nodeptr->node;
    if (NULL==retnodeptr) { return NULL; }
    NCONF_UNUSED(extended);
    while (cur) {
        nconf_xmlutils_nptr_stack_push(&stack, cur);
        cur=cur.parent();
    }
    while (stack != NULL) {
        cur=nconf_xmlutils_nptr_stack_pop(&stack);
        if (retnodeptr->node.find_child(node_compare(cur))==NULL) {
            retnodeptr->node=retnodeptr->refdoc->append_copy(cur);
        } else {
            retnodeptr->node=retnodeptr->node.find_child(node_compare(cur));
        }
    }
    while (stack != NULL) {
        (void)nconf_xmlutils_nptr_stack_pop(&stack);
    }
    return retnodeptr;
}

nconf_nodeptr_t nconf_xmlutils_add_sibling_node(nconf_nodeptr_t curnode, nconf_nodeptr_t newnode)
{
    nconf_nodeptr_t retnode=NULL;
    pugi::xml_node parent=curnode->node.parent();
    if (parent && !parent.empty()) {
        retnode=XmlUtils::instance()->new_nodeinst(curnode->refdoc);
        if (NULL != retnode) {
            retnode->node=parent.insert_copy_after(newnode->node, curnode->node);
        }
    }
    return retnode;
}

nconf_nodeptr_t nconf_xmlutils_get_module_node(nconf_nodeptr_t node)
{
    nconf_nodeptr_t modulenode=NULL;
    if (NULL != node) {
        pugi::xml_node top=node->node.root().first_child();
        char *modulename=nconf_xmlutils_get_module_name(top);
        if (NULL != modulename) {
            modulenode=XmlUtils::instance()->new_nodeinst(node->refdoc);
            if (NULL != modulenode) {
                modulenode->node=node->node.root().prepend_child(modulename);
                node->node.root().first_child().append_move(top);
            } else {
                UB_LOG(UBL_ERROR, "%s: failed create new node instance\n", __func__);
            }
        }
    }
    return modulenode;
}

nconf_nodeptr_t nconf_xmlutils_get_modnode_w_modname(char *modname, nconf_nodeptr_t node)
{
    nconf_nodeptr_t modulenode=XmlUtils::instance()->new_nodeinst(node->refdoc);
    if (NULL != modulenode) {
        pugi::xml_node top=node->node.root().first_child();
        modulenode->node=node->node.root().prepend_child(modname);
        node->node.root().first_child().append_move(top);
    } else {
        UB_LOG(UBL_ERROR, "%s: failed create new node instance\n", __func__);
    }
    return modulenode;
}

char *nconf_xmlutils_get_node_name(nconf_nodeptr_t nodeptr)
{
    return const_cast<char*>(nodeptr->node.name());
}

char* nconf_xmlutils_get_node_content(nconf_nodeptr_t nodeptr)
{
    char *nodeval=NULL;
    const char *tempval=nodeptr->node.text().get();
    if (NULL != tempval && '\0' != tempval[0]) {
        size_t vallen=std::strlen(tempval);
        nodeval=(char*)nconf_memalloc(vallen + 1);
        if (NULL != nodeval) { std::strcpy(nodeval, tempval); }
    }
    return nodeval;
}

char* nconf_xmlutils_get_node_prop(nconf_nodeptr_t nodeptr, char *propname)
{
    pugi::xml_attribute prop=nodeptr->node.attribute(propname);
    char *retprop=NULL;
    if (NULL != prop) {
        const char *propval=prop.value();
        size_t proplen=std::strlen(propval);
        retprop=(char*)nconf_memalloc(proplen + 1);
        if (NULL != retprop) { std::strcpy(retprop, propval); }
    }
    return retprop;
}

char* nconf_xmlutils_get_ns_node_prop(nconf_nodeptr_t nodeptr, char *propname, char *nspace)
{
    pugi::xml_node cur=nodeptr->node;
    std::string search_prop=propname;
    bool prefix_found=false;
    char *retprop=NULL;
    while (cur) {
        pugi::xml_attribute attr;
        if (pugi::node_element!=cur.type()) { cur=cur.parent(); continue; }
        for (attr = cur.first_attribute(); attr; attr = attr.next_attribute()) {
            const char* attrname = attr.name();
            size_t prefixLength = std::strlen("xmlns:");
            if (std::strncmp(attrname, "xmlns:", prefixLength) == 0) {
                if(std::strcmp(attr.value(), nspace)==0) {
                    search_prop=std::string(attrname+6) + ":" + propname;
                    prefix_found=true;
                    break;
                }
            }
        }
        if (prefix_found) { break; }
        cur=cur.parent();
    }
    pugi::xml_attribute prop=nodeptr->node.attribute(search_prop.c_str());
    if (NULL != prop) {
        const char *propval=prop.value();
        size_t proplen=std::strlen(propval);
        retprop=(char*)nconf_memalloc(proplen + 1);
        if (NULL != retprop) { std::strcpy(retprop, propval); }
    }
    return retprop;
}

char* nconf_xmlutils_get_node_namespace_uri(nconf_nodeptr_t nodeptr)
{
    char *retns=NULL;
    if (NULL != nodeptr) {
        nconf_xmlutils_set_node_nsasc(nodeptr);
        if (!nodeptr->nsassoc.empty()) {
            std::string& tmp=nodeptr->nsassoc[""];
            retns=const_cast<char*>(tmp.c_str());
        }
    }
    return retns;
}

char* nconf_xmlutils_get_node_path(nconf_nodeptr_t nodeptr, uint8_t level)
{
    nconf_nodeptr_stack_t* stack=NULL;
    pugi::xml_node cur=nodeptr->node;
    std::string nodepath;
    char* retpath=NULL;
    uint8_t lcount=0;
    while (cur != NULL) {
        nconf_xmlutils_nptr_stack_push(&stack, cur);
        if (level > 0 && lcount >= level) { break; }
        cur=cur.parent();
        lcount++;
    }
    while (stack != NULL) {
        cur=nconf_xmlutils_nptr_stack_pop(&stack);
        if (pugi::node_element==cur.type()) {
            nodepath += (std::string("/") + cur.name());
        }
    }
    while (stack != NULL) {
        (void)nconf_xmlutils_nptr_stack_pop(&stack);
    }
    retpath=(char*)nconf_memalloc(nodepath.length() + 1); //(+1 for null terminator)
    if(ub_assert_fatal((NULL!=retpath), __func__, "alloc error")){return NULL;}
    std::memset(retpath, 0, nodepath.length() + 1);
    std::strcpy(retpath, nodepath.c_str());
    return retpath;
}

char* nconf_xmlutils_get_node_header(nconf_nodeptr_t nodeptr)
{
    std::stringstream ss;
    pugi::xml_node node=nodeptr->node;
    pugi::xml_attribute attr;
    ss << "<" << node.name();
    for (attr=node.first_attribute(); attr; attr=attr.next_attribute()){
        ss << " " << attr.name() << "=\"" << attr.value() << "\"";
    }
    ss << ">";
    return nconf_strdup(const_cast<char*>(ss.str().c_str()));
}

char* nconf_xmlutils_get_module_node_path(char *modname, nconf_nodeptr_t nodeptr, uint8_t level)
{
    nconf_nodeptr_stack_t* stack=NULL;
    pugi::xml_node cur=nodeptr->node;
    std::string nodepath;
    char* retpath=NULL;
    uint8_t lcount=0;
    while (cur != NULL) {
        nconf_xmlutils_nptr_stack_push(&stack, cur);
        if (level > 0 && lcount >= level) { break; }
        cur=cur.parent();
        lcount++;
    }
    lcount=0; // reset level count
    while (stack != NULL) {
        cur=nconf_xmlutils_nptr_stack_pop(&stack);
        if (pugi::node_element==cur.type()) {
            if (0==lcount && NULL != modname) {
                nodepath += (std::string("/") + modname);
            } else {
                nodepath += (std::string("/") + cur.name());
            }
            lcount++;
        }
    }
    while (stack != NULL) {
        (void)nconf_xmlutils_nptr_stack_pop(&stack);
    }
    retpath=(char*)nconf_memalloc(nodepath.length()+1); //(+1 for null terminator)
    if(ub_assert_fatal((NULL!=retpath), __func__, "alloc error")){return NULL;}
    std::memset(retpath, 0, nodepath.length()+1);
    std::strcpy(retpath, nodepath.c_str());
    return retpath;
}

char* nconf_xmlutils_sanitize_value_string(char *valstr, int *isUpdated)
{
    char *retstr=valstr;
    int ampcnt=nconf_xmlutils_count_ampersands(valstr);
    if (ampcnt>0) {
        size_t vallen=strlen(valstr)+1;
        size_t retlen=vallen+(strlen("&amp;")*ampcnt);
        char *tmpbuf=nconf_xmlutils_replaced_amp(valstr, vallen, retlen);
        if (tmpbuf) {
            retstr=tmpbuf;
            *isUpdated=1;
        }
    }
    return retstr;
}

char* nconf_xmlutils_remove_keyinfo(char *npathcstr)
{
    std::string nodepath=npathcstr;
    std::string nodepath_wout_key=nconf_xmlutils_remove_keyinfo(nodepath);
    return (char*)nconf_strdup(const_cast<char*>(nodepath_wout_key.c_str()));
}

bool nconf_xmlutils_node_has_children(nconf_nodeptr_t nodeptr)
{
    bool bret=false;
    pugi::xml_node childnode=nodeptr->node.first_child();
    if (NULL != nodeptr->node.first_child() &&
        pugi::node_element==childnode.type()) {
        bret=true;
    }
    return bret;
}

void nconf_xmlutils_lib_init(void)
{
    static bool is_initialized=false;
    if (!is_initialized) {
        pugi::set_memory_management_functions(nconf_xmlutils_alloc, nconf_xmlutils_free);
        is_initialized=true;
    }
}

void nconf_xmlutils_set_node_prop(nconf_nodeptr_t nodeptr, char *name, char *value)
{
    nodeptr->node.append_attribute(name) = value;
}

void nconf_xmlutils_copy_node_prop(nconf_nodeptr_t dst, nconf_nodeptr_t src)
{
    pugi::xml_attribute attr;
    for (attr=src->node.first_attribute(); attr; attr=attr.next_attribute()) {
        std::string attrname=attr.name();
        if (attrname.find("xmlns") == std::string::npos) {
            dst->node.append_copy(attr);
        }
    }
}

void nconf_xmlutils_new_node_namespace(nconf_nodeptr_t nodeptr, char *nspace, char *prefix)
{
    std::string nspace_attr_name = "xmlns";
    if (NULL != prefix && 0!=prefix[0]) {
        nspace_attr_name += (std::string(":") + prefix);
    } else {
        /* default namespace (i.e. no prefix) need to be associated to the node */
        nodeptr->nsassoc[""]=std::string(nspace);
    }
    nodeptr->node.append_attribute(nspace_attr_name.c_str()) = nspace;
}

void nconf_xmlutils_copy_node_nsdefs(nconf_nodeptr_t dst, nconf_nodeptr_t src)
{
    pugi::xml_attribute attr;
    for (attr=src->node.first_attribute(); attr; attr=attr.next_attribute()) {
        std::string attrname=attr.name();
        std::string::size_type index=attrname.find("xmlns");
        if (0 == index) {
            dst->node.append_copy(attr);
            if (std::strchr(attrname.c_str(), ':')==NULL) {
                /* default namespace (i.e. no prefix) need to be associated
                 * to the node */
                dst->nsassoc[""]=std::string(attr.value());
            }
        }
    }
}

void nconf_xmlutils_set_node_nsasc(nconf_nodeptr_t nodeptr)
{
    if (NULL != nodeptr && nodeptr->nsassoc.empty()) {
        for (pugi::xml_node node=nodeptr->node; node; node=node.parent()) {
            pugi::xml_attribute attr;
            for (attr=node.first_attribute(); attr; attr=attr.next_attribute()){
                std::string attrname=attr.name();
                std::string::size_type index=attrname.find("xmlns");
                if (0 == index) {
                    if (std::strchr(attrname.c_str(), ':')==NULL) {
                        nodeptr->nsassoc[""]=std::string(attr.value());
                        break;
                    }
                }
            }
            if (!nodeptr->nsassoc.empty()) { break; }
        }
    }
}

void nconf_xmlutils_set_node_content(nconf_nodeptr_t nodeptr, char *content)
{
    nodeptr->node.text().set(content);
}

void nconf_xmlutils_delete_child_node(nconf_nodeptr_t parent, nconf_nodeptr_t child)
{
    if (NULL != parent && NULL != child) {
        pugi::xml_node pnode=parent->node;
        pugi::xml_node cnode=child->node;
        if(!pnode.remove_child(cnode)) {
            UB_LOG(UBL_ERROR, "%s:failed to remove child node: %s\n",
                   __func__, cnode.name());
        }
    }
}

void nconf_xmlutils_delete_all_child_nodes(nconf_nodeptr_t nodeptr)
{
    if (NULL != nodeptr) {
        if (!nodeptr->node.remove_children()) {
            UB_LOG(UBL_ERROR, "%s:failed to remove all child node of [%s]\n",
                   __func__, nodeptr->node.name());
        }
    }
}

void nconf_xmlutils_delete_child_nodes_of_childs(nconf_nodeptr_t nodeptr)
{
    if (NULL != nodeptr) {
        pugi::xml_node parent=nodeptr->node;
        for (pugi::xml_node child=parent.first_child(); child;
             child=child.next_sibling()) {
            if (!child.remove_children()) {
                UB_LOG(UBL_ERROR, "%s:failed to remove all child node of [%s]\n",
                       __func__, child.name());
            }
        }
    }
}

void nconf_xmlutils_free_node_path(char *nodepath)
{
    nconf_memfree(nodepath);
}

void nconf_xmlutils_free_node(nconf_nodeptr_t nodeptr)
{
    if (NULL != nodeptr) {
        XmlUtils::instance()->release_nodeinst(nodeptr);
    }
}

void nconf_xmlutils_release_node(nconf_nodeptr_t nodeptr)
{
    return nconf_xmlutils_free_node(nodeptr);
}

void nconf_xmlutils_release_all_nodeinst(void)
{
    XmlUtils::instance()->release_all_nodeinst();
}

void nconf_xmlutils_free_doc(nconf_docptr_t doc)
{
    delete doc;
}

void nconf_xmlutils_free(uint8_t *buf)
{
    nconf_memfree(buf);
}

void nconf_xmlutils_free_encode_buf(uint8_t *buf)
{
    /* release memory used by std::string */
    std::string().swap(XmlUtils::instance()->get_docstr());
    NCONF_UNUSED(buf);
}

void nconf_xmlutils_free_nptr_encode_buf(uint8_t *buf)
{
    /* release memory used by std::string */
    std::string().swap(XmlUtils::instance()->get_nodestr());
    NCONF_UNUSED(buf);
}

void nconf_xmlutils_release_sanitize_value(char *valstr)
{
    nconf_memfree(valstr);
}

void nconf_xmlutils_dump_docptr(nconf_docptr_t doc)
{
#ifdef NCONF_DEBUG_LOG_ENABLED
    doc->inst.print(std::cout, "  ",
            pugi::format_default | pugi::format_no_escapes,
            pugi::encoding_utf8);
#else
    NCONF_UNUSED(doc);
#endif
}

void nconf_xmlutils_get_docptr_str(nconf_docptr_t doc, char **xmlstr, uint32_t *xmllen)
{
    std::stringstream doc_sstream;
    doc->inst.save(doc_sstream, "",
            pugi::format_default | pugi::format_no_escapes,
            pugi::encoding_utf8);
    XmlUtils::instance()->get_docstr() = doc_sstream.str();
    *xmlstr=(char*)XmlUtils::instance()->get_docstr().c_str();
    *xmllen=XmlUtils::instance()->get_docstr().length();
}

void nconf_xmlutils_get_nodeptr_str(nconf_nodeptr_t nptr, char **xmlstr, uint32_t *xmllen)
{
    std::stringstream node_sstream;
    nptr->node.print(node_sstream, "",
            pugi::format_default | pugi::format_no_escapes,
            pugi::encoding_utf8);
    XmlUtils::instance()->get_nodestr() = node_sstream.str();
    *xmlstr=(char*)XmlUtils::instance()->get_nodestr().c_str();
    *xmllen=XmlUtils::instance()->get_nodestr().length();
}

void nconf_xmlutils_dump_nodeptr(nconf_nodeptr_t nodeptr)
{
#ifdef NCONF_DEBUG_LOG_ENABLED
    nodeptr->node.print(std::cout, "  ",
            pugi::format_default | pugi::format_no_escapes,
            pugi::encoding_utf8);
#else
    NCONF_UNUSED(nodeptr);
#endif
}

void nconf_xmlutils_xpath_release(nconf_xpath_args_t *param)
{
    if (NULL != param->foundnodes.node) {
        for (size_t i = 0; i < param->foundnodes.count; i++) {
            if (param->foundnodes.node[i]) {
                delete param->foundnodes.node[i];
                param->foundnodes.node[i]=NULL;
            }
        }
        delete[] param->foundnodes.node;
        param->foundnodes.node=NULL;
    }
}

void nconf_xmlutils_release_search_handle(nconf_xpath_args_t *param)
{
    if (NULL != param) { UB_SD_RELMEM(NCONF_XMLUTILS_SEARCH_ARGS, param); }
}

void nconf_xmlutils_cleanup(void)
{
    XmlUtils::instance()->cleanup();
}

void nconf_xmlutils_release_tree_info(nconf_treeinfo_t *treeinfo)
{
    if (NULL != treeinfo) {
        for (uint8_t i=0; i < treeinfo->count; ++i) {
            if (NULL != treeinfo->nodepaths && NULL != treeinfo->nodepaths[i]) {
                nconf_memfree(treeinfo->nodepaths[i]);
            }
            if (NULL != treeinfo->nodevalues && NULL != treeinfo->nodevalues[i]) {
                nconf_memfree(treeinfo->nodevalues[i]);
            }
        }
        if (NULL != treeinfo->nodepaths) { nconf_memfree(treeinfo->nodepaths); }
        if (NULL != treeinfo->nodevalues) { nconf_memfree(treeinfo->nodevalues); }
        nconf_memfree(treeinfo);
    }
}

nconf_treeinfo_t* nconf_xmlutils_generate_tree_info(nconf_nodeptr_t nodeptr,
                                                    nconf_keychecker_t iskeycb,
                                                    nconf_treeinfo_type_t type)
{
    nconf_treeinfo_t *tree=NULL;
    if (NULL != nodeptr) {
        char *modname=nconf_xmlutils_get_module_name(nodeptr->node);
        if (NCONF_TREEINFO_TYPE_CONFIG==type) {
            ConfigVector result=nconf_xmlutils_flatten_xmltree<ConfigVector>(
                nodeptr, modname, iskeycb);
            nconf_xmlutils_sort_configs(result, iskeycb);
            tree=nconf_xmlutils_get_config_treeinfo(result);
        } else {
            FilterVector result=nconf_xmlutils_flatten_xmltree<FilterVector>(
                nodeptr, modname, iskeycb);
            nconf_xmlutils_sort_filters(result, iskeycb);
            tree=nconf_xmlutils_get_filter_treeinfo(result);
        }
    } else {
        UB_LOG(UBL_ERROR, "%s:invalid parameter\n", __func__);
    }
    return tree;
}

nconf_xpath_args_t* nconf_xmlutils_get_search_handle(void)
{
    nconf_xpath_args_t *rhandle=(nconf_xpath_args_t *)
        UB_SD_GETMEM(NCONF_XMLUTILS_SEARCH_ARGS, sizeof(nconf_xpath_args_t));
    if(ub_assert_fatal((NULL!=rhandle), __func__, "alloc error")){
        return NULL;
    }
    return rhandle;
}

int nconf_xmlutils_xpath_search(nconf_xpath_args_t *param)
{
    int ret=-1;
    pugi::xml_document& doc=param->xmldoc->inst;
    char *xpath_expr=nconf_xmlutils_create_xpath_expr(param);
    if (NULL != xpath_expr) {
        UB_LOG(UBL_DEBUG, "%s:xpath search: %s\n", __func__, xpath_expr);
        pugi::xpath_node_set search_result=doc.select_nodes(xpath_expr);
        if (!search_result.empty()) {
            ret=nconf_xmlutils_set_found_nodes(param, search_result);
        } else {
            UB_LOG(UBL_DEBUG, "%s:Failed to evaluate XPath expression: %s\n",
                   __func__, xpath_expr);
        }
        nconf_memfree(xpath_expr);
    } else {
        UB_LOG(UBL_DEBUG, "%s:Failed to generate XPath expression\n", __func__);
    }
    return ret;
}

XmlUtils* XmlUtils::s_util_inst = nullptr;

XmlUtils* XmlUtils::instance()
{
    if (!s_util_inst) {
        s_util_inst = new XmlUtils;
        if(ub_assert_fatal((NULL!=s_util_inst), __func__, "alloc error")){
            return NULL;
        }
    }
    return s_util_inst;
}

void XmlUtils::cleanup()
{
    if (s_util_inst) {
        s_util_inst->release_all_nodeinst();
        delete s_util_inst;
        s_util_inst=nullptr;
    }
}

nconf_nodeptr_t XmlUtils::new_nodeinst(pugi::xml_document *refdoc)
{
    nconf_nodeptr_t retnptr=NULL;
    for (uint8_t i=0; i<NCONF_MAX_NODEPTR_NUM; i++) {
        if (NULL==nptr[i]) {
            if (void *ptr=nconf_memalloc(sizeof(struct _nconf_docptr))) {
                retnptr = new (ptr) _nconf_nodeptr();
                if (NULL==refdoc) {
                    if (void *ptr2=nconf_memalloc(sizeof(pugi::xml_document))){
                        retnptr->refdoc=new (ptr2) pugi::xml_document;
                        retnptr->refdocmem=ptr2;
                    }
                } else {
                    retnptr->refdoc=refdoc;
                }
                if (NULL != retnptr->refdoc) {
                    nptr[i]=retnptr;
                    nptrnum++;
                } else {
                    UB_LOG(UBL_ERROR, "%s:xml_document alloc failed\n", __func__);
                    delete retnptr;
                    retnptr=NULL;
                }
            } else {
                UB_LOG(UBL_ERROR, "%s:nodeptr alloc failed\n", __func__);
            }
            break;
        }
    }
    if (nptrnum >= NCONF_MAX_NODEPTR_NUM) {
        UB_LOG(UBL_ERROR, "%s:Max Node ptr num reached=%u\n", __func__, nptrnum);
    }
    return retnptr;
}

void XmlUtils::release_nodeinst(nconf_nodeptr_t nodeinst)
{
    bool found=false;
    for (uint8_t i=0; i<NCONF_MAX_NODEPTR_NUM; i++) {
        if (nodeinst==nptr[i]) {
            found=true;
            if (NULL != nptr[i]->refdocmem) {
                nptr[i]->refdoc->~xml_document();
                nconf_memfree(nptr[i]->refdocmem);
            }
            delete nptr[i];
            nptr[i]=NULL;
            nptrnum--;
        }
    }
    if (!found) {
        UB_LOG(UBL_DEBUG, "%s: Node instance not found=%u\n", __func__, nptrnum);
    }
}

void XmlUtils::release_all_nodeinst(void)
{
    for (uint8_t i=0; i<NCONF_MAX_NODEPTR_NUM; i++) {
        if (NULL!=nptr[i]) {
            if (NULL != nptr[i]->refdocmem) {
                nptr[i]->refdoc->~xml_document();
                nconf_memfree(nptr[i]->refdocmem);
            }
            delete nptr[i];
            nptr[i]=NULL;
            nptrnum--;
        }
    }
    UB_LOG(UBL_DEBUG, "%s: Node instance count=%u\n", __func__, nptrnum);
}

static void nconf_xmlutils_nptr_stack_push(nconf_nodeptr_stack_t **stack,
                                           pugi::xml_node node)
{
    nconf_nodeptr_stack_t* new_node= new nconf_nodeptr_stack_t;
    if (NULL != new_node) {
        new_node->node=node;
        new_node->next=*stack;
        *stack=new_node;
    } else {
        nconf_memusage(UBL_INFO);
    }
}

static pugi::xml_node nconf_xmlutils_nptr_stack_pop(nconf_nodeptr_stack_t **stack)
{
    pugi::xml_node node;
    if (NULL != *stack) {
        nconf_nodeptr_stack_t *temp=(*stack);
        *stack=(*stack)->next;
        node=temp->node;
        delete temp;
    }
    return node;
}

static char* nconf_xmlutils_create_xpath_expr(nconf_xpath_args_t *args)
{
    uint32_t bufsz=0;
    char *xpath_expr=nconf_xmlutils_get_xpath_buf(args, &bufsz);
    if (NULL != xpath_expr) {
        uint32_t enclen=nconf_xmlutils_encode_xpath_expr(args, xpath_expr, bufsz);
        if (enclen > bufsz) {
            UB_LOG(UBL_ERROR, "%s:unexpected encode length [%u != %u]: %s\n",
                   __func__, enclen, bufsz, xpath_expr);
            nconf_memfree(xpath_expr);
            xpath_expr=NULL;
        }
    } else {
        UB_LOG(UBL_ERROR, "%s:unable to get output buffer\n", __func__);
    }
    return xpath_expr;
}

static char* nconf_xmlutils_get_xpath_buf(nconf_xpath_args_t *args, uint32_t *len)
{
    char *buf=NULL;
    if (0!=args->nodesnum) {
        uint32_t bufsz=nconf_xmlutils_get_xpath_buf_size(args);
        if (bufsz > 2) {
            buf=(char*)nconf_memalloc(bufsz+1);
            if(ub_assert_fatal((NULL!=buf), __func__, "alloc error")){
                return NULL;
            }
            memset(buf, 0, bufsz+1);
            *len=bufsz;
        }
    } else {
        UB_LOG(UBL_ERROR, "%s:node name can't be null\n", __func__);
    }
    return buf;
}

static uint32_t nconf_xmlutils_get_xpath_buf_size(nconf_xpath_args_t *args)
{
    uint32_t predlen=0;
    uint32_t bufsz=0;
    uint8_t i;
    for(i=0; i<args->nodesnum; i++) {
        if (i > 0) { bufsz += 3; } // for ' | '
        /* add length of node name and namespace prefix if any */
        bufsz += nconf_xmlutils_get_nodename_len(args, i);
        /* add length of value and/or predicate attributes */
        if (nconf_xmlutils_has_predicate(args, i, &predlen)) {
            bufsz+=predlen;
        }
    }
    return bufsz+1; //+1 for null terminator
}

static uint32_t nconf_xmlutils_get_nodename_len(nconf_xpath_args_t *args, uint8_t index)
{
    return 2+strlen(args->nodes[index].name); //+2 for '//'
}

static bool nconf_xmlutils_has_predicate(nconf_xpath_args_t *args, uint8_t index,
                                         uint32_t *plen)
{
    uint32_t len=0;
    bool ret=false;
    if (args->nodes[index].value != NULL && args->nodes[index].value[0] != '\0') {
        // "text()='<nodevalue>'"
        len+=strlen(args->nodes[index].value);
        len+=9; //+9 for text()=''
        ret=true;
    }
    if (ret) {
        len += 2; // +2 for '[' and ']'
        *plen = len;
    }
    return ret;
}

static uint32_t nconf_xmlutils_encode_xpath_expr(nconf_xpath_args_t *args,
                                                 char *buf, uint32_t bufsz)
{
    uint32_t baselen, plen, curlen=bufsz;
    char *cur=buf;
    uint8_t i;
    for(i=0; i<args->nodesnum; i++) {
        if (i > 0) { strcat(buf, " | "); cur+=3; curlen-=3; }
        baselen=nconf_xmlutils_create_xpath_base(args, i, cur, curlen);
        if (baselen > 0) {
            plen=nconf_xmlutils_add_xpath_predicate(args, i, buf, curlen);
            cur+=(baselen+plen);
            curlen-=(baselen+plen);
        } else {
            UB_LOG(UBL_ERROR, "%s:invalid xpath base len\n", __func__);
            break;
        }
    }
    return (uint32_t)(cur-buf);
}

static uint32_t nconf_xmlutils_create_xpath_base(nconf_xpath_args_t *args, uint8_t index,
                                                 char *buf, uint32_t bufsz)
{
    char *xpath_base=buf;
    uint32_t baselen=0;
    if (NULL != xpath_base) {
        snprintf(xpath_base, bufsz, "//%s", args->nodes[index].name);
        baselen=strlen(xpath_base);
    } else {
        UB_LOG(UBL_ERROR, "%s:invalid buffer pointer\n", __func__);
    }
    return baselen;
}

static uint32_t nconf_xmlutils_add_xpath_predicate(nconf_xpath_args_t *args, uint8_t index,
                                                   char *buf, uint32_t bufsz)
{
    uint32_t len=0;
    // "text()='<nodevalue>'"
    if (args->nodes[index].value != NULL && args->nodes[index].value[0] != '\0') {
        strcat(buf, "[");
        len+=1;
        strcat(buf, "text()='");
        strcat(buf, args->nodes[index].value);
        strcat(buf, "'");
        len+=strlen(args->nodes[index].value) + 9; //+9 for text()=''
    }
    if (len>0) {
        strcat(buf, "]");
        len+=1;
    }
    NCONF_UNUSED(bufsz);
    return len;
}

static int nconf_xmlutils_set_found_nodes(nconf_xpath_args_t *param,
                                          pugi::xpath_node_set search_result)
{
    size_t ndlist_size=search_result.size()*sizeof(nconf_nodeptr_t);
    param->foundnodes.node=new nconf_nodeptr_t[search_result.size()];
    if(ub_assert_fatal((NULL!=param->foundnodes.node), __func__, "alloc error")){
        return -1;
    }
    memset(param->foundnodes.node, 0, ndlist_size);
    param->foundnodes.count=0;
    pugi::xpath_node_set::const_iterator it=search_result.begin();
    for (int i=0; it != search_result.end(); ++it, i++) {
        param->foundnodes.node[i]=new struct _nconf_nodeptr;
        param->foundnodes.node[i]->node=it->node();
        param->foundnodes.count++;
    }
    return 0;
}

static char* nconf_xmlutils_get_module_name(pugi::xml_node& node)
{
    pugi::xml_attribute attr;
    char *attrval=NULL, *retname=NULL;
    for (attr=node.first_attribute(); attr; attr=attr.next_attribute()){
        if (std::strcmp(attr.name(), "xmlns")==0) {
            attrval=const_cast<char*>(attr.value());
        }
    }
    if (NULL != attrval && attrval[0] != '\0') {
        char *modname=std::strrchr(attrval, '/');
        if (NULL==modname) {
            modname=std::strrchr(attrval, ':');
        }
        if (NULL!=modname) {
            modname=&modname[1];
            retname=modname;
        } else {
            UB_LOG(UBL_ERROR, "%s:invalid module namespace: %s\n", __func__, attrval);
        }
    } else {
        UB_LOG(UBL_ERROR, "%s:namespace not found: %s\n", __func__, node.name());
    }
    return retname;
}

static int nconf_xmlutils_count_ampersands(const char *valstr)
{
    const char *amp = "&amp;";
    const char *cur = valstr;
    size_t amplen = strlen(amp);
    int ampnum = 0;
    while (*cur != '\0') {
        cur = strstr(cur, "&");
        if (cur == NULL) {
            break;
        }
        size_t remainingLen = strlen(cur);
        if (remainingLen < amplen || strncmp(cur, amp, amplen) != 0) {
            ampnum++;
        }
        cur++;  // Move past the '&'
    }
    return ampnum;
}

static char* nconf_xmlutils_replaced_amp(char *instr, size_t inlen, size_t outlen)
{
    const char *amp="&amp;";
    size_t amplen=strlen("&amp;");
    char *outbuf=NULL;
    size_t pos=0, i=0;

    outbuf=(char*)nconf_memalloc(outlen + 1); //(+1 for null terminator)
    if(ub_assert_fatal(outbuf, __func__, "alloc error")){return NULL;}
    std::memset(outbuf, 0, outlen + 1);

    while(i < inlen) {
        if (instr[i]=='&') {
            if (strncmp(&instr[i], amp, amplen)==0) {
                /* If it's already &amp;, just copy it */
                memcpy(&outbuf[pos], &instr[i], amplen);
                pos+=amplen;
                i+=amplen;
            } else {
                memcpy(&outbuf[pos], amp, amplen);
                pos+=amplen;
                i++;
            }
        } else {
            outbuf[pos]=instr[i];
            pos++;
            i++;
        }
    }
    outbuf[pos]='\0';
    return outbuf;
}

static void* nconf_xmlutils_alloc(size_t size)
{
    return nconf_memalloc(size);
}

static void nconf_xmlutils_free(void *ptr)
{
    return nconf_memfree(ptr);
}

template <typename VectorType>
static VectorType nconf_xmlutils_flatten_xmltree(nconf_nodeptr_t xmltree, char *modname,
                                                 nconf_keychecker_t iskeycb)
{
    NodeStack node_stack;
    VectorType treeinfo;

    if (NULL==xmltree) {
        UB_LOG(UBL_ERROR, "%s:invalid parameter\n", __func__);
        goto end_oper;
    }

    node_stack.push({xmltree->node, std::string("/")+modname});
    while (!node_stack.empty()) {
        pugi::xml_node current_node=node_stack.top().first;
        std::string current_path=node_stack.top().second;
        node_stack.pop();
        if (current_node.first_child()==nullptr ||
            current_node.first_child().type() != pugi::node_element) {
            std::string current_modpath=
                nconf_xmlutils_remove_keyinfo(current_path) + "/" +
                current_node.name();
            bool isKey = (NULL != iskeycb) ?
                iskeycb(current_modpath.c_str(), NULL) : false;
            if (isKey) {
                nconf_xmlutils_process_key_leafs(current_node, current_path,
                                                 treeinfo, node_stack);
            } else {
                nconf_xmlutils_process_normal_leafs(current_node, current_path,
                                                    treeinfo);
            }
        } else {
            current_path += std::string("/") + current_node.name();
            for (pugi::xml_node child=current_node.first_child(); child;
                 child = child.next_sibling()) {
                node_stack.push({child, current_path});
            }
        }
    }

end_oper:
    return treeinfo;
}

static void nconf_xmlutils_process_key_leafs(pugi::xml_node current_node, std::string& current_path,
                                             FilterVector& filters, NodeStack& node_stack)
{

    std::string key_value=current_node.text().get();
    if (key_value.length() > 0) {
        std::string previous_path=current_path;
        nconf_xmlutils_update_keyinfo(current_path, current_node, filters);
        if (current_node.previous_sibling()==NULL && current_node.next_sibling()==NULL) {
            nconf_xmlutils_add_tree_info(filters, current_path);
        } else {
            nconf_xmlutils_update_node_stack(node_stack, previous_path, current_path);
        }
    }
}

static void nconf_xmlutils_process_key_leafs(pugi::xml_node current_node, std::string& current_path,
                                             ConfigVector& configs, NodeStack& node_stack)
{

    std::string key_value=current_node.text().get();
    if (key_value.length() > 0) {
        std::string previous_path=current_path;
        nconf_xmlutils_update_keyinfo(current_path, current_node, configs);
        if (current_node.previous_sibling()==NULL && current_node.next_sibling()==NULL) {
            /* when only list is available, append '/dummy' to the current path
             * and set value to '0' */
            char *operation=nconf_xmlutils_get_edit_operation(current_node);
            if (NULL != operation) {
                nconf_xmlutils_add_tree_info(configs, current_path+"/dummy", "0", operation);
                nconf_memfree(operation);
            } else {
                nconf_xmlutils_add_tree_info(configs, current_path+"/dummy", "0", "");
            }
        } else {
            nconf_xmlutils_update_node_stack(node_stack, previous_path, current_path);
        }
    }
}

static void nconf_xmlutils_process_normal_leafs(pugi::xml_node current_node,
                                                std::string& current_path,
                                                FilterVector& filters)
{
    nconf_xmlutils_add_tree_info(filters,
            current_path + "/" + current_node.name());
}

static void nconf_xmlutils_process_normal_leafs(pugi::xml_node current_node,
                                                std::string& current_path,
                                                ConfigVector& configs)
{
    char *operation=nconf_xmlutils_get_edit_operation(current_node);
    if (NULL != operation) {
        nconf_xmlutils_add_tree_info(configs,
            current_path+"/"+current_node.name(),
            current_node.text().get(), operation);
        nconf_memfree(operation);
    } else {
        nconf_xmlutils_add_tree_info(configs,
            current_path+"/"+current_node.name(),
            current_node.text().get(), "");
    }
}

static void nconf_xmlutils_sort_filters(FilterVector& paths, nconf_keychecker_t iskeycb)
{
    for (std::string& path : paths) {
        nconf_xmlutils_sort_path_key_info(path, iskeycb);
    }
    std::sort(paths.begin(), paths.end());
}

static void nconf_xmlutils_sort_configs(ConfigVector& cfginfos, nconf_keychecker_t iskeycb)
{
    for (auto& cfginfo : cfginfos) {
        std::string& path=cfginfo.path;
        nconf_xmlutils_sort_path_key_info(path, iskeycb);
    }
    std::sort(cfginfos.begin(), cfginfos.end(), nconf_xmlustils_cfg_path_comparator);
}

static void nconf_xmlutils_sort_path_key_info(std::string& path, nconf_keychecker_t iskeycb)
{
    size_t pos=0;
    size_t key_info_start=path.find('|', pos);
    while (key_info_start != std::string::npos) {
        size_t key_info_end=path.find('/', key_info_start);
        if (key_info_end==std::string::npos) { break; }
        std::string key_info=path.substr(key_info_start,
                                         key_info_end-key_info_start);
        std::string contpath=path.substr(0, key_info_start);
        std::string contpath_nokey=nconf_xmlutils_remove_keyinfo(contpath);
        if (nconf_xmlutils_sort_key_info(contpath_nokey, key_info, iskeycb)) {
            path.replace(key_info_start, key_info_end-key_info_start, key_info);
        }
        pos=key_info_end;
        key_info_start=path.find('|', pos);
    }
}

static bool nconf_xmlutils_sort_key_info(std::string& contpath, std::string& key_info,
                                         nconf_keychecker_t iskeycb)
{
    std::istringstream keyInfoIss(key_info);
    std::vector<std::string> keylist;
    std::string keyPair;
    bool is_updated=false;
    while (std::getline(keyInfoIss, keyPair, '|')) {
        if (keyPair.length()==0) {continue;}
        keylist.push_back(keyPair);
    }
    if (keylist.size() > 1) {
        is_updated=true;
        std::sort(keylist.begin(), keylist.end(),
            [iskeycb, contpath](const std::string& ls, const std::string& rs) {
                std::istringstream lIss(ls);
                std::istringstream rIss(rs);
                std::string lkey, rkey;
                uint8_t lidx, ridx;
                std::getline(lIss, lkey, ':');
                std::getline(rIss, rkey, ':');
                std::string lpath=contpath+"/"+lkey;
                std::string rpath=contpath+"/"+rkey;
                (void)iskeycb(lpath.c_str(), &lidx);
                (void)iskeycb(rpath.c_str(), &ridx);
                return lidx < ridx;
            });
        key_info.clear();
        for (std::string& keyPair : keylist) {
            if (key_info.length() > 0) {
                key_info += keyPair + "|";
            } else {
                key_info = "|" + keyPair + "|";
            }
        }
    }
    return is_updated;
}

static nconf_treeinfo_t* nconf_xmlutils_get_filter_treeinfo(FilterVector paths)
{
    size_t i;
    nconf_treeinfo_t *tree=(nconf_treeinfo_t*)nconf_memalloc(sizeof(nconf_treeinfo_t));
    if (NULL==tree) {
        UB_LOG(UBL_ERROR, "%s:failed to allocate path list\n", __func__);
        goto end_oper;
    }
    if (paths.size()<=0) {
        UB_LOG(UBL_WARN, "%s:empty xml tree\n", __func__);
        tree->nodepaths=NULL;
        tree->count=0u;
        goto end_oper;
    }
    memset(tree, 0, sizeof(nconf_treeinfo_t));
    tree->nodepaths=(char**)nconf_memalloc(paths.size()*sizeof(char*));
    if (NULL==tree->nodepaths) {
        UB_LOG(UBL_ERROR, "%s:failed to allocate nodepath list\n", __func__);
        nconf_memfree(tree);
        tree=NULL;
        goto end_oper;
    }
    memset(tree->nodepaths, 0, paths.size()*sizeof(char*));
    for (i=0; i < paths.size(); ++i) {
        tree->nodepaths[i]=nconf_strdup(const_cast<char*>(paths[i].c_str()));
        if (NULL==tree->nodepaths[i]) {
            UB_LOG(UBL_ERROR, "%s:failed to allocate nodepath string\n", __func__);
            break;
        }
    }
    if (i < paths.size()) {
        /* clean up when an error occurs inside the loop */
        nconf_xmlutils_release_tree_info(tree);
        tree=NULL;
    } else {
        /* set list count */
        tree->count=static_cast<uint8_t>(paths.size());
    }
end_oper:
    return tree;
}

static nconf_treeinfo_t* nconf_xmlutils_get_config_treeinfo(ConfigVector cfginfo)
{
    size_t i;
    nconf_treeinfo_t *tree=(nconf_treeinfo_t*)
            nconf_memalloc(sizeof(nconf_treeinfo_t));
    if (NULL==tree) {
        UB_LOG(UBL_ERROR, "%s:failed to allocate path list\n", __func__);
        goto end_oper;
    }
    UB_LOG(UBL_INFO, "%s:cfginfo.size=%zu\n", __func__, (size_t)cfginfo.size());
    if (cfginfo.size()<=0) {
        UB_LOG(UBL_WARN, "%s:empty xml tree\n", __func__);
        tree->nodepaths=NULL;
        tree->count=0u;
        goto end_oper;
    }
    memset(tree, 0, sizeof(nconf_treeinfo_t));
    tree->nodepaths=(char**)nconf_memalloc(cfginfo.size()*sizeof(char*));
    if (NULL==tree->nodepaths) {
        UB_LOG(UBL_ERROR, "%s:failed to allocate nodepath list\n", __func__);
        nconf_memfree(tree);
        tree=NULL;
        goto end_oper;
    }
    memset(tree->nodepaths, 0, cfginfo.size()*sizeof(char*));
    tree->nodevalues=(char**)nconf_memalloc(cfginfo.size()*sizeof(char*));
    if (NULL==tree->nodevalues) {
        UB_LOG(UBL_ERROR, "%s:failed to allocate nodevalue list\n", __func__);
        nconf_memfree(tree->nodepaths);
        nconf_memfree(tree);
        tree=NULL;
        goto end_oper;
    }
    memset(tree->nodevalues, 0, cfginfo.size()*sizeof(char*));
    for (i=0; i < cfginfo.size(); ++i) {
        tree->nodepaths[i]=nconf_strdup(const_cast<char*>(cfginfo[i].path.c_str()));
        if (NULL==tree->nodepaths[i]) {
            UB_LOG(UBL_ERROR, "%s:failed to allocate nodepath string\n", __func__);
            break;
        }
        if (cfginfo[i].value.length() > 0) {
            tree->nodevalues[i]=nconf_strdup(const_cast<char*>(cfginfo[i].value.c_str()));
            if (NULL==tree->nodevalues[i]) {
                UB_LOG(UBL_ERROR, "%s:failed to allocate nodevalue string\n", __func__);
                break;
            }
        } else {
            tree->nodevalues[i]=NULL;
        }
    }
    if (i < cfginfo.size()) {
        /* clean up when an error occurs inside the loop */
        nconf_xmlutils_release_tree_info(tree);
        tree=NULL;
    } else {
        /* set list count */
        tree->count=static_cast<uint8_t>(cfginfo.size());
    }
end_oper:
    return tree;
}

static bool nconf_xmlustils_cfg_path_comparator(struct config_strings& lhs,
                                                struct config_strings& rhs)
{
    if (lhs.path != rhs.path) {
        return lhs.path < rhs.path;
    } else {
        return lhs.value < rhs.value;
    }
}

static std::string nconf_xmlutils_remove_keyinfo(const std::string& nodepath)
{
    std::string current_path=nodepath;
    std::string nodepath_wout_key=nodepath;
    size_t keyinfo_pos = current_path.find('|', 0);
    while (keyinfo_pos != std::string::npos) {
        size_t nextnode_pos=current_path.find('/', keyinfo_pos+1);
        nodepath_wout_key=current_path.substr(0, keyinfo_pos);
        if (nextnode_pos != std::string::npos) {
            nodepath_wout_key+=current_path.substr(nextnode_pos);
        }
        current_path=nodepath_wout_key;
        keyinfo_pos=current_path.find('|', 0);
    }
    return nodepath_wout_key;
}

static bool nconf_xmlutils_find_key(std::string& key_info, std::string key_hdr)
{
    std::istringstream keyInfoIss(key_info);
    std::string keyPair;
    bool found=false;
    while (std::getline(keyInfoIss, keyPair, '|')) {
        std::istringstream keyPairIss(keyPair);
        std::string exist_key_hdr;
        std::getline(keyPairIss, exist_key_hdr, ':');
        if (key_hdr.compare(exist_key_hdr)==0) { found=true; break; }
    }
    return found;
}

static void nconf_xmlutils_update_keyinfo(std::string& current_path,
                                          pugi::xml_node& current_node,
                                          FilterVector& paths)
{
    std::string key_hdr = std::string(current_node.name()) + ":";
    std::string key_value = current_node.text().get();

    /* update all paths in the list of paths with same parent path as the
     * current node and does not have the key header yet */
    for (std::string& path : paths) {
        if (path.compare(0, current_path.length(), current_path)==0){
            size_t key_info_start, key_info_end;
            if (current_path.back() != '|') {
                key_info_start=current_path.length();
            } else {
                size_t container_start=path.rfind('/', current_path.length()-1);
                key_info_start=path.find('|', container_start);
            }
            key_info_end=path.find('/', key_info_start);
            if (key_info_start==std::string::npos||key_info_end==std::string::npos) {
                UB_LOG(UBL_ERROR, "%s:invalid path entry=[%s]\n",
                       __func__, path.c_str());
                continue;
            }
            std::string key_info=path.substr(key_info_start, key_info_end-key_info_start);
            if (!nconf_xmlutils_find_key(key_info, current_node.name())) {
                std::string key_info_new=(key_info.back() != '|') ?
                                         ("|" + key_hdr + key_value + "|") :
                                         (key_hdr + key_value + "|");
                path.insert(key_info_end, key_info_new);
            }
        }
    }

    /* update the current node paths's key header info */
    current_path += (current_path.back() != '|') ?
                    ("|" + key_hdr + key_value + "|") :
                    (key_hdr + key_value + "|");
}

static void nconf_xmlutils_update_keyinfo(std::string& current_path,
                                          pugi::xml_node& current_node,
                                          ConfigVector& cfginfos)
{
    std::string key_hdr = std::string(current_node.name()) + ":";
    std::string key_value = current_node.text().get();

    /* update all paths in the list of paths with same parent path as the
     * current node and does not have the key header yet */
    for (auto& cfginfo : cfginfos) {
        std::string& path=cfginfo.path;
        if (path.compare(0, current_path.length(), current_path)==0) {
            size_t key_info_start, key_info_end;
            if (current_path.back() != '|') {
                key_info_start=current_path.length();
            } else {
                size_t container_start=path.rfind('/', current_path.length()-1);
                key_info_start=path.find('|', container_start);
            }
            key_info_end=path.find('/', key_info_start);
            if (key_info_start==std::string::npos||key_info_end==std::string::npos) {
                UB_LOG(UBL_ERROR, "%s:invalid path entry=[%s]\n",
                       __func__, path.c_str());
                continue;
            }
            std::string key_info=path.substr(key_info_start, key_info_end-key_info_start);
            if (!nconf_xmlutils_find_key(key_info, current_node.name())) {
                std::string key_info_new=(key_info.back() != '|') ?
                                         ("|" + key_hdr + key_value + "|") :
                                         (key_hdr + key_value + "|");
                path.insert(key_info_end, key_info_new);
            }
        }
    }

    /* update the current node paths's key header info */
    current_path += (current_path.back() != '|') ?
                    ("|" + key_hdr + key_value + "|") :
                    (key_hdr + key_value + "|");
}

static void nconf_xmlutils_add_tree_info(FilterVector& paths, std::string npath)
{
    paths.push_back(npath + "/");
}

static void nconf_xmlutils_add_tree_info(ConfigVector& cfginfo, std::string npath,
                                         std::string nval, std::string operation)
{
    /* append the operation at the end of node path,
     * ampersand (&) is a special character in xml so
     * it is guaranteed not to appear on node paths */
    if (operation.length() > 0) {
        npath += ("&" + operation);
    }
    cfginfo.push_back({npath, nval});
}

static void nconf_xmlutils_update_node_stack(NodeStack& node_stack,
                                             std::string& previous_path,
                                             std::string& updated_path)
{
    NodeStack tmp_stack;
    while (!node_stack.empty()) {
        std::string stacked_path=node_stack.top().second;
        if (stacked_path.compare(previous_path)==0) {
            node_stack.top().second=updated_path;
        }
        tmp_stack.push(node_stack.top());
        node_stack.pop();
    }
    while (!tmp_stack.empty()) {
        node_stack.push(tmp_stack.top());
        tmp_stack.pop();
    }
}

static char* nconf_xmlutils_get_edit_operation(pugi::xml_node& leafnode)
{
    struct _nconf_nodeptr nodeptr;
    nodeptr.node=leafnode;
    return nconf_xmlutils_get_ns_node_prop(&nodeptr, const_cast<char*>("operation"),
                                           const_cast<char*>(NCONF_BASE_XML_NAMESPACE));
}

