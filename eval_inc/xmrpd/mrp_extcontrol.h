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
#ifndef MRP_EXTCONTROL_H_
#define MRP_EXTCONTROL_H_

#include "mrp_datatypes.h"

typedef struct mrp_extc_dlist{
	uint8_t ext_ctrl;
	uint8_t rescode;
	uint8_t printon;
	ub_esarray_cstd_t *msrp_ta;
	ub_esarray_cstd_t *msrp_tf;
	ub_esarray_cstd_t *msrp_ls;
	ub_esarray_cstd_t *msrp_dm;
	ub_esarray_cstd_t *mvrp;
}mrp_extc_dlist_t;

#define MRP_EXTC_CMD_READ 0
#define MRP_EXTC_CMD_WRITE 1
#define MRP_EXTC_CMD_DELETE 2
#define MRP_EXTC_CMD_UPDATE 3

typedef struct mrp_extc_cat{
	ub_bytearray8_t sid; //0
	ub_bytearray6_t da; //1
	uint16_t vid; //2
	uint16_t mfs; //3
	uint16_t mif; //4
	uint8_t pcp; //5
	uint8_t rank; //6
	uint32_t acl; //7
	ub_bytearray8_t fid; //8
	uint8_t fcode; //9
	uint8_t scid; //10
	uint8_t scpri; //11
	uint8_t fpe; // 12
}mrp_extc_cat_t;

#define SID_INDEX 0
#define VID_INDEX 2
#define SCID_INDEX 10
#define SCPRI_INDEX 11
#define FPE_INDEX 12

typedef struct mrp_extc_cmdele{
	int cmd_rwd;
	netdevname_t devname;
	uint32_t proto; // protocol_index_t + 1
	char cmdc;
	int8_t ext_ctrl; // -1:no action, 0:off, 1:on
	msrp_attributetype_t attrt;
	bool pset[13]; // flags to set fields of 'mcat'
	mrp_extc_cat_t mcat;
	char *resultstr;
}mrp_extc_cmdele_t;

typedef struct msr_listener_data{
	ub_bytearray8_t stream_id;
	uint8_t fpe;
}msr_listener_data_t;

/**
 * @brief initialize the data list to read from 'excelfore-tsn-remote'
 * @return 0:success
 */
int mrp_extc_dlist_init(mrp_extc_dlist_t *addrdl);

/**
 * @brief close the data list
 */
void mrp_extc_dlist_close(mrp_extc_dlist_t *addrdl);

/**
 * @brief update 'cele' data from 'dlistd'
 */
int mrp_extc_update_cele(mrp_extc_cmdele_t *cele, protocol_index_t protoIndex, void *dlistd);

/**
 * @brief read/write mrp external control data
 * @return 0:success, -1:error
 * @param addrdl
 *	  for read, the result comes to this list. addrdl can't be NULL.
 *	  for delete, the deleted data is removed from the list.
 *	  for write/delete it can be NULL.
 */
int mrp_extc_rw_proc(uint8_t instIndex, mrp_extc_cmdele_t *cele,
		     mrp_extc_dlist_t *addrdl);

/**
 * @brief process a command string, for the read command the result comes in addrdl
 * @return 0:success, 1:no data to read, -1:error
 */
int mrp_extc_proc_cmd(mrpgcfgd_t *mrpgd, char *cmdstr, mrp_extc_dlist_t *addrdl);

/**
 * @brief  wait xmprd becomes ready
 * @return 0:success, 1:timed out, -1:error
 */
int mrp_extc_wait_xmrpd(mrpgcfgd_t *mrpgd, uint8_t instIndex, int tout_ms);

/**
 * @brief  delete data in OperRequestlist and IndicationList
 * @param  init_admin	false:don't initialize AdminRequestlist(normal usage),
 *                      true: initialize AdminRequestlist
 * @param  devname	NULL:all devices, otherwise the device name
 * @return 0:success, -1:error
 */
int mrp_extc_initialize_list(uc_dbald *dbald, uint8_t instIndex,
			     bool init_admin, const char *devname);


#endif
