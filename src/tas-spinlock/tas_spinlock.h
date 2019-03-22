/*
 * tas-lock.h
 *
 *  Created on: Jun 13, 2017
 *      Author: davtir
 */

#ifndef _TAS_LOCK_H_
#define _TAS_LOCK_H_

typedef struct tas_spinlock {
	volatile int locked;
} tas_spinlock_t;

/**
 * \brief	Initialize the lock
 */
int tas_spinlock_init(tas_spinlock_t* lock);

/**
 * \brief	Lock routine
 */
int tas_spinlock_lock(tas_spinlock_t* lock);

/**
 * \brief	Unlock routine
 */
int tas_spinlock_unlock(tas_spinlock_t* lock);

#endif /* _TAS_LOCK_H_ */
