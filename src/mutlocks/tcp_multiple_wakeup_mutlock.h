#ifndef _TCP_FIN_MULTIPLE_WAKEUP_MUTLOCK_H_
#define _TCP_FIN_MULTIPLE_WAKEUP_MUTLOCK_H_

#define LOCK_ALGORITHM "TCPMULTIPLEWAKEUPMUTLOCK"
#define NEED_CONTEXT 0
#define SUPPORT_WAITING 0

#define PRVT_LOCK_TKN tcpmultiplewakeupmutlock
#define PRVT_LOCK_NAME #tcpmultiplewakeupmutlock

#define NAME tcp_multiple_wakeup

#define tcp_multiple_wakeup_SPIN_API_INIT      __NOFIFO_SPIN_API_INIT
#define tcp_multiple_wakeup_SPIN_API_DESTROY   __NOFIFO_SPIN_API_DESTROY
#define tcp_multiple_wakeup_SPIN_API_LOCK      __NOFIFO_SPIN_API_LOCK
#define tcp_multiple_wakeup_SPIN_API_UNLOCK    __NOFIFO_SPIN_API_UNLOCK

#define tcp_multiple_wakeup_SLEEP_API_INIT     __FUTEX_API_INIT
#define tcp_multiple_wakeup_SLEEP_API_DESTROY  __FUTEX_API_DESTROY
#define tcp_multiple_wakeup_SLEEP_API_SLEEP    __FUTEX_API_SLEEP
#define tcp_multiple_wakeup_SLEEP_API_WAKEUP   __FUTEX_API_WAKEUP

#include "mutlocks_common.h"

TYPEDEF_FUTEX_OBJ(NAME);
TYPEDEF_NOFIFO_SPIN_OBJ(NAME);

#include "mutlocks_template_multiplewakeup.h"
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


#endif /* _TCP_FIN_MULTIPLE_WAKEUP_MUTLOCK_H_ */
