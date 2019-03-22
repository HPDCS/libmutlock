/*
 * tas-lock.c
 *
 *  Created on: Jun 13, 2017
 *      Author: davtir
 */

#include <tas_spinlock.h>

/*
 * Initialize tas lock
 */
int tas_spinlock_init(tas_spinlock_t* lock) {
	lock->locked = 0;

	return 1;
}

/*
 * Lock routine
 */
int tas_spinlock_lock(tas_spinlock_t* lock) {
	int value = 0;
	do {
		value = lock->locked;
		if (!value) {
			value = __sync_lock_test_and_set(&lock->locked, 1);
		}
	} while(value);

	return 1;
}

/*
 * Unlock routine
 */
int tas_spinlock_unlock(tas_spinlock_t* lock) {
	__sync_synchronize();
	lock->locked = 0;

	return 1;
}
