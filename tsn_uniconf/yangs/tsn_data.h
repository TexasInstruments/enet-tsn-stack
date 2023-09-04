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
#ifndef XL4_DATA_H_
#define XL4_DATA_H_

#include "../uc_dbal.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	YANG_VALUE_TYPES,
	UC_READY,
	UC_ASKACTION_REG,
	UC_NOTICE_REG,
	UC_NOTICE_ACT,
	XL4_EXTMOD_REG,
	UC_LAST_DB_ACCESS,
	XL4_DATA_ENUM_END,
};

/* number of network devices never go beyond this number */
#define XL4_DATA_ABS_MAX_NETDEVS 16

#define YANGINIT_GEN_SMEM yanginit_gen_smem

typedef struct xl4_data_data xl4_data_data_t;

typedef uint8_t (*get_enum_func)(char*);
typedef const char* (*get_string_func)(uint8_t);

xl4_data_data_t *xl4_data_init(uc_dbald *dbald);

void xl4_data_set_dbald(xl4_data_data_t *xdd, uc_dbald *dbald);

void xl4_data_close(xl4_data_data_t *xdd);

int xl4_data_lock(xl4_data_data_t *xdd, int tout_ms);

void xl4_data_unlock(xl4_data_data_t *xdd);

uint8_t xl4_data_get_enum(char *astr);

/**
 * @brief get_enum function from registered xl4-extmod
 * @note set emodid=0xff to search func by emoname
 * 	 set emoname=NULL to search func by emodid
 */
get_enum_func xl4_extmod_get_enum_func(xl4_data_data_t *xdd, char *emodname, uint8_t emodid);

/**
 * @brief get_string function from registered xl4-extmod
 * @note set emodid=0xff to search func by emoname
 * 	 set emoname=NULL to search func by emodid
 */
get_string_func xl4_extmod_get_string_func(xl4_data_data_t *xdd, char *emodname, uint8_t emodid);

const char *xl4_data_get_string(uint8_t anum);

uint8_t xl4_data_get_modid(xl4_data_data_t *xdd, char *emodname);

char *xl4_data_get_modname(xl4_data_data_t *xdd, uint8_t emodid);

int xl4_data_set_xl4ext(xl4_data_data_t *xdd, char *emodname,
			get_enum_func get_enum, get_string_func get_string);

int xl4_data_remove_xl4ext(xl4_data_data_t *xdd, char *emodname);

#ifdef __cplusplus
}
#endif

#endif
