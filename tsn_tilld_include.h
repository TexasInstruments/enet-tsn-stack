/************************************************************************
 *  Copyright 2023 Excelfore Corporation (https://excelfore.com)
 *  All rights reserved
 *
 *  Licensed under the Excelfore License Agreement ("License");
 *  You may not use this file except in compliance with the License.
 *  You may obtain a copy of the License by contacting legal@excelfore.com
 *
 ************************************************************************/
#ifndef __TSN_TILLD_INCLUDE_H_
#define __TSN_TILLD_INCLUDE_H_

// definitions to use the combase headers
#define COMBASE_NO_INET
#define COMBASE_NO_CRC
#define COMBASE_NO_XTIMER
#define COMBASE_NO_IPCSOCK
#define CB_ETHERNET_NON_POSIX_H "tsn_combase/tilld/cb_lld_ethernet.h"
#define CB_THREAD_NON_POSIX_H "tsn_combase/tilld/cb_lld_thread.h"
#define CB_IPCSHMEM_NON_POSIX_H "tsn_combase/tilld/cb_lld_ipcshmem.h"
#define CB_EVENT_NON_POSIX_H "tsn_combase/tilld/cb_lld_tmevent.h"

#endif /* __TSN_TILLD_INCLUDE_H_ */
