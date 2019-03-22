#include <queued_spinlock.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <node_basket.h>

DECLARE_THREAD_LOCAL_NODES(qnode_t);

int queued_spinlock_init(queued_spinlock_t* queued_lock) {
	queued_lock->next = 0L;
	queued_lock->locked = 0;

	return 1;
}

int queued_spinlock_lock(queued_spinlock_t* queued_lock) {
	qnode_t* prev = 0;
	qnode_t* qnode = 0;
	if (queued_lock == 0L) {
		// Invalid pointer
		return -1;
	}
	
	INIT_BASKET(qnode_t);
	
	// pop operation from list, if tail equals head then realloc is needed
	POP_NODE(qnode);
	
	qnode->locked = 0;
	qnode->next = 0;

	__sync_synchronize();
	prev = __sync_lock_test_and_set(&queued_lock->next, qnode);
	if (prev == 0L) {
		// Lock acquired
		queued_lock->owner = qnode;
		return 1;
	}

	prev->next = qnode;

	__sync_synchronize();

	// Spin locally
	while (!qnode->locked);
	
	// Set cs ownership
	queued_lock->owner = qnode;

	// Lock acquired
	return 1;
}

int queued_spinlock_unlock(queued_spinlock_t* queued_lock) {
	struct queued_spinlock* next = 0L;
	
	qnode_t* qnode = queued_lock->owner;

	if (queued_lock == 0L) {
		// Invalid pointer
		return -1;
	}

	__sync_synchronize();
	next = qnode->next;

	if (!next) {
		if (__sync_bool_compare_and_swap(&queued_lock->next, qnode, 0L)) {
			PUSH_NODE(qnode);
			return 1;	// The lock was not contended
		}

		while (!next) {
			next = qnode->next;
		}
	}

	__sync_synchronize();
	next->locked = 1;
	
	// push the node, now it can be used again
	PUSH_NODE(qnode);
	
	return 1;
}

int queued_spinlock_destroy(queued_spinlock_t* mcs_lock) {
	return 0;
}
