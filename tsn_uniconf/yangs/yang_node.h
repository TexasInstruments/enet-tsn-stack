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
#ifndef YANG_NODE_H_
#define YANG_NODE_H_

#include <inttypes.h>
#include "../uc_dbal.h"

enum {
	XL4_DATA_RW = 0,
	XL4_EXTMOD_RW,
	XL4_DATA_RO = 0x80,
	XL4_EXTMOD_RO,
};

enum {
	YANG_VALUE_TYPES,
	UC_READY,
	UC_ASKACTION_REG,
	UC_NOTICE_REG,
	UC_NOTICE_ACT,
	UC_YANGMOD_REG,
	XL4_EXTMOD_REG,
	XL4_ENUM_TABLE,
	XL4_RTIME_REG,
	XL4_RTIME_DATA,
	UC_LAST_DB_ACCESS,
	UC_PREFIX_NAMESPACE,
	XL4_DATA_ENUM_END,
};

/*
YANG_NODE_CACHED_MODULE_GET_ENUM_DEF(IEEE1588_PTP_TT, "ieee1588-ptp-tt")
this create the next variable and function
-----
uint8_t IEEE1588_PTP_TT_var=0xff;
uint8_t IEEE1588_PTP_TT_func(uc_dbald *dbald){
	if(IEEE1588_PTP_TT_var==0xff){
		IEEE1588_PTP_TT_var=yang_node_module_get_enum(dbald, "ieee1588-ptp-tt");
	}
	return IEEE1588_PTP_TT_var;
}
-----
To use them define macros as follows, 'ydrd->dbald' must be adjusted for the code:
#define IEEE1588_PTP_TT_RW IEEE1588_PTP_TT_func(ydrd->dbald)
#define IEEE1588_PTP_TT_RO (IEEE1588_PTP_TT_func(ydrd->dbald)|0x80)

To define the macros outside the same source file, define 'extern'
extern uint8_t IEEE1588_PTP_TT_func(uc_dbald *dbald);
*/

#define YANG_NODE_CACHED_MODULE_GET_ENUM_DEF(x, name)	\
	uint8_t x##_var=0xff; \
	uint8_t x##_func(uc_dbald *dbald){ \
		if(x##_var==0xff){ \
			x##_var=yang_node_mod_get_enum(dbald, name); \
		} \
		return x##_var; \
	}

/*
YANG_NODE_CACHED_EXTMODULE_GET_ENUM_DEF(XL4GPTP, "xl4gptp")
this create the next variable and function
-----
uint8_t XL4_EXTMOD_XL4GPTP_var=0xff;
uint8_t XL4_EXTMOD_XL4GPTP_func(uc_dbald *dbald){
	if(XL4_EXTMOD_XL4GPTP_var==0xff){
		XL4_EXTMOD_XL4GPTP_var=yang_node_setget_extmodid(dbald, "xl4gptp");
	}
	return XL4_EXTMOD_XL4GPTP_var;
}
-----
To use them define macros as follows, 'ydrd->dbald' must be adjusted for the code:
#define XL4_EXTMOD_XL4GPTP XL4_EXTMOD_XL4GPTP_func(ydrd->dbald)

To define the macros outside the same source file, define 'extern'
extern uint8_t uint8_t XL4_EXTMOD_XL4GPTP_func(uc_dbald *dbald);
*/

#define YANG_NODE_CACHED_EXTMODULE_GET_ENUM_DEF(x, name)	\
	uint8_t XL4_EXTMOD_##x##_var=0xff; \
	uint8_t XL4_EXTMOD_##x##_func(uc_dbald *dbald){ \
		if(XL4_EXTMOD_##x##_var==0xff){ \
			XL4_EXTMOD_##x##_var=yang_node_extmod_get_enum(dbald, name); \
		} \
		return XL4_EXTMOD_##x##_var; \
	}

/* simillar macro for 'rtime' module */
#define YANG_NODE_CACHED_RTIMEMODULE_GET_ENUM_DEF(x, name)	\
	uint8_t XL4_RTIME_##x##_var=0xff; \
	uint8_t XL4_RTIME_##x##_func(uc_dbald *dbald){ \
		if(XL4_RTIME_##x##_var==0xff){ \
			XL4_RTIME_##x##_var=yang_node_rtime_get_enum(dbald, name); \
		} \
		return XL4_RTIME_##x##_var; \
	}

/**
 * @brief register uniconf operation nodes('xl4-data' and 'xl4-extmod')
 * @param dbald	DB access pointer
 * @note only 'uniconf' class this function.
 */
int yang_node_uniconf_init(uc_dbald *dbald);

/**
 * @brief get node enums from a string
 * @param dbald	DB access pointer
 * @param astr	node string, "/Aaa/Bbb/Ccc" or "Aaa/Bbb/Ccc" or "Aaa" format
 * @param anums	return resolved enum for nodes
 * @param maxele	this number of elements is available in 'anums'
 * @return -1:error, >=0: number of elements in anums
 * @note "/Aaa/Bbb/Ccc" case, the module name is resolved in this funcion.
 *	"Aaa/Bbb/Ccc" case, the module enum must pre-set in anums[0] before the call
 *	"Aaa/Bbb/Ccc" and anums[0]==XL4_EXTMOD_RW case, Aaa must be an ext. module name
 *	"Aaa" and anums[0]==XL4_EXTMOD_RW case, ext module enum must pre-set in anms[1]
 */
int yang_node_get_node_enums(uc_dbald *dbald, const char *astr, uint8_t *anums, int maxele);

/**
 * @brief get node string from a list of node enum
 * @param dbald	DB access pointer
 * @param rstr	return node string, allocated in side this function.
 *	the caller needs to release by 'UB_SD_RELMEM(YANGINIT_GEN_SMEM, rstr)'
 * @param anums	a list of enums to resolve
 * @return -1:error, 0: success, the result in *rstr
 * @note for yang modules, anums[0] is enum of the module. anums[1:] is a list to resolve.
 *	for non-yang modules, anums[0]=XL4_EXTMOD_RW/RO, anums[1] is enum of the module,
 *	anums[2:] is a list to resolve.
 *	ansums[0] = XL4_DATA_RW/RO case, ansums[1:] is resolved by the internal codes.
 */
int yang_node_get_node_string(uc_dbald *dbald, char **rstr, uint8_t *anums);

/**
 * @brief get module id(enum nuber) of module
 * @param dbald	DB access pointer
 * @param emodname	module name
 * @return 0xff:error, >=0:module id(enum nuber)
 */
uint8_t yang_node_mod_get_enum(uc_dbald *dbald, const char *emodname);
uint8_t yang_node_extmod_get_enum(uc_dbald *dbald, const char *emodname);

/**
 * @brief set module id(enum nuber) of module
 * @param dbald	DB access pointer
 * @param emodname	module name
 * @return -1:error, >=0:module id(enum nuber)
 */
int yang_node_mod_set_enum(uc_dbald *dbald, const char *emodname);
int yang_node_extmod_set_enum(uc_dbald *dbald, const char *emodname);

/**
 * @brief get module name from module id
 * @param dbald	DB access pointer
 * @param modid	module id
 * @param modname char memory to return the result
 * @param nsize	size of modname memory
 * @return -1:error, >=0:module id(enum nuber)
 */
int yang_node_mod_get_string(uc_dbald *dbald, uint8_t modid, char *modname, int nsize);
int yang_node_extmod_get_string(uc_dbald *dbald, uint8_t modid, char *modname, int nsize);

/**
 * @brief get number of registered IDs
 * @param dbald	DB access pointer
 * @return -1:error, >=0:number of modules
 */
int yang_node_mod_numofids(uc_dbald *dbald);
int yang_node_extmod_numofids(uc_dbald *dbald);

/**
 * @brief (name, id) for runtime use
 * @param dbald	DB access pointer
 * @param nameid	id
 * @param name char memory to return the result
 * @param nsize	size of modname memory
 * @return -1:error, >=0:module id(enum nuber)
 */
int yang_node_rtime_get_string(uc_dbald *dbald, uint8_t nameid, char *name, int nsize);
uint8_t yang_node_rtime_get_enum(uc_dbald *dbald, const char *name);
int yang_node_rtime_set_enum(uc_dbald *dbald, const char *name);

/**
 * @brief get namespace from prefix
 * @param dbald	DB access pointer
 * @param prefix
 * @return NULL:error, namespace string(direct DB data, effective only in the same DB session)
 */
const char *yang_node_get_prefix_namespace(uc_dbald *dbald, const char *prefix);

/**
 * @brief set (prefix, namespace)
 * @param dbald	DB access pointer
 * @param prefix
 * @param name_space
 * @return -1:error, 0:okay
 */
int yang_node_set_prefix_namespace(uc_dbald *dbald, const char *prefix,
				   const char *name_space);

/**
 * @brief get namespace from nodepath
 * @param dbald	DB access pointer
 * @param npath node path string which starts with '/'
 * @return NULL:error, namespace string(direct DB data, effective only in the same DB session)
 */
const char *yang_node_get_npath_namespace(uc_dbald *dbald, char *npath);

#endif
