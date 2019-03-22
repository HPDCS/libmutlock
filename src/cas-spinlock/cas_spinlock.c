/*
 * cas-lock.c
 *
 *  Created on: Jun 13, 2017
 *      Author: davtir
 */

#include <cas_spinlock.h>

/*
 * Initialize tas lock
 */
int cas_spinlock_init(cas_spinlock_t* lock) {
	lock->locked = 0;

	return 1;
}

/*
 * Lock routine
 */
int cas_spinlock_lock(cas_spinlock_t* lock) {
	do {
		if (!lock->locked) {
			if (__sync_bool_compare_and_swap(&lock->locked, 0, 1)) {
				break;
			}
		}
	} while(1);

	return 1;
}

/*
 * Unlock routine
 */
int cas_spinlock_unlock(cas_spinlock_t* lock) {
	__sync_synchronize();
	lock->locked = 0;

	return 1;
}
