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
 * @addtogroup xl4avbl2
 * @{
 * @file avtpc.h
 * @brief IEEE1722 AVB Transport Protocol, client program interface
 */
#ifndef __AVTPC_H__
#define __AVTPC_H__


#include "avbtp.h"

/** @brief this number of payload_multi_bufs can be handled as maximum.
 * the final block must have size=0 and nodata,
 * and the index of the final block can be AVTPC_MAX_MEM_BLOCKS+1
 */
#define AVTPC_MAX_MEM_BLOCKS 10

/** @brief the data handle of avtpc, the inside of it is private */
typedef struct avtpc_data avtpc_data_t;

/**
 * @brief initialize avtpc
 * @param shsuf	suffix character of the shared memory node name
 * @return the data handle of avtpc
 */
avtpc_data_t *avtpc_init(char *shsuf);

/**
 * @brief set the direct mode of the transmitter.
 * The talker sends packets directly to the network instead of through avtpd.
 * This mode is used in case need to improve the performance and latency of the talker.
 * In this mode, the software traffic shaper is disabled.
 * To run multiple talkers in the direct mode, each talker must instantiate multiple
 * separated avtpc instances by calling @ref avtpc_init().
 * This mode can coexist with the avtpd.
 * @param avtpc	the data handle of avtpc
 * @return 0:success, -1:error
 * @note This function must be called before avtpc_connection_request().
 */
int avtpc_set_txdirect(avtpc_data_t *avtpc);

/**
 * @brief Set the the rxdirect mode of the receiver.
 *
 * This mode is used in case we need to improve the performance and
 * latency of the listener.
 *
 * The listener receives packets directly from the network, bypassing avtpd.
 * Once this mode is set for a listener connection, the avtpd cannot be used
 * for interfaces handled by this avtpc instance.
 * Note that a single 'avtpc' will receive all packets from the interface,
 * and it is the user's responsibility to select the expected stream ID based on
 * avbtp_rcv_cb_t from the receive callback. The avtpc will not filter packets
 * based on the stream ID.
 * Setting multiple listener connection instances to the direct mode or mixing up
 * with running 'avtpd' for the same interface will result the undefined behavior.
 *
 * @param avtpc Pointer to avtpc_data_t structure.
 * @return 0:success, -1:error
 * @note This function must be called before @ref avtpc_connection_request().
 * Only one 'avtpc' or 'avtpc_crf' or 'avtpc_acf' instance can be set to direct mode
 * for a listener connection.
 */
int avtpc_set_rxdirect(avtpc_data_t *avtpc);

/**
 * @brief check if the client is direct mode or not
 * @param avtpc	the data handle of avtpc
 * @return true:if either tx or rx is direct mode, else return false
 */
bool avtpc_is_direct(avtpc_data_t *avtpc);

/**
 * @brief set the list of join multicast address beyond the join_mcast
 * specified in the ccr. This function is used only in the rxdirect mode to support
 * receiving multiple streams from multiple multicast addresses.
 * @param avtpc	the data handle of avtpc
 * @param join_mcasts	list of join multicast addresses
 * @param num_mcasts	number of join multicast addresses
 * @return 0: success, -1: error
 * @note This function must be called before avtpc_connection_request().
 * @note Only one avtpc or avtpc_crf or avtpc_acf instance can be set to rxdirect mode.
 */
int avtpc_set_join_mcasts(avtpc_data_t *avtpc, ub_macaddr_t join_mcasts[],
						   int num_mcasts);

/**
 * @brief set the stream data to be sent and received through the UDP test port
 * @param avtpc	the data handle of avtpc
 * @param tsport	source port number for the test mode, set to 0 if not used
 * @param tdport	destination port number for the test mode, set to 0 if not used
 * @return 0: success, -1: error
 * @note This function must be called before avtpc_connection_request() and
 * used only in the direct mode.
 */
int avtpc_set_testport(avtpc_data_t *avtpc, uint16_t tsport, uint16_t tdport);

/**
 * @brief set receiver callback function
 *
 * In the rxdirect mode, to receive mixing data type of 'avtpc', 'avtpc_crf' and
 * 'avtpc_acf', the user should create a single 'avtpc' direct instance, then the
 * 'rcv_cb' callback will be called with avbtp_rcv_cb_info_t data type to
 * identify the data type. This callback prototype is same as the CRF rx callback,
 * so the CRF rx processing is the same as in the CRF non-direct mode:
 * the user calls @ref avbtp_deserialize_stdtype_crf_data() to get the CRF data.
 * For the ACF data, the user call the @ref avtpc_acf_process_payload() function
 * to process the ACF data.
 *
 * @param avtpc	the data handle of avtpc
 * @param rcv_cb	callback function
 * @param rcv_cbdata	the callback function uses this data pointer
 */
void avtpc_set_rcv_cb(avtpc_data_t *avtpc,
		      avbtp_rcv_cb_t rcv_cb, void *rcv_cbdata);

/**
 * @brief set receiver timeout callback function
 * @param avtpc	the data handle of avtpc
 * @param rcvtimeout_cb	callback function
 * @param timeout_cbdata	the callback function uses this data pointer
 * @note This function is not supported in the direct mode.
 */
void avtpc_set_rcvtimeout_cb(avtpc_data_t *avtpc,
			avbtp_rcvtimeout_cb_t rcvtimeout_cb, void *timeout_cbdata);
/**
 * @brief close avtpc
 * @return 0:success, -1:error
 * @param avtpc	the data handle of avtpc
 */
int avtpc_close(avtpc_data_t *avtpc);

/**
 * @brief request connection to avtpd or setup the network connection
 * in the direct mode
 *
 * @return 0:success, -1:error
 * @param avtpc	the data handle of avtpc
 * @param ccr	CCR data pointer
 */
int avtpc_connection_request(avtpc_data_t *avtpc, client_connect_request_t *ccr);

/**
 * @brief request disconnection to avtpd or close the network connection in
 * the direct mode.
 *
 * @return 0:success, -1:error
 * @param avtpc	the data handle of avtpc
 */
int avtpc_disconnection_request(avtpc_data_t *avtpc);

/**
 * @brief send a packet of stream data
 * @return 0:success, -1:error
 * @param avtpc	the data handle of avtpc
 * @param sd_size	size of payload data
 * @param sd_payload	payload data pointer
 * @param timestamp	timestamp for this packet
 */
int avtpc_write_sd_packet(avtpc_data_t *avtpc, int sd_size, uint8_t *sd_payload,
			  uint64_t timestamp);

/**
 * @brief send multiple packets of stream data
 * @return 0:success, -1:error
 * @param avtpc	the data handle of avtpc
 * @param plmbs	multiple packet data array
 * @param timestamp	timestamp for all of the packets, the same timestamp is applied
 */
int avtpc_write_sd_packet_mb(avtpc_data_t *avtpc, payload_multi_bufs_t plmbs[],
			     uint64_t timestamp);

/**
 * @brief send a packet of control data
 * @return 0:success, -1:error
 * @param avtpc	the data handle of avtpc
 * @param cd_size	size of payload data
 * @param cd_payload	payload data pointer
 */
int avtpc_write_control_packet(avtpc_data_t *avtpc, int cd_size, uint8_t *cd_payload);

/**
 * @brief get preset of common stream header
 * @return preset of common stream header
 * @param avtpc	the data handle of avtpc
 */
avbtp_cm_stream_header_t *avtpc_cmsh_in_ccd(avtpc_data_t *avtpc);

/**
 * @brief get preset of common control header
 * @return preset of common control header
 * @param avtpc	the data handle of avtpc
 */
avbtp_cm_control_header_t *avtpc_cmch_in_ccd(avtpc_data_t *avtpc);


/**
 * @brief set multicast address as the destination
 * @return 0:success, -1:error
 * @param avtpc	the data handle of avtpc
 * @param newaddr	new multicast address
 * @param wait_prev_done	wait if true and previously sent packets
 * are still in the shared memory
 * @note This function is not applied for direct mode.
 */
int avtpc_update_mcast_addr(avtpc_data_t *avtpc, ub_macaddr_t newaddr, bool wait_prev_done);

/**
 * @brief check the packet timestamp to avb time
 * @return bit0-3:normal,notvalid,negative move,big move
 	   bit4-7:in past, in future
 * @param avtpc	the data handle of avtpc
 * @param rsdinfo	information from a stream packet
 */
int avtpc_rec_ts_check(avtpc_data_t *avtpc, avbtp_sd_info_t *rsdinfo);

/**
 * @brief set software traffic shaper
 * @return 0:success, -1:error
 * @param ccr	CCR data pointer
 * @param tshape_rate	assingn bit rate for this stream, bytes per mili second unit,
 			0 to disable the shaper
 * @param tshape_minsleep	minimum unit of sleep time(in nsec) when the stream is overflowing
 * @note This function is not applied for direct mode.
 */
int avtpc_set_tshaper(client_connect_request_t *ccr, uint32_t tshape_rate, int tshape_minsleep);

/**
 * @brief get number of communication buffer between avtpd and avtpc
 * @return number of buffer
 * @param avtpc	the data handle of avtpc
 * @note This function is not applied for direct mode.
 */
int avtpc_get_bufnum(avtpc_data_t *avtpc);

/**
 * @brief get number of used communication buffer between avtpd and avtpc
 * @return number of used buffer
 * @param avtpc	the data handle of avtpc
 * @note This function is not applied for direct mode.
 */
int avtpc_get_bufused(avtpc_data_t *avtpc);

/**
 * @brief check if the avtpc receive thread is running or not
 * @return true if running
 * @param avtpc	the data handle of avtpc
 * @note This function is not applied for direct mode.
 */
bool avtpc_rcv_thread_is_running(avtpc_data_t *avtpc);

/**
 * @brief get connection index
 * @return connection index:success, -1:error
 * @param avtpc	the data handle of avtpc
 */
int avtpc_get_connection_index(avtpc_data_t *avtpc);

#endif
/** @}*/
