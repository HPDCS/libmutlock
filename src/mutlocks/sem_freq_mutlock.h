#ifndef _SEM_FREQ_MUTLOCK_H_
#define _SEM_FREQ_MUTLOCK_H_

#define NAME sem_freq

#define sem_freq_SPIN_API_INIT      __NOFIFO_SPIN_API_INIT
#define sem_freq_SPIN_API_DESTROY   __NOFIFO_SPIN_API_DESTROY
#define sem_freq_SPIN_API_LOCK      __NOFIFO_SPIN_API_LOCK
#define sem_freq_SPIN_API_UNLOCK    __NOFIFO_SPIN_API_UNLOCK

#define sem_freq_SLEEP_API_INIT     __SEM_API_INIT
#define sem_freq_SLEEP_API_DESTROY  __SEM_API_DESTROY
#define sem_freq_SLEEP_API_SLEEP    __SEM_API_SLEEP
#define sem_freq_SLEEP_API_WAKEUP   __SEM_API_WAKEUP

#include "mutlocks_common.h"

//TYPEDEF_FUTEX_OBJ(NAME);
TYPEDEF_SEM_OBJ(NAME);
TYPEDEF_NOFIFO_SPIN_OBJ(NAME);

#include "mutlocks_template.h"

struct sem_freq_mutlock_wrapper {
	__HLOCK_OBJ(NAME) mtd; // common mutlock metadata
	int inccnt;		// counter for increase event observed
	int deccnt;		// counter for decrease event observed
};

typedef struct sem_freq_mutlock_wrapper sem_freq_mutlock_wrapper_t;
typedef struct sem_freq_mutlock_wrapper sem_freq_mutlock_wrapper_t;

int sem_freq_mutlock_wrap_init(sem_freq_mutlock_wrapper_t*, __HLOCK_PARAM_OBJ(NAME)*);
int sem_freq_mutlock_wrap_lock(sem_freq_mutlock_wrapper_t*);
int sem_freq_mutlock_wrap_unlock(sem_freq_mutlock_wrapper_t*);
int sem_freq_mutlock_wrap_destroy(sem_freq_mutlock_wrapper_t*);

#undef NAME

/*



#define __NOFIFO_SPIN
#define __SLEEP_FUTEX

#include <mutlocks_common_obj.h>

struct freq_mutlock_param {
	unsigned long long initial_sws;
};

struct freq_mutlock {
	hmutlock_t mtd; // common mutlock metadata
	int inccnt;		// counter for increase event observed
	int deccnt;		// counter for decrease event observed
};

typedef struct freq_mutlock freq_mutlock_t;
typedef struct freq_mutlock_param freq_mutlock_param_t;

int freq_mutlock_init(freq_mutlock_t*, freq_mutlock_param_t* params);
int freq_mutlock_lock(freq_mutlock_t*);
int freq_mutlock_unlock(freq_mutlock_t*);
int freq_mutlock_destroy(freq_mutlock_t*);
*/

#endif
