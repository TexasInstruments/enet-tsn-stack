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
#include "../../uc_dbal.h"
#include "../../hal/uc_hwal.h"
#include "ietf-interfaces.h"
#include "ieee1588-ptp-tt.h"
#include "ieee802-dot1q-bridge.h"
#include "ieee802-dot1ab-lldp.h"
#include "ieee802-dot1q-tsn-config-uni.h"
#include "excelfore-tsn-remote.h"
#include "excelfore-netconf-server.h"
#include "ietf-netconf-monitoring.h"
#include "ietf-yang-library.h"
#include "ietf-interfaces_runconf.h"
#include "ieee1588-ptp-tt_runconf.h"
#include "ieee802-dot1q-bridge_runconf.h"
#include "ieee802-dot1ab-lldp_runconf.h"
#include "ieee802-dot1q-tsn-config-uni_runconf.h"
#include "excelfore-tsn-remote_runconf.h"
#include "excelfore-netconf-server_runconf.h"
#include "ietf-netconf-monitoring_runconf.h"
#include "ietf-yang-library_runconf.h"
#include "ietf-interfaces_nconf.h"
#include "ieee1588-ptp-tt_nconf.h"
#include "ieee802-dot1q-bridge_nconf.h"
#include "ieee802-dot1ab-lldp_nconf.h"
#include "ieee802-dot1q-tsn-config-uni_nconf.h"
#include "excelfore-tsn-remote_nconf.h"
#include "excelfore-netconf-server_nconf.h"
#include "ietf-netconf-monitoring_nconf.h"
#include "ietf-yang-library_nconf.h"
// YANG_CONFIG_INIT
int yang_config_init(uc_dbald *dbald, uc_hwald *hwald)
{
	int res=0;
	res|=ietf_interfaces_config_init(dbald, hwald);
	res|=ieee1588_ptp_tt_config_init(dbald, hwald);
	res|=ieee802_dot1q_bridge_config_init(dbald, hwald);
	res|=ieee802_dot1ab_lldp_config_init(dbald, hwald);
	res|=ieee802_dot1q_tsn_config_uni_config_init(dbald, hwald);
	res|=excelfore_tsn_remote_config_init(dbald, hwald);
	res|=excelfore_netconf_server_config_init(dbald, hwald);
	res|=ietf_netconf_monitoring_config_init(dbald, hwald);
	res|=ietf_yang_library_config_init(dbald, hwald);
	res|=ietf_interfaces_runconf_config_init(dbald, hwald);
	res|=ieee1588_ptp_tt_runconf_config_init(dbald, hwald);
	res|=ieee802_dot1q_bridge_runconf_config_init(dbald, hwald);
	res|=ieee802_dot1ab_lldp_runconf_config_init(dbald, hwald);
	res|=ieee802_dot1q_tsn_config_uni_runconf_config_init(dbald, hwald);
	res|=excelfore_tsn_remote_runconf_config_init(dbald, hwald);
	res|=excelfore_netconf_server_runconf_config_init(dbald, hwald);
	res|=ietf_netconf_monitoring_runconf_config_init(dbald, hwald);
	res|=ietf_yang_library_runconf_config_init(dbald, hwald);
	// MODULES_INIT_DONE
	return res;
}

int yang_nconf_config_init(uc_dbald *dbald)
{
	int res=0;
	res|=ietf_interfaces_nconf_config_init(dbald);
	res|=ieee1588_ptp_tt_nconf_config_init(dbald);
	res|=ieee802_dot1q_bridge_nconf_config_init(dbald);
	res|=ieee802_dot1ab_lldp_nconf_config_init(dbald);
	res|=ieee802_dot1q_tsn_config_uni_nconf_config_init(dbald);
	res|=excelfore_tsn_remote_nconf_config_init(dbald);
	res|=excelfore_netconf_server_nconf_config_init(dbald);
	res|=ietf_netconf_monitoring_nconf_config_init(dbald);
	res|=ietf_yang_library_nconf_config_init(dbald);
	// NCONF_INIT_DONE
	return res;
}
