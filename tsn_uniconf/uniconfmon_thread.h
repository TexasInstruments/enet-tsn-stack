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
#ifndef UNICONFMON_THREAD_
#define UNICONFMON_THREAD_

#ifdef UCMON_UDP_THREAD

#define UCMON_CONSOLE_PRINT(...)					\
	if(!uniconfmon_thread_running()){				\
		UB_CONSOLE_PRINT(__VA_ARGS__);				\
	}else{								\
		char coutstr[UB_CHARS_IN_LINE];				\
		(void)snprintf(coutstr, UB_CHARS_IN_LINE, __VA_ARGS__);	\
		uniconfmon_thread_write((uint8_t*)coutstr, strlen(coutstr)); \
	}

#define UCMON_CONSOLE_READ(rsize, data, size, tout_ms)			\
	if(!uniconfmon_thread_running()){				\
		if(tout_ms<=0){rsize=read(STDIN_FILENO, data, size);}	\
		else{rsize=cb_fdread_timeout(STDIN_FILENO, data, size, tout_ms);} \
	}else{								\
		rsize=uniconfmon_thread_read(data, size, tout_ms);	\
	}

/**
 * @brief start uniconfmon as thread
 * @param udp port number, which receives uniconfmon commands
 * @return 0:success, -1:error
 * @note in the thread, UCMON_MAIN is called to pass uniconfmon command
 */
int uniconfmon_thread_start(uint16_t port);

/**
 * @brief stop uniconfmon as thread
 * @return 0:success, -1:error
 */
int uniconfmon_thread_stop(void);

/**
 * @brief check if uniconfmon_thread is running
 * @return true:running, false:not running
 */
bool uniconfmon_thread_running(void);

/**
 * @brief in uniconfmon_thread, write output data on the connected udp socket
 * @return -1:error, >0:written size
 */
int uniconfmon_thread_write(uint8_t *data, int size);

/**
 * @brief in uniconfmon_thread, read data on the connected udp socket
 * @return -1:error, >0:read size
 */
int uniconfmon_thread_read(uint8_t *data, int size, int tout_ms);

#else // else UCMON_UDP_THREAD

#define UCMON_CONSOLE_PRINT UB_CONSOLE_PRINT
#define UCMON_CONSOLE_READ(rsize, data, size, tout_ms)			\
	if(tout_ms<=0){rsize=read(STDIN_FILENO, data, size);}		\
	else{rsize=cb_fdread_timeout(STDIN_FILENO, data, size, tout_ms);}

static inline int uniconfmon_thread_start(uint16_t port){return -1;}
static inline int uniconfmon_thread_stop(void){return -1;}
static inline bool uniconfmon_thread_running(void){return false;}
static inline int uniconfmon_thread_write(uint8_t *data, int size){return -1;}

#endif // UCMON_UDP_THREAD

#endif
