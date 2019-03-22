#ifndef __SLEEP_UTILS__
#define __SLEEP_UTILS__


#include <semaphore.h>

#include <linux/futex.h>
#include <sys/syscall.h>

#include <unistd.h>

typedef int __futex_obj;
typedef sem_t __sem_obj;

/*
#if defined(__SLEEP_SEM)
typedef __sem_obj sleep_obj_t;
#define SLEEP_API_INIT     __SEM_API_INIT
#define SLEEP_API_DESTROY  __SEM_API_DESTROY
#define SLEEP_API_SLEEP    __SEM_API_SLEEP
#define SLEEP_API_WAKEUP   __SEM_API_WAKEUP
#elif defined(__SLEEP_FUTEX)
typedef __futex_obj sleep_obj_t;
#define SLEEP_API_INIT     __FUTEX_API_INIT
#define SLEEP_API_DESTROY  __FUTEX_API_DESTROY
#define SLEEP_API_SLEEP    __FUTEX_API_SLEEP
#define SLEEP_API_WAKEUP   __FUTEX_API_WAKEUP
#else
typedef __futex_obj sleep_obj_t;
#define SLEEP_API_INIT     __FUTEX_API_INIT
#define SLEEP_API_DESTROY  __FUTEX_API_DESTROY
#define SLEEP_API_SLEEP    __FUTEX_API_SLEEP
#define SLEEP_API_WAKEUP   __FUTEX_API_WAKEUP
#endif
*/

#define __FUTEX_API_INIT(futex)			{*(futex)=1;}
#define __FUTEX_API_DESTROY(futex)		{}
#define __FUTEX_API_SLEEP(futex)		syscall(SYS_futex, futex, FUTEX_WAIT, 1, NULL, NULL, NULL)
#define __FUTEX_API_WAKEUP(futex, n)	syscall(SYS_futex, futex, FUTEX_WAKE, n, NULL, NULL, NULL)


#define __SEM_API_INIT(sem)			sem_init(sem, 0, 0)
#define __SEM_API_DESTROY(sem)		sem_destroy(sem)
#define __SEM_API_SLEEP(sem) 		sem_wait(sem)
#define __SEM_API_WAKEUP(sem, n)	({int i=0; for(;i<(n);i++) {sem_post(sem);} n;})


#endif
