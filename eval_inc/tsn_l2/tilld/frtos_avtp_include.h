#ifndef __FRTOS_AVTP_INCLUDE_H_
#define __FRTOS_AVTP_INCLUDE_H_

#include <tsn_combase/combase.h>

extern CB_SEM_T g_avtpd_ready_sem; // in avtpd.c

#define WAIT_AVTPD_READY wait_avtpd_read()
static inline void wait_avtpd_read(void)
{
	UB_LOG(UBL_INFO,"waiting avtpd\n");
	CB_SEM_WAIT(&g_avtpd_ready_sem);
	// to run 2 avtp_testclient, hit the same semaphore one more time
	CB_SEM_POST(&g_avtpd_ready_sem);
	UB_LOG(UBL_INFO,"avtpd is ready\n");
}

#endif
