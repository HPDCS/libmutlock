/*
 * cas-lock.h
 *
 *  Created on: Jun 13, 2017
 *      Author: davtir
 */

#ifndef _CAS_LOCK_H_
#define _CAS_LOCK_H_

typedef struct cas_spinlock {
	volatile int locked;
} cas_spinlock_t;

/**
 * \brief	Initialize the lock
 */
int cas_spinlock_init(cas_spinlock_t* lock);

/**
 * \brief	Lock routine
 */
int cas_spinlock_lock(cas_spinlock_t* lock);

/**
 * \brief	Unlock routine
 */
int cas_spinlock_unlock(cas_spinlock_t* lock);

#endif /* _CAS_LOCK_H_ */
