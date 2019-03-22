//#include <stdlib.h>
//#include <assert.h>

#include "thc1_mutlock.h"

#define NAME thc1



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
	int thc1 = 0;
	int updsws = 0;
	long long delta = 0;
	long long res = 0;
	unsigned long long sws = 0; 
	
	// START ACQUIRE
	res = INLINE_FUNC_NAME(NAME, __mut_lock)(mutlock);
	// END ACQUIRE

	slept  	= (res & SLEPT_MASK ) >> SLEPT_POS ;
	spun   	= (res & SPUN_MASK  ) >> SPUN_POS  ;
	thc1 	= (res & THC1_MASK)   >> THC1_POS  ;
	updsws 	= (res & UPDSWS_MASK) >> UPDSWS_POS; 
	sws 	= res >> SWS_POS;

	if(updsws) 
		return SUCCESS;

	// arrived too late, increase heuristic count
	mutlock->hcnt += (slept && !spun);
	
	// arrived earlier for lock acquisition, decrease heuristic count
	mutlock->hcnt -= (thc1);

	assert(sws > 0);
	assert(mutlock->hcnt <= mutlock->cores);
	assert(mutlock->hcnt >= -mutlock->cores);
	if (mutlock->hcnt == ((int)sws) * ((int)mutlock->inc_scale) ) {
		mutlock->hcnt = 0;
		delta = (sws < mutlock->cores);
	}
	if (mutlock->hcnt == (-1) * ((int)sws) * ((int)mutlock->dec_scale) ) { // sws 0-relative or not??
		mutlock->hcnt = 0;
		delta = -(sws > 1);
	}
	
	if(delta != 0)
		INLINE_FUNC_NAME(NAME, __mut_apply_delta)(mutlock, delta);	


	return SUCCESS;

}

