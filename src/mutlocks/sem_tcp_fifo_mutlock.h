#ifndef _SEM_TCP_FIFO_FIN_MUTLOCK_H_
#define _SEM_TCP_FIFO_FIN_MUTLOCK_H_

#define LOCK_ALGORITHM "SEMTCPFIFOMUTLOCK"
#define NEED_CONTEXT 0
#define SUPPORT_WAITING 0

#define PRVT_LOCK_TKN semtcpfifomutlock
#define PRVT_LOCK_NAME #semtcpfifomutlock

#define NAME sem_tcp_fifo

#define sem_tcp_fifo_SPIN_API_INIT      __FIFO_SPIN_API_INIT
#define sem_tcp_fifo_SPIN_API_DESTROY   __FIFO_SPIN_API_DESTROY
#define sem_tcp_fifo_SPIN_API_LOCK      __FIFO_SPIN_API_LOCK
#define sem_tcp_fifo_SPIN_API_UNLOCK    __FIFO_SPIN_API_UNLOCK

#define sem_tcp_fifo_SLEEP_API_INIT     __SEM_API_INIT
#define sem_tcp_fifo_SLEEP_API_DESTROY  __SEM_API_DESTROY
#define sem_tcp_fifo_SLEEP_API_SLEEP    __SEM_API_SLEEP
#define sem_tcp_fifo_SLEEP_API_WAKEUP   __SEM_API_WAKEUP

#include "mutlocks_common.h"

TYPEDEF_SEM_OBJ(NAME);
//TYPEDEF_FUTEX_OBJ(NAME);
TYPEDEF_FIFO_SPIN_OBJ(NAME);
//TYPEDEF_NOFIFO_SPIN_OBJ(NAME);

#include "mutlocks_template.h"
#undef NAME

/*
#define prova_SPIN_API_INIT     __FIFO_SPIN_API_INIT
#define prova_SPIN_API_DESTROY  __FIFO_SPIN_API_DESTROY
#define prova_SPIN_API_LOCK     __FIFO_SPIN_API_LOCK
#define prova_SPIN_API_UNLOCK   __FIFO_SPIN_API_UNLOCK

#define prova_SLEEP_API_INIT     __SEM_API_INIT
#define prova_SLEEP_API_DESTROY  __SEM_API_DESTROY
#define prova_SLEEP_API_SLEEP    __SEM_API_SLEEP
#define prova_SLEEP_API_WAKEUP   __SEM_API_WAKEUP
*/


#endif /* _ONEVENT_NOFIFO_MUTLOCK_H_ */
