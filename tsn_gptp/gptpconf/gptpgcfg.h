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
#ifndef GPTPGCFG_H_
#define GPTPGCFG_H_

#include "xl4-extmod-xl4gptp.h"
#include "xl4-extmod-xl4gptp_runconf.h"
#include "tsn_uniconf/yangs/ieee1588-ptp.h"
#include "../gptpnet.h"

typedef struct gptpgcfg_data gptpgcfg_data_t;

#define XL4GPTP_MODULE "xl4gptp"


/****************************************
 * layer independent functions
 ****************************************/

int ydbi_get_item_nyptk1vk0(yang_db_item_access_t *ydbia, void **rval, uint8_t instIndex,
			    uint8_t k1, bool status);

int ydbi_rel_item_nyptk1vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			    uint8_t k1, bool status);

int ydbi_set_item_nyptk1vk0(yang_db_item_access_t *ydbia, uint8_t instIndex,
			    uint8_t k1, bool status, void *value, uint32_t vsize,
			    bool notice);

void* gptpgcfg_get_item(uint8_t gptpInstanceIndex, uint8_t confitem, bool status);

int gptpgcfg_get_item_release(uint8_t gptpInstanceIndex);

int gptpgcfg_get_intitem(uint8_t gptpInstanceIndex, uint8_t confitem, bool status);

int gptpgcfg_set_item(uint8_t gptpInstanceIndex, uint8_t confitem,
		      bool status, void *value, uint32_t vsize);

/**
 * @brief wait until gptp2d sets port0 PORT_STATE
 * @return 0:got PORT_SATE, 1:timed out
 */
int gptpgcfg_wait_gptpready(yang_db_item_access_t *ydbia, uint8_t gptpInstance, int tout_ms);

/**
 * @brief copy instance data
 */
int gptpcfg_copy_instance(uint8_t sginst, uint8_t sdomain, uint8_t dginst, uint8_t ddomain);


/****************************************
 * layer private functions
 ****************************************/
#if defined(GPTP_PRIVATE_H_) || defined(GPTP_STATIC_MEMORY_H_) || defined(LLD_GPTP_PRIVATE_H_)

int gptpgcfg_init(const char *dbname, const char **confnames,
			 uint8_t gptpInstanceIndex, bool ucthread);

void gptpgcfg_close(uint8_t gptpInstanceIndex);

int gptpgcfg_get_yang_intitem(uint8_t gptpInstanceIndex,
			      uint8_t confitem0, uint8_t confitem1, uint8_t confitem2,
			      uint8_t domainIndex, bool status);

int gptpgcfg_get_yang_portds_intitem(uint8_t gptpInstanceIndex, uint8_t confitem,
				     uint16_t pindex, uint8_t domainIndex, bool status);

int64_t gptpgcfg_get_yang_portds_int64item(uint8_t gptpInstanceIndex, uint8_t confitem,
					   uint16_t pindex, uint8_t domainIndex, bool status);

int gptpgcfg_set_yang_port_item(uint8_t gptpInstanceIndex, uint8_t confitem1, uint8_t confitem2,
				uint16_t pindex, uint8_t domainIndex, bool status,
				void *value, int vsize, bool notice);

int gptpgcfg_set_clock_state_item(uint8_t gptpInstanceIndex, uint8_t confitem,
				  uint8_t domainIndex, bool status,
				  void *value, int vsize, bool notice);

void gptpgcfg_releasedb(uint8_t gptpInstanceIndex);

int gptpgcfg_set_netdevs(uint8_t gptpInstanceIndex, const char *netdevs[], int numdevs);
void gptpgcfg_remove_netdevs(uint8_t gptpInstanceIndex);
int gptpgcfg_link_check(uint8_t gptpInstanceIndex, gptpnet_data_netlink_t *edtn1);

int gptpgcfg_set_asCapable(uint8_t gptpInstanceIndex, uint8_t domainIndex,
			   uint16_t portIndex, bool asCapable);

#endif // defined(GPTP_PRIVATE_H_) || defined(GPTP_STATIC_MEMORY_H_)

#endif
