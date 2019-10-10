#ifndef __MUTLOCKS_COMMON_OBJ__
#define __MUTLOCKS_COMMON_OBJ__

#ifndef NDEBUG
#define NDEBUG
#endif

#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <sleep_utils.h>
#include <spin_utils.h>


#define MASK_SWS 0xffffffff00000000ULL
#define MASK_THC 0x00000000ffffffffULL

#define SLEPT_POS  	 0
#define SPUN_POS   	 1
#define UPDSWS_POS 	 2
#define THC1_POS 	 3
#define SWS_POS 	32
 
#define SLEPT_MASK  (1 << SLEPT_POS )
#define SPUN_MASK   (1 << SPUN_POS  )
#define UPDSWS_MASK (1 << UPDSWS_POS)
#define THC1_MASK   (1 << THC1_POS  )

//#define __CONCAT(x,y) x##y
#define __MYCAT(a, ...)	    a ## __VA_ARGS__
#define __MYPRE(a, ...)	    __VA_ARGS__ ## a
#define __MYCAT2(a, ...)	__MYCAT(a, __VA_ARGS__)	
#define __MYCAT3(a, ...)	__MYPRE(__VA_ARGS__, a)	


#define __SLEEP_OBJ(NAME) __CONCAT(NAME,_sleep_obj_t)
#define __SPIN_OBJ(NAME)  __CONCAT(NAME,_spin_obj_t)
#define __HLOCK_OBJ(NAME)  __CONCAT(NAME,_mutlock_t)
#define __HLOCK_STRUCT(NAME)  __CONCAT(NAME,_mutlock)
#define __HLOCK_PARAM_STRUCT(NAME)  __CONCAT(NAME,_mutlock_param)
#define __HLOCK_PARAM_OBJ(NAME)  __CONCAT(NAME,_mutlock_param_t)

#define TYPEDEF_SEM_OBJ(NAME)  typedef __sem_obj __SLEEP_OBJ(NAME)
#define TYPEDEF_FUTEX_OBJ(NAME)  typedef __futex_obj __SLEEP_OBJ(NAME)

#define TYPEDEF_FIFO_SPIN_OBJ(NAME)  typedef __fifo_spin_obj __SPIN_OBJ(NAME)
#define TYPEDEF_NOFIFO_SPIN_OBJ(NAME)  typedef __nofifo_spin_obj __SPIN_OBJ(NAME)

#define FUNC_NAME(NAME, STR)	__CONCAT(NAME, STR)
#define PREFIX(NAME)			__CONCAT(__, NAME)
#define INLINE_FUNC_NAME(NAME, STR)	FUNC_NAME(PREFIX(NAME), STR)

#define DEF_INIT(NAME) int __CONCAT(NAME, _mutlock_init)(__HLOCK_OBJ(NAME)*, __HLOCK_PARAM_OBJ(NAME)*)
#define DEF_DEST(NAME) int __CONCAT(NAME, _mutlock_destroy)(__HLOCK_OBJ(NAME)*)
#define DEF_LOCK(NAME) int __CONCAT(NAME, _mutlock_lock)(__HLOCK_OBJ(NAME)*)
#define DEF_UNLO(NAME) int __CONCAT(NAME, _mutlock_unlock)(__HLOCK_OBJ(NAME)*)

#define DEC_INIT(NAME) __CONCAT(NAME, _mutlock_init)
#define DEC_DEST(NAME) __CONCAT(NAME, _mutlock_destroy)
#define DEC_LOCK(NAME) __CONCAT(NAME, _mutlock_lock)
#define DEC_UNLO(NAME) __CONCAT(NAME, _mutlock_unlock)


#define API_NAME(NAME, STR)	__CONCAT(NAME,_ ## STR)
#define SPIN_API_INIT      FUNC_NAME(NAME,_SPIN_API_INIT    ) 
#define SPIN_API_DESTROY   FUNC_NAME(NAME,_SPIN_API_DESTROY ) 
#define SPIN_API_LOCK      FUNC_NAME(NAME,_SPIN_API_LOCK    ) 
#define SPIN_API_UNLOCK    FUNC_NAME(NAME,_SPIN_API_UNLOCK  ) 
#define SLEEP_API_INIT     FUNC_NAME(NAME,_SLEEP_API_INIT   ) 
#define SLEEP_API_DESTROY  FUNC_NAME(NAME,_SLEEP_API_DESTROY) 
#define SLEEP_API_SLEEP    FUNC_NAME(NAME,_SLEEP_API_SLEEP  ) 
#define SLEEP_API_WAKEUP   FUNC_NAME(NAME,_SLEEP_API_WAKEUP ) 


#define SUCCESS 1
#define FAIL 0

/* accurate time measurements on late recent cpus */
static inline unsigned long long __attribute__((always_inline))
read_tsc_p()
{
	unsigned long long tsc;
	asm volatile ("rdtscp\n"
		"shl $32, %%rdx\n"
		"or %%rdx, %%rax"
		: "=a"(tsc)
		:
		: "%rcx", "%rdx");
	return tsc;
}

#endif

