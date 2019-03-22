#ifndef _FREQ_MUTLOCK_H_
#define _FREQ_MUTLOCK_H_

#define NAME freq

#define freq_SPIN_API_INIT      __NOFIFO_SPIN_API_INIT
#define freq_SPIN_API_DESTROY   __NOFIFO_SPIN_API_DESTROY
#define freq_SPIN_API_LOCK      __NOFIFO_SPIN_API_LOCK
#define freq_SPIN_API_UNLOCK    __NOFIFO_SPIN_API_UNLOCK

#define freq_SLEEP_API_INIT     __FUTEX_API_INIT
#define freq_SLEEP_API_DESTROY  __FUTEX_API_DESTROY
#define freq_SLEEP_API_SLEEP    __FUTEX_API_SLEEP
#define freq_SLEEP_API_WAKEUP   __FUTEX_API_WAKEUP

#include "mutlocks_common.h"

TYPEDEF_FUTEX_OBJ(NAME);
TYPEDEF_NOFIFO_SPIN_OBJ(NAME);

#include "mutlocks_template.h"

struct freq_mutlock_wrapper {
	__HLOCK_OBJ(NAME) mtd; // common mutlock metadata
	int inccnt;		// counter for increase event observed
	int deccnt;		// counter for decrease event observed
};

typedef struct freq_mutlock_wrapper freq_mutlock_wrapper_t;
typedef struct freq_mutlock_wrapper freq_mutlock_wrapper_t;

int freq_mutlock_wrap_init(freq_mutlock_wrapper_t*, __HLOCK_PARAM_OBJ(NAME)*);
int freq_mutlock_wrap_lock(freq_mutlock_wrapper_t*);
int freq_mutlock_wrap_unlock(freq_mutlock_wrapper_t*);
int freq_mutlock_wrap_destroy(freq_mutlock_wrapper_t*);

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
