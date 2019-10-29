#ifndef _MUTLOCKS_TEMPLATE_MULTIPLE_WAKEUP_H_
#define _MUTLOCKS_TEMPLATE_MULTIPLE_WAKEUP_H_

#if COND_VAR
#include "padding.h"
#include <errno.h>
#endif

typedef struct __HLOCK_STRUCT(NAME) {
#if COND_VAR
    pthread_mutex_t posix_lock;
    char __pad[pad_to_cache_line(sizeof(pthread_mutex_t))];
#endif
volatile unsigned long long lstate;  	/* 8*/
char pad1[64-sizeof(unsigned long long)];
__SPIN_OBJ(NAME) spin_obj;					/*12*/
char pad2[64-sizeof(__SPIN_OBJ(NAME))];
__SLEEP_OBJ(NAME) sleep_obj;					/*16*/
char pad3[64-sizeof(__SLEEP_OBJ(NAME))];
int wuc;								/*4*/
int hcnt;								/*8*/
char dec_scale;							/**/
char inc_scale;							/**/
short cores;							/*12*/
int trylocked;							/*16*/
int towakeup;							/*20*/
char pad4[64-5*sizeof(int)];
} __HLOCK_OBJ(NAME);


typedef struct __HLOCK_PARAM_STRUCT(NAME) {
	unsigned long long initial_sws;
}  __HLOCK_PARAM_OBJ(NAME);



DEF_INIT(NAME);
//DEF_DEST(NAME);
DEF_LOCK(NAME);
//DEF_UNLO(NAME);


#include <stdio.h>

#define MUTLOCK_INIT(lock) {MUTLOCK_INIT_SPECIFIC(lock, 1, 1, 1);}

#define MUTLOCK_INIT_SPECIFIC(lock, dec, inc, initsws)\
do {\
	SPIN_API_INIT(&lock->spin_obj);\
	SLEEP_API_INIT(&lock->sleep_obj);\
	lock->cores = sysconf(_SC_NPROCESSORS_ONLN);\
	lock->wuc = 0;\
	lock->hcnt = 0;\
	lock->dec_scale = dec;\
	lock->inc_scale = inc;\
	lock->lstate = ((unsigned long long)initsws) << 32;\
} while (0)


static inline long long INLINE_FUNC_NAME(NAME, __mut_lock)( __HLOCK_OBJ(NAME)* mutlock){

	long long lstate = 0;
	long long new_sws = 0;
	long long thc = 0;
	long long sws = 0;	
	long long spun = 0;	
	long long res = 0;
 	
 	long long trylocked = __NOFIFO_SPIN_API_TRYLOCK(&mutlock->spin_obj);
	if(!trylocked){
		// START ACQUIRE
		lstate = __sync_fetch_and_add(&mutlock->lstate, 1LL);
		sws = lstate >> 32;
		thc = (lstate & MASK_THC);

		assert(lstate > 0);
		assert(sws > 0);
		
		assert(thc >= 0);

		res = (thc >= sws);

			if (res) { SLEEP_API_SLEEP(&mutlock->sleep_obj);}
			spun = SPIN_API_LOCK(&mutlock->spin_obj);
	}
	mutlock->trylocked = trylocked;
//	if(spun)
//		printf("DAJEEEEEEEE\n");
	res |= (spun << SPUN_POS);

	lstate = mutlock->lstate;
	new_sws = lstate >> SWS_POS;
	thc |=  lstate & MASK_THC;
	
	res |= (thc == 1) << THC1_POS;
	res |= ((new_sws != sws) << UPDSWS_POS);
	res |= (sws << SWS_POS);
	// END ACQUIRE

	return res;

}


static inline int INLINE_FUNC_NAME(NAME, __mut_unlock)( __HLOCK_OBJ(NAME) *mutlock){
	long long lstate = 0;
	long long thc = 0;
	long long sws = 0;
	int wuc = 0;
	if(mutlock->trylocked){
		SPIN_API_UNLOCK(&mutlock->spin_obj);
		return SUCCESS;
	}
#ifndef NDEBUG
//	long long old_wuc = mutlock->wuc;
//	assert(old_wuc == -1 || old_wuc == 1 || old_wuc == 0 );
#endif
	if (mutlock->wuc > 0) {
//		assert(old_wuc == -1 || old_wuc == 1 || old_wuc == 0 );
		wuc = mutlock->wuc;
		mutlock->wuc = 0;
	}
	if (mutlock->wuc < 0) {
//		assert(old_wuc == -1 || old_wuc == 1 || old_wuc == 0 );
		wuc = -1;
		mutlock->wuc += 1;
	}
		
//	assert(wuc == -1 || wuc == 1 || wuc == 0 );

	lstate = __sync_fetch_and_add(&mutlock->lstate, -1LL);
	sws = lstate >> 32;
	thc = (lstate & MASK_THC);

	if(wuc == 0){
		mutlock->towakeup += (thc > sws);
		if(thc <= sws || ((sws >> 1) > mutlock->towakeup)){
			wuc = mutlock->towakeup;
			mutlock->towakeup = 0;
		}
	}
	SPIN_API_UNLOCK(&mutlock->spin_obj);

	if(wuc<0)
		assert(thc > sws);

	//if(wuc != 0)
	//	printf("WUC %d WUC %d THC %lld SWS %lld\n", wuc, wuc+(thc>sws), thc, sws);


	assert(lstate > 0);
	assert(sws > 0);
	
	assert(thc >= 0);

	assert(wuc >= 0 || (wuc < 0 && thc > sws));
	while (wuc > 0) {
		wuc -= SLEEP_API_WAKEUP(&mutlock->sleep_obj, wuc);
	}
	

	return SUCCESS;
}

static inline int INLINE_FUNC_NAME(NAME, __mut_apply_delta)( __HLOCK_OBJ(NAME)* mutlock, long long delta){

	long long lstate = 0;
	long long thc = 0;
	long long sws = 0;	
	long long tmp = 0;
	int deltamod = 0;
	
	if(delta == 0 || mutlock->wuc != 0 || mutlock->trylocked)
		return 0;



	tmp = delta;
	tmp <<= 32;

	lstate = __sync_fetch_and_add(&mutlock->lstate, tmp);
	
	sws = lstate >> 32;
	thc = lstate & MASK_THC;
	//printf("SWS OLD %lld SWS NEW %lld THC %lld ", sws, sws+delta, thc);

	assert(lstate > 0);
	assert(sws > 0);
	assert(sws+delta > 0);
	
	assert(thc >= 0);

	deltamod = (delta < 0) ? (-delta) : (delta);
	assert(deltamod > 0);

	tmp = LLONG_MAX;
	
	if (delta < 0 && thc > sws+delta) {
		tmp = thc - (sws + delta);
	}
	
	else if (delta > 0 && thc > sws) {
		tmp = thc - sws;
	}
	else{
		tmp = 0;
	}
	assert(tmp >= 0);

	tmp = (deltamod <= tmp) ? (deltamod) : (tmp);

	tmp = (delta > 0) ? (tmp) : (-tmp); 

	assert(mutlock->wuc == 0);

	mutlock->wuc += ((int)(tmp)) + (mutlock->towakeup);
	mutlock->towakeup = 0;
	//assert(mutlock->wuc == -1 || mutlock->wuc == 1 || mutlock->wuc == 0 );

	
#ifndef NDEBUG 
	if(mutlock->wuc == -1){
		assert(thc > sws+delta);
	}

	if(mutlock->wuc == 1){
		assert(thc > sws);
	}
#endif
	return 1;
}



/*
 * Unlock routine for the input mutable lock
 */
static inline int FUNC_NAME(NAME, _mutlock_unlock)(__HLOCK_OBJ(NAME)* mutlock){
	 return INLINE_FUNC_NAME(NAME, __mut_unlock)(mutlock);
}

static inline int FUNC_NAME(NAME, _mutlock_destroy)(__HLOCK_OBJ(NAME)* lock){
	if (!lock) return FAIL;
	return SUCCESS;
}




typedef __HLOCK_OBJ(NAME) 	 lock_mutex_t;


//typedef semtcpmutlock_context_t lock_context_t;
//typedef semtcpmutlock_cond_t lock_cond_t;
//typedef pthread_cond_t 		semtcpmutlock_cond_t;
//typedef void 				*semtcpmutlock_context_t; // Unused, take the less space as possible


typedef pthread_cond_t 		lock_cond_t;
typedef void 				*lock_context_t; // Unused, take the less space as possible

#define lock_mutex_create 	 __MYCAT2(PRVT_LOCK_TKN, _mutex_create	) //semtcpmutlock_mutex_create
#define lock_mutex_lock 	 __MYCAT2(PRVT_LOCK_TKN, _mutex_lock	)
#define lock_mutex_trylock   __MYCAT2(PRVT_LOCK_TKN, _mutex_trylock	)
#define lock_mutex_unlock 	 __MYCAT2(PRVT_LOCK_TKN, _mutex_unlock	)
#define lock_mutex_destroy   __MYCAT2(PRVT_LOCK_TKN, _mutex_destroy	)

#define lock_cond_init 			__MYCAT2(PRVT_LOCK_TKN, _cond_init				)	
#define lock_cond_timedwait 	__MYCAT2(PRVT_LOCK_TKN, _cond_timedwait		)			
#define lock_cond_wait 			__MYCAT2(PRVT_LOCK_TKN, _cond_wait				)	
#define lock_cond_signal 		__MYCAT2(PRVT_LOCK_TKN, _cond_signal			)		
#define lock_cond_broadcast 	__MYCAT2(PRVT_LOCK_TKN, _cond_broadcast		)			
#define lock_cond_destroy 		__MYCAT2(PRVT_LOCK_TKN, _cond_destroy			)		
#define lock_thread_start 		__MYCAT2(PRVT_LOCK_TKN, _thread_start			)		
#define lock_thread_exit 		__MYCAT2(PRVT_LOCK_TKN, _thread_exit			)		
#define lock_application_init 	__MYCAT2(PRVT_LOCK_TKN, _application_init		)			
#define lock_application_exit 	__MYCAT2(PRVT_LOCK_TKN, _application_exit		)			
#define lock_init_context 		__MYCAT2(PRVT_LOCK_TKN, _init_context			)		


lock_mutex_t *lock_mutex_create(const pthread_mutexattr_t *attr);
int  lock_mutex_lock(   lock_mutex_t *impl, lock_context_t *me);
int  lock_mutex_trylock(lock_mutex_t *impl, lock_context_t *me);
void lock_mutex_unlock( lock_mutex_t *impl, lock_context_t *me);
int  lock_mutex_destroy(lock_mutex_t *lock);


int  lock_cond_init(lock_cond_t *cond, const pthread_condattr_t *attr);
int  lock_cond_timedwait(lock_cond_t *cond, lock_mutex_t *lock, lock_context_t *me, const struct timespec *ts);
int  lock_cond_wait(lock_cond_t *cond, lock_mutex_t *lock, lock_context_t *me);
int  lock_cond_signal(lock_cond_t *cond);
int  lock_cond_broadcast(lock_cond_t *cond);
int  lock_cond_destroy(lock_cond_t *cond);

void lock_thread_start(void);
void lock_thread_exit(void);
void lock_application_init(void);
void lock_application_exit(void);


#endif
