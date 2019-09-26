//#include <stdlib.h>
//#include <assert.h>

#if __has_include("sem_tcp_mutlock.h")
#include "sem_tcp_mutlock.h"
#else
#include "semtcpmutlock.h"
#include "utils.h"
#include "interpose.h"
#endif

#define NAME sem_tcp



/*
 * Creates the mutable lock
 */

int DEC_INIT(NAME)(__HLOCK_OBJ(NAME)* mutlock, __HLOCK_PARAM_OBJ(NAME)* params) {
	if (!mutlock) return FAIL;
	
	MUTLOCK_INIT(mutlock);


	return SUCCESS;
}

/*
 * Lock routine for the input mutable lock
 */ 
int DEC_LOCK(NAME)(__HLOCK_OBJ(NAME)* mutlock){
	
	int slept = 0;
	int spun = 0;

	int updsws = 0;
	long long delta = 0;
	long long res = 0;
	unsigned long long sws = 0; 
	
	// START ACQUIRE
	res = INLINE_FUNC_NAME(NAME, __mut_lock)(mutlock);
	// END ACQUIRE

	slept  	= (res & SLEPT_MASK ) >> SLEPT_POS ;
	spun   	= (res & SPUN_MASK  ) >> SPUN_POS  ;
	
	updsws 	= (res & UPDSWS_MASK) >> UPDSWS_POS; 
	sws 	= res >> SWS_POS;

	if(updsws) 
		return SUCCESS;

	// arrived too late, increase heuristic count
	mutlock->hcnt += !(slept && !spun);

	
	// arrived earlier for lock acquisition, decrease heuristic count
	//mutlock->hcnt -= (!slept && spun);

	assert(sws > 0);
	assert(mutlock->hcnt <= mutlock->cores);
	assert(mutlock->hcnt >= -mutlock->cores);
	
	if (slept && !spun) {
		mutlock->hcnt = 0;
		delta = sws;
		delta = ( (sws + delta) <= mutlock->cores) ? delta : (mutlock->cores-delta);
	}
	if (mutlock->hcnt == 10 ) { // sws 0-relative or not??
		mutlock->hcnt = 0;
		delta = -(sws > 1);
	}
	
	if(delta != 0)
		INLINE_FUNC_NAME(NAME, __mut_apply_delta)(mutlock, delta);	


	return SUCCESS;

}

#if __has_include("semtcpmutlock.h")
lock_mutex_t *lock_mutex_create(const pthread_mutexattr_t *attr){
    lock_mutex_t *impl = (lock_mutex_t *)alloc_cache_align(sizeof(lock_mutex_t));
	sem_tcp_mutlock_param_t sem_tcp_params;
	sem_tcp_params.initial_sws = 1;
	sem_tcp_mutlock_init(impl, &sem_tcp_params); // attiva euristica
	#if COND_VAR
    REAL(pthread_mutex_init)(&impl->posix_lock, /*&errattr */ attr);
    DEBUG("Mutex init lock=%p posix_lock=%p\n", impl, &impl->posix_lock);
	#endif

	return impl;
}
#endif

#if !__has_include("sem_tcp_mutlock.h")
#include "mutlocks_template_ending.h"
#endif
