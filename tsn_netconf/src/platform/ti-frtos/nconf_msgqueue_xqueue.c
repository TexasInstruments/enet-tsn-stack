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
/**
 * @file        nconf_msgqueue.c
 *
 * @brief       Netconf Message Queue Implemenation using
 *              FreeRTOS xQueue
 */

/*=============================================================================
 * Include Files
 *============================================================================*/

#include <FreeRTOS.h>
#include <string.h>
#include <queue.h>
#include "nconf_msgqueue.h"

/*=============================================================================
 * Typedefs, Structures, and Enums
 *============================================================================*/

struct _msgq_hdl {
    QueueHandle_t msgq_id;
};

/*=============================================================================
 * Macros and Constants
 *============================================================================*/

#define NCONF_MSGQ_DEFAULT_QUEUE_NUM    (10U)

/*=============================================================================
 * Global Variables
 *============================================================================*/

UB_SD_GETMEM_DEF(NCONF_MSGQ_HANDLE, sizeof(struct _msgq_hdl), 1);

/*=============================================================================
 * Function Definitions
 *============================================================================*/

int nconf_msgq_init(nconf_msgq_t *msgq, uint32_t maxmsg, uint32_t maxsize)
{
    struct _msgq_hdl *new_msgq=NULL;
    UBaseType_t queueLength=(maxmsg>0) ? maxmsg : NCONF_MSGQ_DEFAULT_QUEUE_NUM;

    new_msgq=(struct _msgq_hdl *)
        UB_SD_GETMEM(NCONF_MSGQ_HANDLE, sizeof(struct _msgq_hdl));
    if(NULL==new_msgq) {
        UB_LOG(UBL_ERROR, "%s:Failed to allocate new message queue\n", __func__);
        return -1;
    }
    memset(new_msgq, 0, sizeof(struct _msgq_hdl));

    new_msgq->msgq_id=xQueueCreate(queueLength, (UBaseType_t)maxsize);
    if(new_msgq->msgq_id==NULL) {
        UB_LOG(UBL_ERROR, "%s:xQueueCreate() failed\n", __func__);
        return -1;
    }

    *msgq=new_msgq;
    return 0;
}


int nconf_msgq_recv(nconf_msgq_t msgq, void *buf, size_t buf_size)
{
    BaseType_t qret=pdFALSE;
    int ret=0;
    NCONF_UNUSED(buf_size);
    qret=xQueueReceive(msgq->msgq_id, buf, portMAX_DELAY);
    if (pdTRUE!=qret) {
        UB_LOG(UBL_ERROR, "%s:xQueueReceive() failed\n", __func__);
        ret=-1;
    }
    return ret;
}

int nconf_msgq_send(nconf_msgq_t msgq, void *buf, size_t buf_size)
{
    BaseType_t qret=pdFALSE;
    int ret=0;
    NCONF_UNUSED(buf_size);
    qret=xQueueSendToBack(msgq->msgq_id, buf, portMAX_DELAY);
    if (pdTRUE!=qret) {
        UB_LOG(UBL_ERROR, "%s:xQueueSendToBack() failed\n", __func__);
        ret=-1;
    }
    return ret;
}

void nconf_msgq_deinit(nconf_msgq_t *msgq)
{
    struct _msgq_hdl *tmp=*msgq;
    if (NULL != tmp) {
        vQueueDelete(tmp->msgq_id);
        UB_SD_RELMEM(NCONF_MSGQ_HANDLE, tmp);
        *msgq=NULL;
    } else {
        UB_LOG(UBL_DEBUG, "%s:msgq handle not yet initialized\n", __func__);
    }
}

