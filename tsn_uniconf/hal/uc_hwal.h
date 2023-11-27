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
#ifndef UC_HWAL_H_
#define UC_HWAL_H_

/*
 * the interfaces of this hardware abstraction layer
 * to the upper layer
 */
#include <inttypes.h>
#include <stdbool.h>
#include <tsn_combase/cb_thread.h>
#include "../uc_dbal.h"

typedef struct uc_hwald uc_hwald;

typedef struct uc_notice_data uc_notice_data_t;

enum {
	UC_HWAL_CBS_DISABLE=0,
	UC_HWAL_CBS_ENABLE,
	UC_HWAL_CBS_DISABLE_COMPLETE,
	UC_HWAL_CBS_ENABLE_COMPLETE,
};

uc_hwald *uc_hwal_open(uc_dbald *dbald);

void uc_hwal_close(uc_hwald *hwald);

/*
 * This fuction is called when the item is newly set in the DB
 */
int uc_hwal_reghw(uc_hwald *hwald, uint8_t *aps, void **kvs, uint8_t *kss,
		  void *value, uint32_t vsize);
/*
 * This fuction is called when the item is removed from the DB
 */
int uc_hwal_dereghw(uc_hwald *hwald, uint8_t *aps, void **kvs, uint8_t *kss);

/*
 * This fuction is called when the item is updated in the DB
 */
int uc_hwal_writehw(uc_hwald *hwald, uint8_t *aps, void **kvs, uint8_t *kss,
		    void *value, uint32_t vsize);


/*
 * srat a thread to catch events
 */
int uc_hwal_catch_events_thread(uc_hwald *hwald, CB_SEM_T *sigp);

/*
 * this function wait HW events(network events) for tout_ms time.
 * Detecting the updated status, update the DB.
 * and call 'uc_nu_putnotice_push' to notice the update to uc_client side.
 */
int uc_hwal_detect_notice(uc_hwald *hwald, uc_notice_data_t *ucntd);

#endif
