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
 * @file        nconf_fsutils.c
 *
 * @brief       Netconf FileSystem Utility Implementation in
 *              TI FreeRTOS (Sitara)
 */

/*=============================================================================
 * Include Files
 *============================================================================*/

#if !defined(DISABLE_FAT_FS)
#include <FreeRTOS.h>
#include <ff_mmcsd.h>
#include <ff_stdio.h>
#endif //!DISABLE_FAT_FS
#include "nconf_fsutils.h"

/*=============================================================================
 * Macros and Constants
 *============================================================================*/

#if !defined(DISABLE_FAT_FS)
#define NCONF_FILE      FF_FILE
#define NCONF_FOPEN     ff_fopen
#define NCONF_FREAD     ff_fread
#define NCONF_FCLOSE    ff_fclose
#else
#define NCONF_FILE      FILE
#define NCONF_FOPEN     fopen
#define NCONF_FREAD     fread
#define NCONF_FCLOSE    fclose
#endif //!DISABLE_FAT_FS

/*=============================================================================
 * Function Definitions
 *============================================================================*/

nconf_fshdl_t nconf_fopen(const char *pathname, char *mode)
{
    NCONF_FILE *file=NCONF_FOPEN(pathname, mode);
    if (NULL==file) {
        UB_LOG(UBL_ERROR, "%s:failed to open [%s]\n", __func__, pathname);
    }
    return (nconf_fshdl_t)file;
}

size_t nconf_fread(void *buff, uint32_t size, uint32_t nmemb, nconf_fshdl_t hdl)
{
    return NCONF_FREAD(buff, size, nmemb, hdl);
}

void nconf_fclose(nconf_fshdl_t hdl)
{
    int ret=NCONF_FCLOSE(hdl);
    if (0!=ret) {
        UB_LOG(UBL_ERROR, "%s:failed to close file\n", __func__);
    }
}

