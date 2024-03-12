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
 * @addtogroup gptp
 * @{
 * @file gptpbasetypes.h
 * @brief File contains base datatypes used, as per IEEE 802.1AS
 * Standard Documentation.
 */

#ifndef GPTPBASETYPES_H_
#define GPTPBASETYPES_H_

#include <string.h>

/**
 * @brief Macro to define maximum path trace.
 * @note The maximum possible number is 179 based on specification we can set
 * the maximum here and restrict the frame using MAX_PTP_PACKET instead
 * @code{.c}
 * #define MAX_PATH_TRACE_N 179
 * @endcode
 * @see 10.3.8.23 pathTrace
 */
#define MAX_PATH_TRACE_N 179

/**
 * @brief Macro which defines Clock Identity Length.
 */
#define CLOCK_IDENTITY_LENGTH	  8

typedef uint8_t ClockIdentity[CLOCK_IDENTITY_LENGTH];

/**
 * @brief Primitive data types specifications for IEEE 802.1AS.
 * All non-primitive data types are derived from the primitive types, signed integers are
 * represented as two's complement form.
 * @verbatim See the following Table@endverbatim
 * Data Types   |         Definition
 * ----------   | ----------------------------------------------
 * EnumerationN | N-bit enumerated value
 * UIntegerN    | N-bit unsigned integer
 * Nibble       | 4-bit field not interpreted as a number
 * Octet	| 8-bit field not interpreted as a number
 * OctetN	| N-octet fiels not interpreted as a number
 * Double 	| Double precision (64-bit) floating-point vlaue
 */

typedef uint8_t Nibble;
typedef uint8_t Octet;
typedef uint8_t Octet2[2];
typedef uint8_t Octet3[3];
typedef uint8_t Octet4[4];
typedef uint8_t Enumeration2;
typedef uint8_t Enumeration4;
typedef uint8_t Enumeration8;
typedef uint16_t Enumeration16;
typedef uint32_t Enumeration24;
typedef uint8_t UInteger4;

/**
 * @brief The portIdentity identifies a port of a time-aware system.
 * @note ClockIdentity is a array of unsigned char which is used to identify time-aware system.
 */
typedef struct PortIdentity {
	ClockIdentity clockIdentity;
	uint16_t portIndex;
} PortIdentity;

/**
 * @brief The clock quality represents quality of clock.
 */
typedef struct ClockQuality {
	uint8_t clockClass;
	Enumeration8 clockAccuracy;
	uint16_t offsetScaledLogVariance;
} ClockQuality;

/**
 * @brief 48bit variable, lsb is 32 bits
 */
typedef struct UInteger48 {
	uint32_t lsb;
	uint16_t msb;
} UInteger48;
/**
 * @brief 48bit variable, msb is 32 bits
 */
typedef struct UInteger48m32 {
	uint16_t lsb;
	uint32_t msb;
} UInteger48m32;

/**
 * @brief UInteger112 (802.1AS, 10.3.2 systemIdentity)
 */
typedef struct UInteger112 {
	uint8_t priority1;
	uint8_t clockClass;
	uint8_t clockAccuracy;
	uint16_t offsetScaledLogVariance;
	uint8_t priority2;
	ClockIdentity clockIdentity;
} __attribute__((packed, aligned(1))) UInteger112;

/**
 * @brief UInteger224 (802.1AS, 10.3.4 time-synchronization spanning
 * tree priority vectors )
 */
typedef struct UInteger224 {
        UInteger112 rootSystemIdentity;
        uint16_t stepsRemoved;
        PortIdentity sourcePortIdentity;
        uint16_t portNumber;
} __attribute__((packed, aligned(1))) UInteger224;

/**
 * @brief The ScaledNs type represents signed values of time and time
 * interval in units of 2e-16 ns.
 */
typedef struct ScaledNs {
	uint16_t subns;
	int64_t nsec;
	int16_t nsec_msb;
} ScaledNs;

/**
 * @brief The ScaledNs type represents unsigned values of time and
 * time interval in units of 2^-16 ns.
 * @code
 * 2.5 ns expressed as below
 * 0x0000 0000 0000 0000 0002 8000
 * @endcode
 *
 */
typedef struct UScaledNs {
	uint16_t subns;
	uint64_t nsec;
	uint16_t nsec_msb;
} UScaledNs;

/*
 * @brief 2^-16 ns scaling multiplier
 */
#define SCALEDNS_FACTOR 0x10000

/*
 * @brief The TimeInterval type represents time intervals, in units of 2^-16 ns
 * @verbatim Example to express ns@endverbatim
 * @code
 * 2.5 ns expressed as below
 * 0x0000 0000 0002 8000
 * @endcode
 *
 */
typedef struct TimeInterval {
	int64_t scaledNanoseconds;
} TimeInterval;

/**
 * @brief The Timestamp type represents a positive time with respect to the epoch.
 * @verbatim For example:@endverbatim
 * @code
 * +2.000000001 seconds is represented by seconds = 0x0000 0000 0002 and nanoseconds= 0x0000 0001
 * @endcode
 *
 */
typedef struct Timestamp {
	uint32_t nanoseconds;
	UInteger48 seconds;
} Timestamp;

/**
 * @brief The ExtendTimestamp type represents a positive time with respect to the epoch.
 * The fractionalNanoseconds member is the fractional portion of the timestamp in units of 2^16 ns.
 * @verbatim For example:@endverbatim
 * @code
 * +2.000000001 seconds is represented by seconds = 0x0000 0000 0002
 * and fractionalNnanoseconds = 0x0000 0001 0000
 * @endcode
 *
 */
typedef struct ExtendedTimestamp {
	UInteger48m32 fractionalNanoseconds;
	UInteger48 seconds;
} ExtendedTimestamp;

#define VALUE_DISABLED 0u
#define VALUE_ENABLED 1u

/**
 * @brief the type of source of time used by a ClockMaster(802.1AS, 8.6.2.7 timeSource)
 */
typedef enum {
        ATOMIC_CLOCK         = 0x10,
        GPS                  = 0x20,
        TERRESTRIAL_RADIO    = 0x30,
        PTP                  = 0x40,
        NTP                  = 0x50,
        HAND_SET             = 0x60,
        OTHER                = 0x90,
        INTERNAL_OSCILLATOR  = 0xA0,
} TimeSource;

// 14.8.3 portState, (from IEEE 1588 Table-8)
/**
 * @brief value of the port state (802.1AS 14.8.3 portState)
 * @note IEEE1588-2019,
 * 	initializing=1, faulty=2, disabled=3, listening=4,
 * 	pre-master=5, master=6, passive=7, uncalibrated=8,
 * 	slave=9
 */
typedef enum {
	DisabledPort = 3,
	MasterPort = 6,
	PassivePort = 7,
	SlavePort = 9,
} PTPPortState;

typedef enum {
	GMSYNC_UNSYNC=0,
	GMSYNC_SYNC,
	GMSYNC_SYNC_STABLE,
} gmsync_status_t;

#endif
/** @}*/
