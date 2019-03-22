#include "sem_freq_mutlock.h"


#define NAME sem_freq



/*
 * Creates the mutable lock
 */
int sem_freq_mutlock_wrap_init(sem_freq_mutlock_wrapper_t* mutlock,  __HLOCK_PARAM_OBJ(NAME)* params) {
	if (!mutlock) return FAIL;
	
	__HLOCK_OBJ(NAME)* lock = (__HLOCK_OBJ(NAME)*)mutlock;
	MUTLOCK_INIT(lock);
	mutlock->inccnt = 0;
	mutlock->deccnt = 0;

	return SUCCESS;
}


/*
 * Lock routine for the input mutable lock
 */ 
int sem_freq_mutlock_wrap_lock(sem_freq_mutlock_wrapper_t* mutlock){
	if (!mutlock) return FAIL;
	
	int cntvar = 0;
	int slept = 0;
	int spun = 0;
	int updsws = 0;
	float eventfrq = 0.0;
	long long delta = 0;
	long long res = 0;
	unsigned long long sws = 0; 

	__HLOCK_OBJ(NAME)* lock = (__HLOCK_OBJ(NAME)*)mutlock;

	// START ACQUIRE
	res = INLINE_FUNC_NAME(NAME, __mut_lock)(lock);
	// END ACQUIRE
	
	slept  	= (res & SLEPT_MASK ) >> SLEPT_POS ;
	spun   	= (res & SPUN_MASK  ) >> SPUN_POS  ;
	
	updsws 	= (res & UPDSWS_MASK) >> UPDSWS_POS; 
	sws 	= res >> SWS_POS;
	
	if(updsws) 
		return SUCCESS;
		
	// arrived too late, increase heuristic count
	
	cntvar = (slept && !spun);
	lock->hcnt += cntvar;
	mutlock->inccnt += cntvar;
	
	// arrived earlier for lock acquisition, decrease heuristic count
	cntvar = (!slept && spun);
	lock->hcnt -= cntvar;
	mutlock->deccnt += cntvar;
	
	if (lock->hcnt == ((int)sws) * ((int)lock->inc_scale) ) {
		lock->hcnt = 0;
		
		eventfrq = mutlock->inccnt / (float)(mutlock->inccnt + mutlock->deccnt);
		// Increase the sws proportionally wrt to the frequency of the triggering event
		delta = sws * eventfrq;
		delta = (sws + delta <= lock->cores) ? (delta) : (lock->cores - sws);
		assert(sws + delta <= lock->cores);
		mutlock->inccnt = mutlock->deccnt = 0;
	}
	
	if (lock->hcnt == (-1) * ((int)sws) * ((int)lock->dec_scale) ) { // sws 0-relative or not??
		lock->hcnt = 0;

		eventfrq = mutlock->deccnt / (float)(mutlock->inccnt + mutlock->deccnt);
		// Decrease the sws proportionally wrt to the frequency of the triggering event
		delta = -(sws * eventfrq);
		delta = (sws + delta >= 1) ? (delta) : (-(sws - 1));
		assert(sws + delta >= 1); 
		mutlock->inccnt = mutlock->deccnt = 0;
	}

	if(delta != 0)
		INLINE_FUNC_NAME(NAME, __mut_apply_delta)(lock, delta);	

	return SUCCESS;
}

int sem_freq_mutlock_wrap_unlock(sem_freq_mutlock_wrapper_t* mutlock) {
	if (!mutlock) return FAIL;

	__HLOCK_OBJ(NAME)* lock = (__HLOCK_OBJ(NAME)*)mutlock;	
	INLINE_FUNC_NAME(NAME, __mut_unlock)(lock);	
	
	return SUCCESS;
}

int sem_freq_mutlock_wrap_destroy(sem_freq_mutlock_wrapper_t* mutlock) {
	if (!mutlock) return FAIL;
	return SUCCESS;	
}
