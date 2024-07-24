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
#ifndef MRPGCFG_H_
#define MRPGCFG_H_

#include <tsn_combase/combase.h>
#include "xl4-extmod-xl4mrp.h"
#include "xl4-extmod-xl4mrp_runconf.h"
#include "../mrp_datatypes.h"

typedef struct mrpgcfg_data mrpgcfgd_t;

#define XL4MRP_MODULE "xl4mrp"

#define XL4MRP_DEFAULT_JOIN_TIME 200
#define XL4MRP_DEFAULT_LEAVE_TIME 800

mrpgcfgd_t *mrpgcfg_init(uint8_t instanceIndex,
			 const char *dbname, const char **confnames,
			 bool ucthread, bool mrpman);

void mrpgcfg_close(mrpgcfgd_t *mgcd);

uint8_t mrpgcfg_instanceIndex(mrpgcfgd_t *mgcd);

bool mrpgcfg_threadmode(mrpgcfgd_t *mgcd);

void mrpgcfg_releasedb(mrpgcfgd_t *mgcd);

int mrpgcfg_set_asCapable_ucnotice(uint8_t instanceIndex, uint16_t portIndex,
				   UC_NOTICE_SIG_T **sem);
int mrpgcfg_clear_asCapable_ucnotice(uint8_t instanceIndex);
int mrpgcfg_get_asCapable_ucnotice(uint8_t instanceIndex, uint8_t *portIndex, bool *asCapable);

int mrpgcfg_set_ifupdown_ucnotice(uint8_t instanceIndex, const char *netdev, UC_NOTICE_SIG_T **sem);
int mrpgcfg_clear_ifupdown_ucnotice(uint8_t instanceIndex);
int mrpgcfg_get_ifupdown_ucnotice(uint8_t instanceIndex, char *netdev, uint8_t *ifupdown);

int mrpgcfg_set_rstp_ucnotice(uint8_t instanceIndex, uint8_t portIndex, UC_NOTICE_SIG_T **sem);
int mrpgcfg_clear_rstp_ucnotice(uint8_t instanceIndex);
int mrpgcfg_get_rstp_ucnotice(uint8_t instanceIndex, uint8_t *portIndex, uint8_t *update);

/**
 * @brief cons instance data from sinst to dinst
 * @return 0:success, -1:error
 * @note ydbi_access_init must be called before calling this fuction.
 */
int mrpgcfg_copy_instance(uint8_t sinst, uint8_t dsinst);

/**
 * @brief get MRP exteranl control status
 * @return 0:exteranl control disabled, 1:exteranl control enabled
 * @note when there is no data in the DB, it is treated as disabled status
 */
int ydbi_mrp_get_external_control(uint8_t instanceIndex, const char *netdev,
				  protocol_index_t protoi);

/**
 * @brief set MRP exteranl control status
 * @param extc 0:disable exteranl control, 1:enable exteranl control
 */
int ydbi_mrp_set_external_control(uint8_t instanceIndex, const char *netdev,
				  protocol_index_t protoi, uint8_t extc);

/**
 * @brief check a request notice, and receive the request
 * @return 0:got the notice and update the parameters, -1: no update
 */
int ydbi_xmrpd_receive_extupdate(mrpgcfgd_t *mgcd, protocol_index_t *protoi,
				 netdevname_t devname, int tout_ms);

#define MRP_EXT_SUCCESS 1
#define MRP_EXT_FAILURE 2
/**
 * @brief clear the requested notice, and send the return code
 * @param rescode 1:success to process the request, 2:failure
 * @return 0:okay, -1:error
 */
int ydbi_xmrpd_clear_received_extupdate(mrpgcfgd_t *mgcd, uint8_t rescode);

/**
 * @brief send a request to xmrpd
 * @return 1:busy, the other request in the process, 0:okay, -1:error
 */
int ydbi_mrpext_request_update(mrpgcfgd_t *mgcd, protocol_index_t protoi,
			       netdevname_t devname);

/**
 * @brief check thre return code from xmrpd
 * @return 1:no result, 0:received the result , -1:error
 */
int ydbi_mrpext_get_updateresult(mrpgcfgd_t *mgcd, uint8_t *rescode, int tout_ms);

/**
 * @note access non-yang data, one-key items
 */
int ydbi_get_item_nymrk1vk0(yang_db_item_access_t *ydbia, void **rval, uint8_t instIndex,
			    uint8_t k1, bool status);

int ydbi_set_item_nymrk1vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			    uint8_t k1, bool status, void *value, uint32_t vsize, bool notice);

int ydbi_rel_item_nymrk1vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			    uint8_t k1, bool status);

/**
 * @note access non-yang data, two-key items
 */
int ydbi_get_item_nymrk2vk0(yang_db_item_access_t *ydbia, void **rval, uint8_t instIndex,
			    uint8_t k1, uint8_t k2,
			    uint8_t *pindex, uint8_t *tcindex, bool status);

int ydbi_set_item_nymrk2vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			    uint8_t k1, uint8_t k2,
			    uint8_t *pindex, uint8_t *tcindex, bool status,
			    void *value, uint32_t vsize, bool notice);

int ydbi_rel_item_nymrk2vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			    uint8_t k1, uint8_t k2,
			    uint8_t *pindex, uint8_t *tcindex, bool status);
/**
 * @note access non-yang data, three-key items
 *       set action may need HW config.
 */
int ydbi_get_item_nymrk3vk0(yang_db_item_access_t *ydbia, void **rval, uint8_t instIndex,
			    uint8_t k1, uint8_t k2, uint8_t k3,
			    uint8_t *pindex, uint8_t *tcindex, bool status);

int ydbi_set_item_nymrk3vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			    uint8_t k1, uint8_t k2, uint8_t k3,
			    uint8_t *pindex, uint8_t *tcindex, bool status,
			    void *value, uint32_t vsize, bool notice, uint8_t onhw);

int ydbi_rel_item_nymrk3vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			    uint8_t k1, uint8_t k2, uint8_t k3,
			    uint8_t *pindex, uint8_t *tcindex, bool status);

#endif
