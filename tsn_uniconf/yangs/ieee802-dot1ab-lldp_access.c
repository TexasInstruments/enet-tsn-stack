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
#include "yang_modules.h"
#include "ieee802-dot1ab-lldp.h"
#include "ieee802-dot1ab-lldp_access.h"
#include "generated/ieee802-dot1ab-lldp_runconf.h"

/**
 * @brief Refered from uniconfmon. Private struct to store data queried 
 * to shortern params number in functions
 */
typedef struct lldp_cfg_param {
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
} lldp_cfg_param_t;

/*functions prototype*/
static void set_dpara_k1vk0(yang_db_access_para_t *dbpara,  uint8_t k1, bool status);
static void set_dpara_k2vk0(yang_db_access_para_t *dbpara, uint8_t k1, uint8_t k2, bool status);
static bool is_manangement_addr_tx_port_existed(lldp_port_t* port, address_subtype_t subtype, char* man_address);
static management_address_tx_port_t* get_existed_manament_addr_tx(lldp_port_t* port, address_subtype_t subtype, char* man_address);
static bool is_remote_unknown_tlv_existed(remote_systems_data_t* rs_data, uint32_t tlv_type);
static remote_unknown_tlv_t* get_existed_remote_unknown_tlv(remote_systems_data_t* rs_data, uint32_t tlv_type);
static bool is_remote_org_defined_existed(remote_systems_data_t* rs_data, uint32_t info_id, uint32_t info_sybtype, uint32_t info_index);
static remote_org_defined_info_t* get_existed_remote_org(remote_systems_data_t* rs_data, uint32_t info_id, uint32_t info_sybtype, uint32_t info_index);
static bool is_manangement_addr_existed(remote_systems_data_t* rs_data, address_subtype_t subtype, char* addr);
static management_address_t* get_existed_manament_addr(remote_systems_data_t* rs_data, address_subtype_t subtype, char* address);
static bool is_remote_system_data_existed(lldp_port_t* port, timeticks time_mark, uint32_t remote_index);
static bool is_port_existed(yang_lldp_t* lldp, const char * if_name, uint8_t* mac);
static lldp_port_t* get_existed_port(yang_lldp_t* lldp, const char * if_name, uint8_t* mac);
static void fill_remote_statistic_info(uint8_t k3, lldp_cfg_param_t* prm, yang_lldp_t* lldp);
static void fill_local_system_data(uint8_t k3, lldp_cfg_param_t* prm, yang_lldp_t* lldp);
static void fill_management_info(uint8_t* k, address_subtype_t addr_subtype, char* man_addr, lldp_cfg_param_t* prm, lldp_port_t* port);
static void fill_manament_addr(uint8_t* k, address_subtype_t addr_subtype, char* addr, lldp_cfg_param_t* prm, remote_systems_data_t* rs_data);
static void fill_remote_unknown_tlv(uint8_t* k, uint32_t tlv_type, lldp_cfg_param_t* prm, remote_systems_data_t* rs_data);
static void fill_remote_org_info(uint8_t* k, uint32_t info_id, uint32_t info_subtype, uint32_t info_index, lldp_cfg_param_t* prm, remote_systems_data_t* rs_data);
static void fill_port_info(uint8_t* k, char* if_name, uint8_t* mac, lldp_cfg_param_t* prm, yang_lldp_t* lldp);
static void fill_lldp_config_info(void *key, yang_lldp_t* lldp, lldp_cfg_param_t* prm);


/*Functions definition*/
static void set_dpara_k1vk0(yang_db_access_para_t *dbpara,
			                uint8_t k1, bool status)
{
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	dbpara->aps[0] = status?IEEE802_DOT1AB_LLDP_RO:IEEE802_DOT1AB_LLDP_RW;
	dbpara->aps[1] = IEEE802_DOT1AB_LLDP_LLDP;
	dbpara->aps[2] = k1;
	dbpara->aps[3] = 255u;
	dbpara->kvs[0]=NULL;
}

static void set_dpara_k2vk0(yang_db_access_para_t *dbpara,
			                uint8_t k1, uint8_t k2, bool status)
{
	dbpara->onhw=YANG_DB_ONHW_NOACTION;
	dbpara->aps[0] = status?IEEE802_DOT1AB_LLDP_RO:IEEE802_DOT1AB_LLDP_RW;
	dbpara->aps[1] = IEEE802_DOT1AB_LLDP_LLDP;
	dbpara->aps[2] = k1;
	dbpara->aps[3] = k2;
	dbpara->aps[4] = 255u;
	dbpara->kvs[0]=NULL;
}

static bool is_manangement_addr_tx_port_existed(lldp_port_t* port, address_subtype_t subtype, char* man_address)
{
    bool ret = false;
    for (int i=0; i<MAX_PORT_MANAGEMENT_ADDRESS; i++)
    {
        if (strcmp(man_address, port->management_address_tx_port[i].man_address) == 0 && port->management_address_tx_port[i].address_subtype == subtype)
        {
            ret = true;
            break;
        }
    }

    return ret;
}

static management_address_tx_port_t* get_existed_manament_addr_tx(lldp_port_t* port, address_subtype_t subtype, char* man_address)
{
    management_address_tx_port_t* m = NULL;
    for (int i=0; i<MAX_PORT_MANAGEMENT_ADDRESS; i++)
    {
        if (strcmp(man_address, port->management_address_tx_port[i].man_address) == 0 && port->management_address_tx_port[i].address_subtype == subtype)
        {
            m = &port->management_address_tx_port[i];
            break;
        }
    }

    return m;
}

static bool is_remote_unknown_tlv_existed(remote_systems_data_t* rs_data, uint32_t tlv_type)
{
    bool ret = false;
    for (int i=0; i<MAX_UNKNOW_TLV_LEN; i++)
    {
        if (rs_data->remote_unknown_tlv[i].tlv_type == tlv_type)
        {
            ret = true;
            break;
        }
    }

    return ret;
}

static remote_unknown_tlv_t* get_existed_remote_unknown_tlv(remote_systems_data_t* rs_data, uint32_t tlv_type)
{
    remote_unknown_tlv_t* t = NULL;
    for (int i=0; i<MAX_UNKNOW_TLV_LEN; i++)
    {
        if (rs_data->remote_unknown_tlv[i].tlv_type == tlv_type)
        {
            t = &rs_data->remote_unknown_tlv[i];
            break;
        }
    }

    return t;
}

static bool is_remote_org_defined_existed(remote_systems_data_t* rs_data, uint32_t info_id, uint32_t info_sybtype, uint32_t info_index)
{
    bool ret = false;
    for (int i=0; i<MAX_REMOTE_ORG_DEFINE_LEN; i++)
    {
        if (rs_data->remote_org_defined_info[i].info_identifier == info_id 
        && rs_data->remote_org_defined_info[i].info_identifier == info_index
        && rs_data->remote_org_defined_info[i].info_subtype == info_sybtype)
        {
            ret = true;
            break;
        }
    }

    return ret;
}

static remote_org_defined_info_t* get_existed_remote_org(remote_systems_data_t* rs_data, uint32_t info_id, uint32_t info_sybtype, uint32_t info_index)
{
    remote_org_defined_info_t* t = NULL;
    for (int i=0; i<MAX_REMOTE_ORG_DEFINE_LEN; i++)
    {
        if (rs_data->remote_org_defined_info[i].info_identifier == info_id 
        && rs_data->remote_org_defined_info[i].info_identifier == info_index
        && rs_data->remote_org_defined_info[i].info_subtype == info_sybtype)
        {
            t = &rs_data->remote_org_defined_info[i];
            break;
        }
    }

    return t;
}


static bool is_manangement_addr_existed(remote_systems_data_t* rs_data, address_subtype_t subtype, char* addr)
{
    bool ret = false;
    for (int i=0; i<MAX_PORT_MANAGEMENT_ADDRESS; i++)
    {
        if (rs_data->management_address[i].address_subtype == subtype && strcmp(rs_data->management_address[i].address, addr) == 0)
        {
            ret = true;
            break;
        }
    }

    return ret;
}

static management_address_t* get_existed_manament_addr(remote_systems_data_t* rs_data, address_subtype_t subtype, char* addr)
{
    management_address_t* m = NULL;
    for (int i=0; i<MAX_PORT_MANAGEMENT_ADDRESS; i++)
    {
        if (rs_data->management_address[i].address_subtype == subtype && strcmp(rs_data->management_address[i].address, addr) == 0)
        {
            m = &rs_data->management_address[i];
            break;
        }
    }

    return m;
}

static bool is_remote_system_data_existed(lldp_port_t* port, timeticks time_mark, uint32_t remote_index)
{
    bool ret = false;
    for (int i=0; i<MAX_REMOTE_SYSTEM_DATA_LEN; i++)
    {
        if (time_mark == port->remote_systems_data[i].time_mark && port->remote_systems_data[i].remote_index == remote_index)
        {
            ret = true;
            break;
        }
    }

    return ret;
}

static remote_systems_data_t* get_existed_remote_system_data(lldp_port_t* port, timeticks time_mark, uint32_t remote_index)
{
    remote_systems_data_t* r = NULL;
    for (int i=0; i<MAX_REMOTE_SYSTEM_DATA_LEN; i++)
    {
        if (time_mark == port->remote_systems_data[i].time_mark && port->remote_systems_data[i].remote_index == remote_index)
        {
            r = &port->remote_systems_data[i];
            break;
        }
    }

    return r;
}

static bool is_port_existed(yang_lldp_t* lldp, const char * if_name, uint8_t* mac)
{
    bool ret = false;
    for (int i=0; i<LLDP_MAX_LOCAL_INTERFACES; i++)
    {
        if ( (strcmp(if_name, lldp->ports[i].name)  == 0) && (memcmp(lldp->ports[i].dest_mac_address, mac, 6) == 0 ))
        {
            ret = true;
            break;
        }
    }

    return ret;
}

static lldp_port_t* get_existed_port(yang_lldp_t* lldp, const char * if_name, uint8_t* mac)
{
    lldp_port_t* p = NULL;
    for (int i=0; i<LLDP_MAX_LOCAL_INTERFACES; i++)
    {
        if ( (strcmp(if_name, lldp->ports[i].name)  == 0) && (memcmp(lldp->ports[i].dest_mac_address, mac, 6) == 0 ))
        {
            p = &lldp->ports[i];
            break;
        }
    }

    return p;
}

static void fill_remote_statistic_info(uint8_t k3, lldp_cfg_param_t* prm, yang_lldp_t* lldp)
{
    switch(k3)
    {
        case IEEE802_DOT1AB_LLDP_LAST_CHANGE_TIME:
            lldp->remote_statistic.last_change_time = *((uint32_t*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_REMOTE_INSERTS:
            lldp->remote_statistic.remote_inserts = *((uint32_t*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_REMOTE_DELETES:
            lldp->remote_statistic.remote_deletes= *((uint32_t*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_REMOTE_DROPS:
            lldp->remote_statistic.remote_drops = *((uint32_t*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_REMOTE_AGEOUTS:
            lldp->remote_statistic.remote_ageouts = *((uint32_t*)prm->value);
            break;
    }
}

static void fill_local_system_data(uint8_t k3, lldp_cfg_param_t* prm, yang_lldp_t* lldp)
{
    switch(k3)
    {
        case IEEE802_DOT1AB_LLDP_CHASSIS_ID:
            memcpy(lldp->local_system_data.chassis_id, (char*)prm->value, prm->vsize);
            break;
        case IEEE802_DOT1AB_LLDP_CHASSIS_ID_SUBTYPE:
            lldp->local_system_data.chassis_id_subtype = *((chassis_id_type_t*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_SYSTEM_NAME:
            memcpy(lldp->local_system_data.system_name, (char*)prm->value, prm->vsize);
            break;
        case IEEE802_DOT1AB_LLDP_SYSTEM_DESCRIPTION:
            memcpy(lldp->local_system_data.system_description, (char*)prm->value, prm->vsize);
            break;
        case IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_ENABLED:
            lldp->local_system_data.system_capabilities_enabled = *((uint16_t*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_SUPPORTED:
            lldp->local_system_data.system_capabilities_supported = *((uint16_t*)prm->value);
            break;
    }
}

static void fill_management_info(uint8_t* k, address_subtype_t addr_subtype, char* man_addr, lldp_cfg_param_t* prm, lldp_port_t* port)
{
    static int man_addr_idx = 0;
    if (!is_manangement_addr_tx_port_existed(port, addr_subtype, man_addr))
    {
        UB_LOG(UBL_INFO, "%s addr not existed. Added new man addr tx port \n", man_addr);
        memcpy(port->management_address_tx_port[man_addr_idx].man_address, man_addr, strlen(man_addr));
        port->management_address_tx_port[man_addr_idx].address_subtype =  addr_subtype;
        man_addr_idx++;
    }
    else
    {
        UB_LOG(UBL_INFO, "man addr %s existed\n", man_addr);
    }

    management_address_tx_port_t* man_addr_tx_port = get_existed_manament_addr_tx(port, addr_subtype, man_addr);
    if (man_addr_tx_port == NULL)
    {
        UB_LOG(UBL_INFO, "%s not existed. Doesn't make sense \n", man_addr);
    }
    else
    {
        switch (k[4])
        {
        case IEEE802_DOT1AB_LLDP_TX_ENABLE:
            man_addr_tx_port->tx_enable = *((bool*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_ADDR_LEN:
            man_addr_tx_port->addr_len = *((uint32_t*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_IF_SUBTYPE:
            man_addr_tx_port->if_subtype = *((man_addr_if_subtype_t*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_IF_ID:
            man_addr_tx_port->if_id = *((uint32_t*)prm->value);
            break;
        default:
            break;
        }
    }
}

static void fill_manament_addr(uint8_t* k, address_subtype_t addr_subtype, char* addr, lldp_cfg_param_t* prm, remote_systems_data_t* rs_data)
{
    static int addr_idx = 0;
    if (!is_manangement_addr_existed(rs_data, addr_subtype, addr))
    {
        UB_LOG(UBL_INFO, "%d addr subtype not existed. Added new man addr \n", addr_subtype);
        rs_data->management_address[addr_idx].address_subtype = addr_subtype;
        memcpy(rs_data->management_address[addr_idx].address,  addr, strlen(addr));
        addr_idx++;
    }
    else
    {
        UB_LOG(UBL_INFO, "man addr %d existed\n", addr_subtype);
    }

    management_address_t* mgr_addr = get_existed_manament_addr(rs_data, addr_subtype, addr);
    if (mgr_addr == NULL)
    {
        UB_LOG(UBL_INFO, "%d - %s not existed. Doesn't make sense \n", addr_subtype, addr);
    }
    else
    {
        switch(k[5])
        {
        case IEEE802_DOT1AB_LLDP_IF_SUBTYPE:
            mgr_addr->if_subtype = *((man_addr_if_subtype_t*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_IF_ID:
            mgr_addr->if_id = *((uint32_t*)prm->value);
            break;
        }
    }
}

static void fill_remote_unknown_tlv(uint8_t* k, uint32_t tlv_type, lldp_cfg_param_t* prm, remote_systems_data_t* rs_data)
{
    static int tlv_idx = 0;
    if (!is_remote_unknown_tlv_existed(rs_data, tlv_type))
    {
        UB_LOG(UBL_INFO, "%d addr subtype not existed. Added new tlv \n", tlv_type);
        rs_data->remote_unknown_tlv[tlv_idx].tlv_type = tlv_type;
        tlv_idx++;
    }
    else
    {
        UB_LOG(UBL_INFO, "tlv %d existed\n", tlv_type);
    }

    remote_unknown_tlv_t* remote_unknow_tlv = get_existed_remote_unknown_tlv(rs_data, tlv_type);
    switch(k[5])
    {
    case IEEE802_DOT1AB_LLDP_TLV_INFO:
        memcpy(remote_unknow_tlv->tlv_info, (uint8_t*)prm->value, prm->vsize);
        break;
    }
}

static void fill_remote_org_info(uint8_t* k, uint32_t info_id, uint32_t info_subtype, uint32_t info_index, lldp_cfg_param_t* prm, remote_systems_data_t* rs_data)
{
    static int org_idx = 0;
    if (!is_remote_org_defined_existed(rs_data, info_id, info_subtype, info_index))
    {
        UB_LOG(UBL_INFO, "%d - %d org info not existed. Added new  \n", info_id, info_index);
        rs_data->remote_org_defined_info[org_idx].info_identifier = info_id;
        rs_data->remote_org_defined_info[org_idx].info_index = info_index;
        org_idx++;
    }
    else
    {
        UB_LOG(UBL_INFO, "%d - %d org info existed  \n", info_id, info_index);
    }

    remote_org_defined_info_t* remote_org_info = get_existed_remote_org(rs_data, info_id, info_subtype, info_index);
    switch(k[5])
    {
    case IEEE802_DOT1AB_LLDP_REMOTE_INFO:
        memcpy(remote_org_info->remote_info, (uint8_t*)prm->value, prm->vsize);
        break;
    }

}

static void fill_remote_system_data(uint8_t* k, timeticks time_mark, uint32_t remote_index, lldp_cfg_param_t* prm, lldp_port_t* port)
{
    static int remote_system_data_idx = 0;
    remote_systems_data_t* remote_system_data;
    
    if ( !is_remote_system_data_existed(port, time_mark, remote_index) )
    {
        UB_LOG(UBL_INFO, "%d rs data not existed. Add new \n", remote_index);
        port->remote_systems_data[remote_system_data_idx].remote_index = remote_index;
        port->remote_systems_data[remote_system_data_idx].time_mark =  time_mark;
        remote_system_data_idx++;
    }
    else
    {
        UB_LOG(UBL_INFO, "%d rs data existed. \n", remote_index);
    }

    remote_system_data = get_existed_remote_system_data(port, time_mark, remote_index);
    if (remote_system_data == NULL)
    {
        UB_LOG(UBL_INFO, "%d rs data not existed. Doesn't make sense \n", remote_index);
    }
    else
    {
        switch (k[4])
        {
        case IEEE802_DOT1AB_LLDP_REMOTE_TOO_MANY_NEIGHBORS:
            remote_system_data->remote_too_many_neighbors = *((bool*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_REMOTE_CHANGES:
            remote_system_data->remote_changes = *((bool*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_PORT_ID_SUBTYPE:
            remote_system_data->port_id_subtype = *((port_id_type_t*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_PORT_ID:
            memcpy(remote_system_data->port_id, (char*)prm->value, strlen(prm->value));
            break;
        case IEEE802_DOT1AB_LLDP_PORT_DESC:
            memcpy(remote_system_data->port_desc, (char*)prm->value, strlen(prm->value));
            break;
        case IEEE802_DOT1AB_LLDP_CHASSIS_ID:
            memcpy(remote_system_data->chassis_id, (char*)prm->value, prm->vsize);
            break;
        case IEEE802_DOT1AB_LLDP_CHASSIS_ID_SUBTYPE:
            remote_system_data->chassis_id_subtype = *((chassis_id_type_t*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_SYSTEM_NAME:
            memcpy(remote_system_data->system_name, (char*)prm->value, prm->vsize);
            break;
        case IEEE802_DOT1AB_LLDP_SYSTEM_DESCRIPTION:
            memcpy(remote_system_data->system_description, (char*)prm->value, prm->vsize);
            break;
        case IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_ENABLED:
            remote_system_data->system_capabilities_enabled = *((uint16_t*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_SYSTEM_CAPABILITIES_SUPPORTED:
            remote_system_data->system_capabilities_supported = *((uint16_t*)prm->value);
            break;
        case IEEE802_DOT1AB_LLDP_MANAGEMENT_ADDRESS:
            {
                address_subtype_t addr_subtype = *((address_subtype_t*)prm->kvs[4]);
                char* addr = (char*)prm->kvs[5];
                fill_manament_addr(k, addr_subtype, addr, prm, remote_system_data);
            }
            break;
        case IEEE802_DOT1AB_LLDP_REMOTE_UNKNOWN_TLV:
            {
                uint32_t tlv_type = *((uint32_t*)prm->kvs[4]);
                fill_remote_unknown_tlv(k, tlv_type, prm, remote_system_data);
            }
            break;
        case IEEE802_DOT1AB_LLDP_REMOTE_ORG_DEFINED_INFO:
            {
                uint32_t info_id = *((uint32_t*)prm->kvs[4]);
                uint32_t info_subtype = *((uint32_t*)prm->kvs[5]);
                uint32_t info_index = *((uint32_t*)prm->kvs[6]);
                fill_remote_org_info(k, info_id, info_subtype, info_index, prm, remote_system_data);
            }
            break;
        default:
            break;
        }
    }
}

static void fill_port_info(uint8_t* k, char* if_name, uint8_t* mac, lldp_cfg_param_t* prm, yang_lldp_t* lldp)
{
    static int port_idx = 0;
    lldp_port_t* port;
    if (!is_port_existed(lldp, if_name, mac))
    {
        UB_LOG(UBL_INFO, "%s not existed. Added new lldp port \n", if_name);
        memcpy(lldp->ports[port_idx].name, if_name, strlen(if_name));
        memcpy(lldp->ports[port_idx].dest_mac_address, mac, 6);
        port_idx++;
    }
    else
    {
        UB_LOG(UBL_INFO, "%s existed\n", if_name);
    }

    port = get_existed_port(lldp, if_name, mac);
    if (port == NULL)
    {
        UB_LOG(UBL_INFO, "%s not existed. Doesn't make sense \n", if_name);
    }
    else
    {
        switch(k[3])
        {
            case IEEE802_DOT1AB_LLDP_ADMIN_STATUS:
                port->admin_status = *((admin_status_t*)prm->value);
                break;
            case IEEE802_DOT1AB_LLDP_NOTIFICATION_ENABLE:
                port->notification_enable = *((bool*)prm->value);
                break;
            case IEEE802_DOT1AB_LLDP_TLVS_TX_ENABLE:
                port->tlvs_tx_enable = *((uint8_t*)prm->value);
                break;
            case IEEE802_DOT1AB_LLDP_MESSAGE_FAST_TX:
                port->message_fast_tx = *((uint32_t*)prm->value);
                break;
            case IEEE802_DOT1AB_LLDP_MESSAGE_TX_HOLD_MULTIPLIER:
                port->message_tx_hold_multiplier = *((uint32_t*)prm->value);
                break;
            case IEEE802_DOT1AB_LLDP_MESSAGE_TX_INTERVAL:
                port->message_tx_interval = *((uint32_t*)prm->value);
                break;
            case IEEE802_DOT1AB_LLDP_REINIT_DELAY:
                port->reinit_delay = *((uint32_t*)prm->value);
                break;
            case IEEE802_DOT1AB_LLDP_TX_CREDIT_MAX:
                port->tx_credit_max = *((uint32_t*)prm->value);
                break;
            case IEEE802_DOT1AB_LLDP_TX_FAST_INIT:
                port->tx_fast_init = *((uint32_t*)prm->value);
                break;
            case IEEE802_DOT1AB_LLDP_NOTIFICATION_INTERVAL:
                port->notification_interval = *((uint32_t*)prm->value);
                break;
            case IEEE802_DOT1AB_LLDP_MANAGEMENT_ADDRESS_TX_PORT:
                char* man_addr = (char*)prm->kvs[3];
                address_subtype_t addr_subtype = *((address_subtype_t*) prm->kvs[2]);
                UB_LOG(UBL_INFO, "IP [%s] subtype: %d\n",man_addr, addr_subtype);
                fill_management_info(k, addr_subtype, man_addr, prm, port);
                break;
            case IEEE802_DOT1AB_LLDP_PORT_ID_SUBTYPE:
                port->port_id_subtype = *((port_id_type_t*)prm->value);
                break;
            case IEEE802_DOT1AB_LLDP_PORT_ID:
                memcpy(port->port_id, (char*)prm->value, strlen(prm->value));
                break;
            case IEEE802_DOT1AB_LLDP_PORT_DESC:
                memcpy(port->port_desc, (char*)prm->value, strlen(prm->value));
                break;
            case IEEE802_DOT1AB_LLDP_TX_STATISTICS:
                switch(k[4])
                {
                    case IEEE802_DOT1AB_LLDP_TOTAL_FRAMES:
                        port->tx_statistic.total_frames = *((uint32_t*)prm->value);
                        break;
                    case IEEE802_DOT1AB_LLDP_TOTAL_LENGTH_ERRORS:
                        port->tx_statistic.total_length_errors = *((uint32_t*)prm->value);
                        break;
                }
                break;
            case IEEE802_DOT1AB_LLDP_RX_STATISTICS:
                switch(k[4])
                {
                    case IEEE802_DOT1AB_LLDP_TOTAL_AGEOUTS:
                        port->rx_statistic.total_ageouts = *((uint32_t*)prm->value);
                        break;
                    case IEEE802_DOT1AB_LLDP_TOTAL_DISCARDED_FRAMES:
                        port->rx_statistic.total_discarded_frames = *((uint32_t*)prm->value);
                        break;
                    case IEEE802_DOT1AB_LLDP_ERROR_FRAMES:
                        port->rx_statistic.error_frames = *((uint32_t*)prm->value);
                        break;
                    case IEEE802_DOT1AB_LLDP_TOTAL_DISCARDED_TLVS:
                        port->rx_statistic.total_discarded_tlvs = *((uint32_t*)prm->value);
                        break;
                    case IEEE802_DOT1AB_LLDP_TOTAL_UNRECOGNIZED_TLVS:
                        port->rx_statistic.total_unrecognized_tlvs = *((uint32_t*)prm->value);
                        break;
                }
                break;
            case IEEE802_DOT1AB_LLDP_REMOTE_SYSTEMS_DATA:
                timeticks time_mark = *((timeticks*)prm->kvs[2]);
                uint32_t remote_index = *((uint32_t*)prm->kvs[3]);
                UB_LOG(UBL_INFO, "REMOTE_SYSTEMS_DATA [%d-%d]\n",time_mark, remote_index);
                fill_remote_system_data(k, time_mark, remote_index, prm, port);
                break;
        }
    }
}

static void fill_lldp_config_info(void *key, yang_lldp_t* lldp, lldp_cfg_param_t* prm)
{
    if (key)
    {
        if( ( ((uint8_t*)key)[0] == IEEE802_DOT1AB_LLDP_RW || ((uint8_t*)key)[0] == IEEE802_DOT1AB_LLDP_RO)  
            && ((uint8_t*)key)[1] == IEEE802_DOT1AB_LLDP_LLDP )
        {
            // UB_LOG(UBL_INFO, "%s: RW key[2] %s\n", __func__, ieee802_dot1ab_lldp_get_string(((uint8_t*)key)[2]));
            switch (((uint8_t*)key)[2])
            {
                case IEEE802_DOT1AB_LLDP_MESSAGE_FAST_TX:
                    lldp->message_fast_tx = *((uint32_t*)prm->value);
                    break;
                case IEEE802_DOT1AB_LLDP_MESSAGE_TX_HOLD_MULTIPLIER:
                    lldp->message_tx_hold_multiplier = *((uint32_t*)prm->value);
                    break;
                case IEEE802_DOT1AB_LLDP_MESSAGE_TX_INTERVAL:
                    lldp->message_tx_interval = *((uint32_t*)prm->value);
                    break;
                case IEEE802_DOT1AB_LLDP_REINIT_DELAY:
                    lldp->reinit_delay = *((uint32_t*)prm->value);
                    break;
                case IEEE802_DOT1AB_LLDP_TX_CREDIT_MAX:
                    lldp->tx_credit_max = *((uint32_t*)prm->value);
                    break;
                case IEEE802_DOT1AB_LLDP_TX_FAST_INIT:
                    lldp->tx_fast_init = *((uint32_t*)prm->value);
                    break;
                case IEEE802_DOT1AB_LLDP_NOTIFICATION_INTERVAL:
                    lldp->notification_interval = *((uint32_t*)prm->value);
                    break;
                case IEEE802_DOT1AB_LLDP_REMOTE_STATISTICS:
                    fill_remote_statistic_info(((uint8_t*)key)[3], prm, lldp);
                    break;
                case IEEE802_DOT1AB_LLDP_LOCAL_SYSTEM_DATA:
                    fill_local_system_data(((uint8_t*)key)[3], prm, lldp);
                    break;
                case IEEE802_DOT1AB_LLDP_PORT:
                {
                    ub_macaddr_t mac = {0};
                    char* if_name = (char*)prm->kvs[0];
                    memcpy(mac, (uint8_t*)prm->kvs[1], prm->kss[1]);
                    UB_LOG(UBL_INFO, "[%s]MAC %x %x %x %x %x %x\n",if_name, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                    fill_port_info(key, if_name, mac, prm, lldp);
                }
                break;
            }
        }
    }
}

int ydbi_load_lldp_db(yang_db_item_access_t *ydbia, yang_lldp_t* lldp)
{
    uint8_t key1[1]={XL4_EXTMOD_RW};
    uint8_t key2[1]={YANG_MODULES_ENUM_END-1};
    lldp_cfg_param_t prm;
    uc_range *range=NULL;
    void *key=NULL;
    uint32_t ksize;
    int ret=0;
    memset(lldp, 0, sizeof(yang_lldp_t));

    range=uc_get_range(ydbia->dbald, key1, 1, key2, 1);
    while(true){
        memset(&prm, 0, sizeof(prm));
        if(uc_get_key_in_range(ydbia->dbald, range, &key, &ksize, UC_DBAL_NOMOVE)){
            break;
        }
		// UB_LOG(UBL_INFO, "%s ksize %d \n", (char*)key, ksize);
        if(uc_get_value_in_range(ydbia->dbald, range, &prm.value, &prm.vsize, UC_DBAL_FORWARD)){
            break;
        }

        if(yang_db_extract_key(key, ksize, &prm.aps, prm.kvs, prm.kss)!=0){
            UB_SD_RELMEM(YANGINIT_GEN_SMEM, prm.rstr);
            continue;
        }

        fill_lldp_config_info(key, lldp, &prm);

        yang_db_extract_key_free(prm.aps, prm.kvs, prm.kss);
        UB_SD_RELMEM(YANGINIT_GEN_SMEM, prm.rstr);
    }
    uc_get_range_release(ydbia->dbald, range);
    uc_dbal_releasedb(ydbia->dbald);
    return ret;
}

int ydbi_get_item_abk1vk0(yang_db_item_access_t *ydbia, void **rval, uint8_t k1,  bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k1vk0(&ydbia->dbpara, k1, status);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}

int ydbi_get_item_abk2vk0(yang_db_item_access_t *ydbia, void **rval, uint8_t k1, uint8_t k2, bool status)
{
	if(ydbi_get_head(ydbia, __func__)!=0){return -1;}
	set_dpara_k2vk0(&ydbia->dbpara, k1, k2, status);
	return ydbi_get_foot(ydbia, __func__, rval, UBL_INFO);
}