//#include <stdlib.h>
//#include <assert.h>



#if __has_include("sem_tcp_multiple_wakeup_mutlock.h")
#include "sem_tcp_multiple_wakeup_mutlock.h"
#define NAME sem_tcp_multiple_wakeup
#else
#include "semtcpmultiplewakeupmutlock.h"
#include "utils.h"
#include "interpose.h"
#define NAME sem_tcp_multiple_wakeup
#include "mutlocks_template_multiplewakeup.h"
#endif




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
	long long ssws;
	
	// START ACQUIRE
	res = INLINE_FUNC_NAME(NAME, __mut_lock)(mutlock);
	// END ACQUIRE

	slept  	= (res & SLEPT_MASK ) >> SLEPT_POS ;
	spun   	= (res & SPUN_MASK  ) >> SPUN_POS  ;
	
	updsws 	= (res & UPDSWS_MASK) >> UPDSWS_POS; 
	sws 	= res >> SWS_POS;

	if(updsws) 
		return SUCCESS;

	ssws = sws;
	// arrived too late, increase heuristic count
	mutlock->hcnt += !(slept && !spun);

	
	// arrived earlier for lock acquisition, decrease heuristic count
	//mutlock->hcnt -= (!slept && spun);

	assert(sws > 0);
	assert(mutlock->hcnt <= mutlock->cores);
	assert(mutlock->hcnt >= -mutlock->cores);
	
	if (slept && !spun) {
		if(sws != mutlock->cores){
			mutlock->hcnt = 0;
			delta = sws;
			delta = ( (sws + delta) <= mutlock->cores) ? delta : (mutlock->cores-sws);
		} 
		else if(sws == mutlock->cores){
			mutlock->hcnt = 0;
			delta = -ssws+1;
		} 
		
	}
	if (mutlock->hcnt == 10 ) { // sws 0-relative or not??
		mutlock->hcnt = 0;
		delta = -(sws > 1);
	}
	
	if(delta != 0)
		INLINE_FUNC_NAME(NAME, __mut_apply_delta)(mutlock, delta);	


	return SUCCESS;

}