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
 * @ingroup TSN_UNIBASE_MODULE
 * @defgroup strutils String Utilities
 * @{
 * @file ub_strutils.h
 *
 * @brief Utility functions for strings
 *
 */

#ifndef UB_STRUTILS_H_
#define UB_STRUTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t ub_bytearray8_t[8]; //!< 8-byte binary array
typedef ub_bytearray8_t ub_streamid_t; //!< 8-byte binary array is used for Stream ID

typedef uint8_t ub_bytearray6_t[6]; //!< 6-byte binary array
typedef ub_bytearray6_t ub_macaddr_t; //!< 6-byte binary array is used for MAC Address


/**
 * @brief dump out data in hex format to the console
 * @param console	select output to consol
 * @param debug	select output to debug memory
 * @param buf	reference to the data buffer
 * @param size	size of the data to be dumped.
 * @param addr	offset address at the top of the data
 */
void ub_hexdump(bool console, bool debug, unsigned char *buf, int size, int addr);

/**
 * @brief convert a byte to 2-digit hex string
 * @param a	a byte value
 * @param astr	2-byte string pointer to return 2-digit hex string
 */
void ub_byte2hexstr(uint8_t a, char *astr);

/**
 * @brief convert array of 2-digit hex string
 * @param bstr	hex string array separated by delim
 * @param bytes	byte array to return the result
 * @param delim	a delimiter character
 * @param len	length of bytes
 * @return the number of bytes converted into the byte array
 */
int ub_hexstr2barray(const char *bstr, uint8_t *bytes, const char delim, int len);

/**
 * @brief convert a binary type mac address to a string type mac
 * address('XX:XX:XX:XX:XX:XX' format)
 * @return mac address in string format, the same pointer as smac
 * @param bmac	mac address in binary format.
 * @param smac	the converted mac addrsss.
 * @note smac must have at least 18 bytes.
*/
char *ub_bmac2smac(ub_macaddr_t bmac, char *smac);

/**
 * @brief convert a string type mac address('XX:XX:XX:XX:XX:XX' format)
 *  to a binary type mac address.
 * @return a reference of the mac address in binary format, the same pointer as bmac.
 * @param smac	mac address in string format
 * @param bmac	the converted mac address in binary format
 */
uint8_t *ub_smac2bmac(const char *smac, ub_macaddr_t bmac);

/**
 * @brief convert a binary type stream id to a string type stream id
 *  ('XX:XX:XX:XX:XX:XX:XX:XX' format)
 * @return the converted stream id in string format, the same pointer as sidstr
 * @param bsid	stream id in binary format
 * @param ssid the converted stream id in string format.
 * @note ssid must have at least 24 bytes
 */
char *ub_bsid2ssid(ub_streamid_t bsid, char *ssid);

/**
 * @brief convert a string type stream id('XX:XX:XX:XX:XX:XX:XX:XX' format)
 *   to a binary type stream id
 * @return a reference of the mac address in binary format, the same pointer as bsid
 * @param ssid	stream id in string format; either lower case format or upper scale format
 * @param bsid	the converted stream id in binary format
 */
uint8_t *ub_ssid2bsid(const char *ssid, ub_streamid_t bsid);

/**
 * @brief convert a string type byte array('01,00,1e,00,00,03,....02,00,01')
 *   to a byte array
 * @return  the length of the returned array
 * @param   input byte array in string format; either lower or upper case hex
 * @param   dest  the converted byte array.
 *	    The caller is responsible to reserve enough size of memory with 'dest'.
 * @param   base  used for strtoul conversion, must be '10','16'
 * @note    the conversion is stopped when it meets non-convertible string
 *	    Whatever non-convertible characters are treated as a delimiter.
 */
int ub_str2bytearray(uint8_t* dest, const char* input, uint8_t base);

/**
 * @brief convert a byte array to a string type
 *   byte array('01,00,1e,00,00,03,....02,00,01')
 * @return  the converted byte array.
 * @param   dest  the converted byte array in string format; lower case hex
 *	    The caller is responsible to reserve enough size of memory with 'dest'.
 * @param   bytes byte array to be converted
 * @param   len  number of bytes to be converted to string
 * @note    the conversion is stopped when the value of len is reached
 */
char* ub_bytearray2str(char * dest, const unsigned char* bytes, int len);

/**
 * @brief look for non space charcter and return the number of passed characters
 * @param astr	string
 * @param maxn	max number of characters to be checked
 * @note whitespace is either of space,tab,CR,LF
 * @return the index of the first non-whitespace charcter
 */
int ub_find_nospace(const char *astr, int maxn);

/**
 * @brief look for space charcter and return the number of passed characters
 * @return the index of the first whitespace charcter appears.
 * @param astr	string
 * @param maxn	max number of characters to be checked
 * @note whitespace is either of space,tab,CR,LF \n
 * whitespaces quoted by ' or " are skipped, and they can be escaped with back slash
 */
int ub_find_space(const char *astr, int maxn);

/**
 * @brief parse a command line string and make argc,argv[] for main function,
 * @return the number of parameters (argc)
 * @param line	command line string
 * @param argv	the result of argment array
 * @param maxargc	maximum number of arguments
 */
int ub_command_line_parser(char *line, char *argv[], int maxargc);

/**
 * @brief copy a string
 * @return 0:success, -1:fail
 * @param dest	destination string space, must have maxlen size including null
 * @param src	source string
 * @param maxlen	max length
 */
int ub_strncpy(char *dest, const char *src, int maxlen);

#ifdef __cplusplus
}
#endif
#endif
/** @}*/
