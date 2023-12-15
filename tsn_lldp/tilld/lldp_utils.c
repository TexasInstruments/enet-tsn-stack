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
/*
 * Copyright (c) 2017 Simon Goldschmidt
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Simon Goldschmidt <goldsimon@gmx.de>
 *
 */

#include "../lldp_utils.h"
#include "../lldp_cfg.h"
#include <string.h>
#include <time.h>
#include <tsn_combase/combase.h>

/**Refer from LWIP source code*/
#define LLDP_IPV4 1
#define lldp_in_range(c, lo, up)  ((uint8_t)(c) >= (lo) && (uint8_t)(c) <= (up))
#define lldp_isdigit(c)           lldp_in_range((c), '0', '9')
#define lldp_isxdigit(c)          (lldp_isdigit(c) || lldp_in_range((c), 'a', 'f') || lldp_in_range((c), 'A', 'F'))
#define lldp_islower(c)           lldp_in_range((c), 'a', 'z')
#define lldp_isspace(c)           ((c) == ' ' || (c) == '\f' || (c) == '\n' || (c) == '\r' || (c) == '\t' || (c) == '\v')
#define lldp_isupper(c)           lldp_in_range((c), 'A', 'Z')
#define lldp_tolower(c)           (lldp_isupper(c) ? (c) - 'A' + 'a' : c)
#define lldp_toupper(c)           (lldp_islower(c) ? (c) - 'a' + 'A' : c)

/** IPv4 only: set the IP address given as an uint32_t */
#define ip4_addr_set_u32(dest_ipaddr, src_u32) ((dest_ipaddr)->addr = (src_u32))
/** Clear the zone field of an IPv6 address, setting it to "no zone". */
#if LLDP_IPV6_SCOPES
#define ip6_addr_clear_zone(ip6addr) ((ip6addr)->zone = IP6_NO_ZONE)
#endif

/** This is the aligned version of ip4_addr_t,
   used as local variable, on the stack, etc. */
struct ip4_addr {
  uint32_t addr;
};

/** ip4_addr_t uses a struct for convenience only, so that the same defines can
 * operate both on ip4_addr_t as well as on ip4_addr_p_t. */
typedef struct ip4_addr ip4_addr_t;

/** This is the aligned version of ip6_addr_t,
    used as local variable, on the stack, etc. */
struct ip6_addr {
  uint32_t addr[4];
#if LLDP_IPV6_SCOPES
  uint8_t zone;
#endif /* LLDP_IPV6_SCOPES */
};

/** IPv6 address */
typedef struct ip6_addr ip6_addr_t;

static int ip4addr_aton(const char *cp, ip4_addr_t *addr);
static int ip6addr_aton(const char *cp, ip6_addr_t *addr);

/// @brief Get Dest MAC Address
/// @param buf 
/// @param pos 
/// @param mac 
void get_dest_mac_addr(const char* buf, int* pos, ub_macaddr_t mac)
{
	memcpy(mac, &buf[*pos], sizeof(ub_macaddr_t));
	*pos += sizeof(ub_macaddr_t); // shift the index of buffer
}

/// @brief Get Source MAC Address
/// @param buf 
/// @param pos 
/// @param mac 
void get_src_mac_addr(const char* buf, int* pos, ub_macaddr_t mac)
{
	memcpy(mac, &buf[*pos], sizeof(ub_macaddr_t));
	*pos += sizeof(ub_macaddr_t); // shift the index of buffer
}

/// @brief Get Ethernet Protocol
/// @param buf 
/// @param pos 
/// @return 
uint16_t get_eth_proto(const char* buf, int* pos)
{
	uint16_t protocol = 0x0000;
	protocol = (uint16_t)buf[*pos + 1] & 0x00FF;
	protocol |= ((buf[*pos] << 8) & 0xFF00);
	 *pos += sizeof(uint16_t); // shift the index of buffer
	return protocol;
}

uint32_t get_current_time_in_milisec()
{
	uint32_t time_in_ms = 0;
	uint64_t time_in_nsec =  ub_mt_gettime64();
	time_in_ms = (uint32_t)(time_in_nsec / 1000000);
	return time_in_ms;
}

uint64_t get_current_time_in_usec()
{
	uint64_t time_in_us = 0;
	uint64_t time_in_nsec =  ub_mt_gettime64();
	time_in_us =  time_in_nsec / 1000;
	return time_in_us;
}

void ipv4_str_to_hex(const char* ipv4str, uint8_t* buf)
{
	ip4_addr_t addr;
	memset (&addr, 0, sizeof(ip4_addr_t));
	if (ip4addr_aton(ipv4str, &addr) == 1)
	{
		memcpy(buf, (uint8_t*)&addr.addr, sizeof(addr.addr));
	}
	else
	{
		// Invalid Address
		UB_LOG(UBL_INFO, "%s Wrong IPv4 format (%s) \n", __func__, ipv4str);
	}
}

void ipv6_str_to_hex(const char* ipv6str, uint8_t* buf)
{
	ip6_addr_t addr;
	memset (&addr, 0, sizeof(ip6_addr_t));
	if (ip6addr_aton(ipv6str, &addr) == 1)
	{
		for (int i=0; i<4;i++)
		{
			memcpy(&buf[i*4], (uint8_t*)&addr.addr[i], sizeof(addr.addr[i]));
		}
	}
	else
	{
		// Invalid Address
		UB_LOG(UBL_INFO, "%s Wrong IPv6 format (%s) \n", __func__, ipv6str);
	}
	
}

void str_to_ipv4(uint8_t* buf, char* ipv4_str)
{
	sprintf(ipv4_str, "%d.%d.%d.%d", buf[0], buf[1], buf[2], buf[3]);
}

void split_str_into_arr(char* buf, char* delim, char** out, int expected_no)
{
	int counter = 0;
	char *p = strtok (buf, delim);
	while(p!= NULL && counter < expected_no)
	{
		out[counter++] = p;
        p = strtok (NULL, delim);
	}
}

static int ip4addr_aton(const char *cp, ip4_addr_t *addr)
{
  uint32_t val;
  uint8_t base;
  char c;
  uint32_t parts[4];
  uint32_t *pp = parts;

  c = *cp;
  for (;;) {
    /*
     * Collect number up to ``.''.
     * Values are specified as for C:
     * 0x=hex, 0=octal, 1-9=decimal.
     */
    if (!lldp_isdigit(c)) {
      return 0;
    }
    val = 0;
    base = 10;
    if (c == '0') {
      c = *++cp;
      if (c == 'x' || c == 'X') {
        base = 16;
        c = *++cp;
      } else {
        base = 8;
      }
    }
    for (;;) {
      if (lldp_isdigit(c)) {
        val = (val * base) + (uint32_t)(c - '0');
        c = *++cp;
      } else if (base == 16 && lldp_isxdigit(c)) {
        val = (val << 4) | (uint32_t)(c + 10 - (lldp_islower(c) ? 'a' : 'A'));
        c = *++cp;
      } else {
        break;
      }
    }
    if (c == '.') {
      /*
       * Internet format:
       *  a.b.c.d
       *  a.b.c   (with c treated as 16 bits)
       *  a.b (with b treated as 24 bits)
       */
      if (pp >= parts + 3) {
        return 0;
      }
      *pp++ = val;
      c = *++cp;
    } else {
      break;
    }
  }
  /*
   * Check for trailing characters.
   */
  if (c != '\0' && !lldp_isspace(c)) {
    return 0;
  }
  /*
   * Concoct the address according to
   * the number of parts specified.
   */
  switch (pp - parts + 1) {

    case 0:
      return 0;       /* initial nondigit */

    case 1:             /* a -- 32 bits */
      break;

    case 2:             /* a.b -- 8.24 bits */
      if (val > 0xffffffUL) {
        return 0;
      }
      if (parts[0] > 0xff) {
        return 0;
      }
      val |= parts[0] << 24;
      break;

    case 3:             /* a.b.c -- 8.8.16 bits */
      if (val > 0xffff) {
        return 0;
      }
      if ((parts[0] > 0xff) || (parts[1] > 0xff)) {
        return 0;
      }
      val |= (parts[0] << 24) | (parts[1] << 16);
      break;

    case 4:             /* a.b.c.d -- 8.8.8.8 bits */
      if (val > 0xff) {
        return 0;
      }
      if ((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff)) {
        return 0;
      }
      val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
      break;
    default:
      // LLDP_ASSERT("unhandled", 0);
      // Do nothing
      break;
  }
  if (addr) {
    ip4_addr_set_u32(addr, htonl(val));
  }
  return 1;
}

static int ip6addr_aton(const char *cp, ip6_addr_t *addr)
{
  uint32_t addr_index, zero_blocks, current_block_index, current_block_value;
  const char *s;
#if LLDP_IPV4
  int check_ipv4_mapped = 0;
#endif /* LLDP_IPV4 */

  /* Count the number of colons, to count the number of blocks in a "::" sequence
     zero_blocks may be 1 even if there are no :: sequences */
  zero_blocks = 8;
  for (s = cp; *s != 0; s++) {
    if (*s == ':') {
      zero_blocks--;
#if LLDP_IPV4
    } else if (*s == '.') {
      if ((zero_blocks == 5) ||(zero_blocks == 2)) {
        check_ipv4_mapped = 1;
        /* last block could be the start of an IPv4 address */
        zero_blocks--;
      } else {
        /* invalid format */
        return 0;
      }
      break;
#endif /* LLDP_IPV4 */
    } else if (!lldp_isxdigit(*s)) {
      break;
    }
  }

  /* parse each block */
  addr_index = 0;
  current_block_index = 0;
  current_block_value = 0;
  for (s = cp; *s != 0; s++) {
    if (*s == ':') {
      if (addr) {
        if (current_block_index & 0x1) {
          addr->addr[addr_index++] |= current_block_value;
        }
        else {
          addr->addr[addr_index] = current_block_value << 16;
        }
      }
      current_block_index++;
#if LLDP_IPV4
      if (check_ipv4_mapped) {
        if (current_block_index == 6) {
          ip4_addr_t ip4;
          int ret = ip4addr_aton(s + 1, &ip4);
          if (ret) {
            if (addr) {
              addr->addr[3] = htonl(ip4.addr);
              current_block_index++;
              goto fix_byte_order_and_return;
            }
            return 1;
          }
        }
      }
#endif /* LLDP_IPV4 */
      current_block_value = 0;
      if (current_block_index > 7) {
        /* address too long! */
        return 0;
      }
      if (s[1] == ':') {
        if (s[2] == ':') {
          /* invalid format: three successive colons */
          return 0;
        }
        s++;
        /* "::" found, set zeros */
        while (zero_blocks > 0) {
          zero_blocks--;
          if (current_block_index & 0x1) {
            addr_index++;
          } else {
            if (addr) {
              addr->addr[addr_index] = 0;
            }
          }
          current_block_index++;
          if (current_block_index > 7) {
            /* address too long! */
            return 0;
          }
        }
      }
    } else if (lldp_isxdigit(*s)) {
      /* add current digit */
      current_block_value = (current_block_value << 4) +
          (lldp_isdigit(*s) ? (uint32_t)(*s - '0') :
          (uint32_t)(10 + (lldp_islower(*s) ? *s - 'a' : *s - 'A')));
    } else {
      /* unexpected digit, space? CRLF? */
      break;
    }
  }

  if (addr) {
    if (current_block_index & 0x1) {
      addr->addr[addr_index++] |= current_block_value;
    }
    else {
      addr->addr[addr_index] = current_block_value << 16;
    }
#if LLDP_IPV4
fix_byte_order_and_return:
#endif
    /* convert to network byte order. */
    for (addr_index = 0; addr_index < 4; addr_index++) {
      addr->addr[addr_index] = htonl(addr->addr[addr_index]);
    }
#if LLDP_IPV6_SCOPES
    ip6_addr_clear_zone(addr);
 #endif
  }

  if (current_block_index != 7) {
    return 0;
  }

  return 1;
}

char *lldp_bmac2smac(ub_macaddr_t bmac, char *smac, const char delim)
{
	int i;
	for(i=0;i<5;i++){
		ub_byte2hexstr(bmac[i], &smac[i*3]);
		smac[(i*3)+2]=delim;
	}
	ub_byte2hexstr(bmac[5], &smac[5*3]);
	smac[17]=0;
	return smac;
}