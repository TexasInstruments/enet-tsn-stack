/* Automatically generated file.  Don't edit this file.*/

/*-----------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <tsn_unibase/unibase.h>
#include "../yang_modules_namespaces.h"

/*-----------------------------------------------------------------------------
 * Type Definitions
 *----------------------------------------------------------------------------*/

struct module_namespace_map {
    const char *module;
    const char *prefix;
    const char *modns;
};

struct nodepath_namespace_map {
    const char *nodepath;
    const char *modns;
};

struct module_nodepath_info {
    const char *module;
    struct nodepath_namespace_map *npath_nsmap;
};

/*-----------------------------------------------------------------------------
 * Data Tables
 *----------------------------------------------------------------------------*/

struct module_namespace_map module_namespace_map_list[] = {
    { "ietf-netconf", "nc", "urn:ietf:params:xml:ns:netconf:base:1.0" },
    { "ietf-interfaces", "if", "urn:ietf:params:xml:ns:yang:ietf-interfaces" },
    { "ieee802-dot1q-bridge", "dot1q", "urn:ieee:std:802.1Q:yang:ieee802-dot1q-bridge" },
    { "ieee802-dot1ab-lldp", "lldp", "urn:ieee:std:802.1AB:yang:ieee802-dot1ab-lldp" },
    { "ieee1588-ptp", "ptp", "urn:ieee:std:1588:yang:ieee1588-ptp" },
    { "ieee802-dot1q-tsn-config-uni", "dot1q-tsn-config-uni", "urn:ieee:std:802.1Q:yang:ieee802-dot1q-tsn-config-uni" },
    { "ietf-netconf-monitoring", "ncm", "urn:ietf:params:xml:ns:yang:ietf-netconf-monitoring" },
    { "ietf-yang-library", "yanglib", "urn:ietf:params:xml:ns:yang:ietf-yang-library" },
    { "excelfore-tsn-remote", "xl4tsnrmt", "http://excelfore.com/ns/excelfore-tsn-remote" },
    { "excelfore-netconf-server", "xl4nconf", "http://excelfore.com/ns/excelfore-netconf-server" },
    { "ieee802-dot1as-ptp", "dot1as-ptp", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "ieee802-dot1q-sched-bridge", "sched-bridge", "urn:ieee:std:802.1Q:yang:ieee802-dot1q-sched-bridge" },
    { "ieee802-dot1q-preemption-bridge", "preempt-bridge", "urn:ieee:std:802.1Q:yang:ieee802-dot1q-preemption-bridge" },
    { "xl4-ietf-interfaces", "xl4if", "http://excelfore.com/ns/xl4interfaces" },
    { "xl4-ieee1588-ptp", "xl4gptp", "http://excelfore.com/ns/xl4gptp" },
    { "xl4-ieee802-dot1q-bridge", "xl4dot1q", "http://excelfore.com/ns/xl4-ieee802-dot1q-bridge" },
    { "xl4-ieee802-dot1q-tsn-config-uni", "xl4tsnuni", "http://excelfore.com/ns/xl4-ieee802-dot1q-tsn-config-uni" },
    { "xl4-ieee802-dot1ab-lldp", "xl4lldp", "http://excelfore.com/ns/xl4lldp" },
    { "xl4-tsn-remote", "xl4devtsnrmt", "http://excelfore.com/ns/xl4-tsn-remote" },
    { "ietf-inet-types", "inet", "urn:ietf:params:xml:ns:yang:ietf-inet-types" },
    { "ietf-yang-types", "yang", "urn:ietf:params:xml:ns:yang:ietf-yang-types" },
    { "ieee802-types", "ieee", "urn:ieee:std:802.1Q:yang:ieee802-types" },
    { "iana-if-type", "ianaift", "urn:ietf:params:xml:ns:yang:iana-if-type" },
    { "ieee802-dot1q-types", "dot1q-types", "urn:ieee:std:802.1Q:yang:ieee802-dot1q-types" },
    { "ieee802-dot1ab-types", "lldp-types", "urn:ieee:std:802.1Q:yang:ieee802-dot1ab-types" },
    { "ietf-routing", "rt", "urn:ietf:params:xml:ns:yang:ietf-routing" },
    { "ieee802-dot1q-tsn-types", "dot1q-tsn-types", "urn:ieee:std:802.1Q:yang:ieee802-dot1q-tsn-types" },
    { "ietf-x509-cert-to-name", "x509c2n", "urn:ietf:params:xml:ns:yang:ietf-x509-cert-to-name" },
    { "ieee802-dot1q-sched", "sched", "urn:ieee:std:802.1Q:yang:ieee802-dot1q-sched" },
    { "ieee802-dot1q-preemption", "preempt", "urn:ieee:std:802.1Q:yang:ieee802-dot1q-preemption" },
    { NULL, NULL, NULL }
};

struct nodepath_namespace_map ietf_interfaces_npath_nsmap[] = {
    { "/interfaces", "urn:ietf:params:xml:ns:yang:ietf-interfaces" },
    { "/interfaces/interface/bridge-port", "urn:ieee:std:802.1Q:yang:ieee802-dot1q-bridge" },
    { "/interfaces/interface/bridge-port/traffic-class/tc-data", "http://excelfore.com/ns/xl4interfaces" },
    { "/interfaces/interface/bridge-port/traffic-class/cbs-enabled", "http://excelfore.com/ns/xl4interfaces" },
    { "/interfaces/interface/bridge-port/traffic-class/number-of-pqueues", "http://excelfore.com/ns/xl4interfaces" },
    { "/interfaces/interface/bridge-port/traffic-class/pqueue-map", "http://excelfore.com/ns/xl4interfaces" },
    { "/interfaces/interface/bridge-port/gate-parameter-table", "urn:ieee:std:802.1Q:yang:ieee802-dot1q-sched-bridge" },
    { "/interfaces/interface/bridge-port/frame-preemption-parameters", "urn:ieee:std:802.1Q:yang:ieee802-dot1q-preemption-bridge" },
    { "/interfaces/interface/duplex", "http://excelfore.com/ns/xl4interfaces" },
    { "/interfaces/interface/discon-workaround-time", "http://excelfore.com/ns/xl4interfaces" },
    { NULL, NULL }
};

struct nodepath_namespace_map ieee802_dot1q_bridge_npath_nsmap[] = {
    { "/bridges", "urn:ieee:std:802.1Q:yang:ieee802-dot1q-bridge" },
    { NULL, NULL }
};

struct nodepath_namespace_map ieee802_dot1ab_lldp_npath_nsmap[] = {
    { "/lldp", "urn:ieee:std:802.1AB:yang:ieee802-dot1ab-lldp" },
    { "/lldp/local-system-changed", "http://excelfore.com/ns/xl4lldp" },
    { "/lldp/local-port-changed", "http://excelfore.com/ns/xl4lldp" },
    { NULL, NULL }
};

struct nodepath_namespace_map ieee1588_ptp_npath_nsmap[] = {
    { "/ptp", "urn:ieee:std:1588:yang:ieee1588-ptp" },
    { "/ptp/instances/instance/default-ds/gm-capable", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/default-ds/current-utc-offset", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/default-ds/current-utc-offset-valid", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/default-ds/leap59", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/default-ds/leap61", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/default-ds/time-traceable", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/default-ds/frequency-traceable", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/default-ds/ptp-timescale", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/default-ds/time-source", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/current-ds/last-gm-phase-change", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/current-ds/last-gm-freq-change", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/current-ds/gm-timebase-indicator", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/current-ds/time-of-last-freq-change", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/is-measuring-delay", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/as-capable", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/mean-link-delay-thresh", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/neighbor-rate-ratio", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/initial-log-announce-interval", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/current-log-announce-interval", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/use-mgt-log-announce-interval", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/initial-log-sync-interval", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/current-log-sync-interval", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/use-mgt-log-sync-interval", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/sync-receipt-timeout", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/sync-receipt-timeout-interval", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/initial-log-pdelay-req-interval", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/current-log-pdelay-req-interval", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/use-mgt-log-pdelay-req-interval", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/mgt-log-pdelay-req-interval", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/current-log-gptp-cap-interval", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/use-mgt-compute-rate-ratio", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/use-mgt-compute-mean-link-delay", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/allowed-lost-responses", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/allowed-faults", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/gptp-cap-receipt-timeout", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/one-step-tx-oper", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/one-step-receive", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/one-step-transmit", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/initial-one-step-tx-oper", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/current-one-step-tx-oper", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/use-mgt-one-step-tx-oper", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/mgt-one-step-tx-oper", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-ds/pdelay-truncated-timestamps", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-statistics-ds", "urn:ieee:std:802.1AS:yang:ieee802-dot1as-ptp" },
    { "/ptp/instances/instance/ports/port/port-clock", "http://excelfore.com/ns/xl4gptp" },
    { "/ptp/instances/instance/clock-state", "http://excelfore.com/ns/xl4gptp" },
    { "/ptp/instance-domain-map", "http://excelfore.com/ns/xl4gptp" },
    { NULL, NULL }
};

struct nodepath_namespace_map ieee802_dot1q_tsn_config_uni_npath_nsmap[] = {
    { "/tsn-uni", "urn:ieee:std:802.1Q:yang:ieee802-dot1q-tsn-config-uni" },
    { NULL, NULL }
};

struct nodepath_namespace_map ietf_netconf_monitoring_npath_nsmap[] = {
    { "/netconf-state", "urn:ietf:params:xml:ns:yang:ietf-netconf-monitoring" },
    { NULL, NULL }
};

struct nodepath_namespace_map ietf_yang_library_npath_nsmap[] = {
    { "/modules-state", "urn:ietf:params:xml:ns:yang:ietf-yang-library" },
    { NULL, NULL }
};

struct nodepath_namespace_map excelfore_tsn_remote_npath_nsmap[] = {
    { "/tsn-remote", "http://excelfore.com/ns/excelfore-tsn-remote" },
    { NULL, NULL }
};

struct nodepath_namespace_map excelfore_netconf_server_npath_nsmap[] = {
    { "/x4nconf-server", "http://excelfore.com/ns/excelfore-netconf-server" },
    { NULL, NULL }
};

struct module_nodepath_info module_npathns_map_list[] = {
    { "ietf-interfaces", ietf_interfaces_npath_nsmap },
    { "ieee802-dot1q-bridge", ieee802_dot1q_bridge_npath_nsmap },
    { "ieee802-dot1ab-lldp", ieee802_dot1ab_lldp_npath_nsmap },
    { "ieee1588-ptp", ieee1588_ptp_npath_nsmap },
    { "ieee802-dot1q-tsn-config-uni", ieee802_dot1q_tsn_config_uni_npath_nsmap },
    { "ietf-netconf-monitoring", ietf_netconf_monitoring_npath_nsmap },
    { "ietf-yang-library", ietf_yang_library_npath_nsmap },
    { "excelfore-tsn-remote", excelfore_tsn_remote_npath_nsmap },
    { "excelfore-netconf-server", excelfore_netconf_server_npath_nsmap },
    { NULL, NULL }
};

/*-----------------------------------------------------------------------------
 * Namespace APIs
 *----------------------------------------------------------------------------*/

const char *yang_db_get_module_namespace(char *module)
{
    struct module_namespace_map *map=&module_namespace_map_list[0];
    const char *ret=NULL;
    int i;
    if (NULL != module) {
        for (i=0; map[i].module; i++) {
            if(strcmp((char*)map[i].module, module)==0) {
                ret=map[i].modns;
                break;
            }
        }
    }
    return ret;
}

const char *yang_db_get_module_prefix(char *module)
{
    struct module_namespace_map *map=&module_namespace_map_list[0];
    const char *ret=NULL;
    int i;
    if (NULL != module) {
        for (i=0; map[i].module; i++) {
            if(strcmp((char*)map[i].module, module)==0) {
                ret=map[i].prefix;
                break;
            }
        }
    }
    return ret;
}

const char *yang_db_get_namespace_prefix(char *modns)
{
    struct module_namespace_map *map=&module_namespace_map_list[0];
    const char *ret=NULL;
    int i;
    if (NULL != modns) {
        for (i=0; map[i].module; i++) {
            if(strcmp((char*)map[i].modns, modns)==0) {
                ret=map[i].prefix;
                break;
            }
        }
    }
    return ret;
}

const char *yang_db_get_prefix_namespace(char *prefix)
{
    struct module_namespace_map *map=&module_namespace_map_list[0];
    const char *ret=NULL;
    int i;
    if (NULL != prefix) {
        for (i=0; map[i].module; i++) {
            if(strcmp((char*)map[i].prefix, prefix)==0) {
                ret=map[i].modns;
                break;
            }
        }
    }
    return ret;
}

const char *yang_db_get_npath_namespace(const char *module, const char *nodepath, int *is_exact)
{
    struct module_nodepath_info *module_npmap=&module_npathns_map_list[0];
    struct nodepath_namespace_map *npath_nsmap=NULL;
    const char *best_match=NULL;
    int i;

    for (i=0; NULL != module_npmap[i].module; i++) {
        if(strcmp((char*)module_npmap[i].module, module)==0) {
            npath_nsmap=&module_npmap[i].npath_nsmap[0];
            break;
        }
    }

    if (NULL != npath_nsmap) {
        size_t nodepath_len=strlen(nodepath);
        size_t best_match_len=0;
        for (i=0; NULL != npath_nsmap[i].nodepath; i++) {
            size_t currentLen=strlen(npath_nsmap[i].nodepath);
            if (nodepath_len >= currentLen) {
                if (strncmp(nodepath, npath_nsmap[i].nodepath, currentLen)==0) {
                    if (currentLen > best_match_len) {
                        best_match=npath_nsmap[i].modns;
                        best_match_len=currentLen;
                        /* break on exact match */
                        if (nodepath_len==currentLen) { *is_exact=1; break; }
                    }
                }
            }
        }
        if (NULL==best_match) {
            UB_LOG(UBL_WARN, "%s:nodepath not found=%s\n", __func__, nodepath);
        }
    } else {
        UB_LOG(UBL_WARN, "%s:module not found=%s\n", __func__, module);
    }

    return best_match;
}

