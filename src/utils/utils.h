#ifndef _ATOMICS_H_
#define _ATOMICS_H_

#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>

/*
 * GCC atomic builtins redefinitions
 */
#define fence() 				__sync_synchronize()
#define atomic_store(ptr, val)			__sync_lock_test_and_set(ptr, val)
#define atomic_swap(ptr, val)			__sync_lock_test_and_set(ptr, val)
#define atomic_cas(ptr, oldval, newval)		__sync_bool_compare_and_swap(ptr, oldval, newval)
#define atomic_add(ptr, val)			 __sync_fetch_and_add(ptr, val)
#define atomic_sub(ptr, val)			 __sync_fetch_and_sub(ptr, val)
#define atomic_or(ptr, val)			 __sync_fetch_and_or(ptr, val)
#define atomic_and(ptr, val)			 __sync_fetch_and_and(ptr, val)
#define atomic_xor(ptr, val)			 __sync_fetch_and_xor(ptr, val)
#define atomic_nand(ptr, val)			 __sync_fetch_and_nand(ptr, val)

#define full_atomic_store(ptr, val) \
{\
	fence();\
	atomic_store(ptr, val);\
}

/*
 * Wait/Wake facilities
 */
#define futex_wake(futex, n)			syscall(SYS_futex, futex, FUTEX_WAKE, n, NULL, NULL, NULL)

#define abort_wait_and_wake(futex, abortval, n)\
{\
	atomic_store(futex, abortval);\
	futex_wake(futex, n);\
}

#define futex_wait(futex, val)\
{\
	if (*futex == val) {\
		syscall(SYS_futex, futex, FUTEX_WAIT, val, NULL, NULL, NULL);\
	}\
}


/*
 * Logging facilities
 */
 #ifdef MUTLOCK_DEBUG
#define LOG_DEBUG(MSG, ...)\
do {\
	char msg[1024];\
	sprintf(msg, "[DEBUG] (%d) %s - [TID: %ld] %s\n", __LINE__, __PRETTY_FUNCTION__, syscall(SYS_gettid), MSG);\
	printf (msg, ##__VA_ARGS__);\
	fflush(stdout);\
} while(0)

#else
#define LOG_DEBUG(MSG, ...) ;
#endif

#define LOG_INFO(MSG, ...)\
do {\
	char msg[1024];\
	sprintf(msg, "[INFO] %s - [TID: %ld] %s\n", __PRETTY_FUNCTION__, syscall(SYS_gettid), MSG);\
	printf (msg, ##__VA_ARGS__);\
	fflush(stdout);\
} while(0)


#endif /* _ATOMICS_H_ */
