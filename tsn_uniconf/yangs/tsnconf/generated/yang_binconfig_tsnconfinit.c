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
#include "../../../uc_dbal.h"
#include "../../../hal/uc_hwal.h"
#include "ieee802-dot1ab-lldp.h"
#include "ieee802-dot1ab-lldp_runconf.h"
#include "ieee802-dot1q-cnc-config.h"
#include "ieee802-dot1q-cnc-config_runconf.h"
#include "excelfore-config-uni.h"
#include "excelfore-config-uni_runconf.h"
#include "ieee802-dot1cb-frer.h"
#include "ieee802-dot1cb-frer_runconf.h"
#include "ieee802-dot1cb-stream-identification.h"
#include "ieee802-dot1cb-stream-identification_runconf.h"
// YANG_CONFIG_INIT
int yang_binconfig_tsnconfinit(uc_dbald *dbald, uc_hwald *hwald)
{
	int res=0;
	res|=ieee802_dot1ab_lldp_runconf_config_init(dbald, hwald);
	res|=ieee802_dot1q_cnc_config_runconf_config_init(dbald, hwald);
	res|=excelfore_config_uni_runconf_config_init(dbald, hwald);
	res|=ieee802_dot1cb_frer_runconf_config_init(dbald, hwald);
	res|=ieee802_dot1cb_stream_identification_runconf_config_init(dbald, hwald);
	// MODULES_INIT_DONE
	return res;
}

int yang_nconf_config_tsnconfinit(uc_dbald *dbald)
{
	int res=0;
	// NCONF_INIT_DONE
	return res;
}
