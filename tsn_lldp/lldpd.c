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
#include "lldpd.h"
#include <tsn_unibase/unibase.h>
#include <tsn_unibase/unibase_binding.h>
#include <tsn_combase/cb_ethernet.h>
#include "lldp_yangdb.h"
#include "interface_mgr.h"
#include "lldp_utils.h"

hw_interface_list hw_if_lst;
yang_lldp_t yang_lldp_cfg;

static void init_default_cfg()
{
	memset(&yang_lldp_cfg, 0, sizeof(yang_lldp_t));
	ub_list_init(&yang_lldp_cfg.ports);
}

/// @brief The function should be called init, to indicate which uniconf-access mode we should use
/// @param mode 
void lldpd_uniconf_access_mode(uint8_t mode)
{
	ydb_set_uniconf_access_mode(mode);
}

int lldpd_init(const char* lldp_db_name, int* vlanid, netdevname_t *netdevs, int ndev_size)
{
	int ret = -1;
	// Init default params
	init_default_cfg();
	ub_list_init(&hw_if_lst);
	init_timer_manager();
	UB_LOG(UBL_INFO, "%s - DB [%s] \n", __func__, lldp_db_name);
	// Load LLDP yang db into mem
	ret=initialize_cfg(lldp_db_name, &yang_lldp_cfg, true);
	if (ret == 0)
	{
		// Open raw socket in all netdevs
		if (init_socket_utils(netdevs, ndev_size) != 0)
		{
			return -1;
		}
		else
		{
			// Create list of hw_if based on yangdb cfg_port* list
			init_local_interfaces(&hw_if_lst, &yang_lldp_cfg);

			// Map socket fd to hw_if. 1 socket fd can be map to 3 hw_if due to 3 valid dest-mac-addr per one port
			ret = init_raw_sock(&hw_if_lst);
			if (ret == 0)
			{
				start_db_mon_timer();
			}
		}
	}
	return ret;
}

void lldpd_run(uint8_t* terminated)
{
	lldpd_start_process(&hw_if_lst, terminated);
}

void lldpd_deinit()
{
	stop_db_mon_timer();
	deinit_db();
	cleanup_cfg(&yang_lldp_cfg);
	terminate_all(&hw_if_lst);
	deinit_socket_utils();
}