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
 *              TI FreeRTOS (Jacinto)
 */

/*=============================================================================
 * Include Files
 *============================================================================*/

#include <string.h>
#include <ti/fs/fatfs/ff.h>
#include <ti/fs/fatfs/FATFS.h>
#include "nconf_fsutils.h"

/*=============================================================================
 * Macros and Constants
 *============================================================================*/

#define NCONF_FS_HANDLE nconf_fs_handle

/*=============================================================================
 * Function Prototypes
 *============================================================================*/

static int nconf_fsmode_conv(const char *mode);

/*=============================================================================
 * Global Variables
 *============================================================================*/

UB_SD_GETMEM_DEF(NCONF_FS_HANDLE, sizeof(FIL), 1);

/*=============================================================================
 * Function Definitions
 *============================================================================*/

nconf_fshdl_t nconf_fopen(const char *pathname, char *mode)
{
    FIL *file=NULL;
    FRESULT res;
    int fs_mode;

    fs_mode=nconf_fsmode_conv(mode);
    if (fs_mode == -1) { goto end_oper; }

    file=(FIL*)UB_SD_GETMEM(NCONF_FS_HANDLE, sizeof(FIL));
    if (NULL==file) {
        UB_LOG(UBL_ERROR, "%s:failed to allocte file handle\n", __func__);
        goto end_oper;
    }

    res=f_open(file, pathname, fs_mode);
    if (res != FR_OK) {
        UB_LOG(UBL_ERROR, "%s:failed to open [%s]\n", __func__, pathname);
        UB_SD_RELMEM(NCONF_FS_HANDLE, file);
        file=NULL;
    }

end_oper:
    return (nconf_fshdl_t)file;
}

size_t nconf_fread(void *buff, uint32_t size, uint32_t nmemb, nconf_fshdl_t hdl)
{
    uint32_t read_size = 0;
    FRESULT res=f_read((FIL*)hdl, buff, size*nmemb, &read_size);
    if (res != FR_OK) {
        UB_LOG(UBL_ERROR,"%s:failed to read data from file err: %d!\n",
               __func__, res);
        return -1;
    }
    return read_size;
}

void nconf_fclose(nconf_fshdl_t hdl)
{
    if (hdl) {
        f_close((FIL*)hdl);
        UB_SD_RELMEM(NCONF_FS_HANDLE, hdl);
    }
}

static int nconf_fsmode_conv(const char *mode)
{
    int ti_mode;
    if (strcmp(mode, "r")==0) {
        ti_mode = FA_READ|FA_OPEN_EXISTING;
    } else if (strcmp(mode, "r+")==0) {
        ti_mode = FA_READ|FA_WRITE;
    } else if (strcmp(mode, "w")==0) {
        ti_mode = FA_WRITE|FA_CREATE_ALWAYS;
    } else if (strcmp(mode, "w+")==0) {
        ti_mode = FA_READ|FA_WRITE|FA_CREATE_NEW;
    } else if (strcmp(mode, "a")==0) {
        ti_mode = FA_READ|FA_WRITE|FA_OPEN_EXISTING;
    } else if(strcmp(mode, "a+")==0) {
        ti_mode = FA_READ|FA_WRITE|FA_CREATE_NEW;
    } else {
        UB_LOG(UBL_ERROR,"%s: incorrect mode when opening file %s\n",
               __func__, mode);
        ti_mode = -1;
    }
    return ti_mode;
}

