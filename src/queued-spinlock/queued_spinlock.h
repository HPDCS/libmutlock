#ifndef _QUEUED_LOCK_H_
#define _QUEUED_LOCK_H_

#include <node_basket.h>

struct queued_spinlock {
	struct queued_spinlock* volatile next;
	struct queued_spinlock* volatile list_pred;
	struct queued_spinlock* volatile owner;
	volatile int locked;
};

typedef struct queued_spinlock queued_spinlock_t;
typedef struct queued_spinlock qnode_t;


DECLARE_EXTERN_THREAD_LOCAL_NODES(qnode_t);


/**
 * \brief	Initialize the lock
 */
int queued_spinlock_init(queued_spinlock_t* mcs_lock);

/**
 * \brief	Lock routine
 */
int queued_spinlock_lock(queued_spinlock_t* mcs_lock);

/**
 * \brief	Unlock routine
 */
int queued_spinlock_unlock(queued_spinlock_t* mcs_lock);

/**
 * \brief	Must be called explicitly by the test in order to cleanup the bucket
 */
int queued_spinlock_destroy(queued_spinlock_t* mcs_lock);

#endif /* _QUEUED_LOCK_H_ */
