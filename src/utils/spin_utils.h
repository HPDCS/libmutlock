#ifndef __SPIN_UTILS__
#define __SPIN_UTILS__

#include <queued_spinlock.h>
#include <node_basket.h>


typedef volatile int __nofifo_spin_obj;
typedef queued_spinlock_t __fifo_spin_obj;

/*
#if defined(__FIFO_SPIN)
typedef __fifo_spin_obj spin_obj_t;
#define SPIN_API_INIT     __FIFO_SPIN_API_INIT
#define SPIN_API_DESTROY  __FIFO_SPIN_API_DESTROY
#define SPIN_API_LOCK     __FIFO_SPIN_API_LOCK
#define SPIN_API_UNLOCK   __FIFO_SPIN_API_UNLOCK
#elif defined(__NOFIFO_SPIN)
typedef __nofifo_spin_obj spin_obj_t;
#define SPIN_API_INIT     __NOFIFO_SPIN_API_INIT
#define SPIN_API_DESTROY  __NOFIFO_SPIN_API_DESTROY
#define SPIN_API_LOCK     __NOFIFO_SPIN_API_LOCK
#define SPIN_API_UNLOCK   __NOFIFO_SPIN_API_UNLOCK
#else
typedef __nofifo_spin_obj spin_obj_t;
#define SPIN_API_INIT     __NOFIFO_SPIN_API_INIT
#define SPIN_API_DESTROY  __NOFIFO_SPIN_API_DESTROY
#define SPIN_API_LOCK     __NOFIFO_SPIN_API_LOCK
#define SPIN_API_UNLOCK   __NOFIFO_SPIN_API_UNLOCK
#endif
*/

#define __FIFO_SPIN_API_INIT(obj)		{(obj)->next=NULL;(obj)->locked=0;__sync_synchronize();}
#define __FIFO_SPIN_API_DESTROY(obj)	{}
#define __FIFO_SPIN_API_LOCK(obj) 		__fifo_spin_lock(obj)
#define __FIFO_SPIN_API_UNLOCK(obj)		__fifo_spin_unlock(obj)


#define __NOFIFO_SPIN_API_INIT(obj)			{ *((__nofifo_spin_obj*)obj) = 0;}
#define __NOFIFO_SPIN_API_DESTROY(obj)		{}
#define __NOFIFO_SPIN_API_LOCK(obj)			__nofifo_spin_lock(obj)
#define __NOFIFO_SPIN_API_TRYLOCK(obj)			__nofifo_spin_trylock(obj)
#define __NOFIFO_SPIN_API_UNLOCK(obj)		__sync_bool_compare_and_swap(obj, 1, 0);


static inline long long __nofifo_spin_trylock(__nofifo_spin_obj* obj){
	return __sync_bool_compare_and_swap(obj, 0, 1);
}

static inline long long __nofifo_spin_lock(__nofifo_spin_obj* obj){

	long long res = 0;
	__nofifo_spin_obj value;
	
	// START ACQUIRE
	if( !__sync_bool_compare_and_swap(obj, 0, 1) ) {
		res = 1LL;
		do {
			do {
				value = *obj;
			} while (value);
		} while ( !__sync_bool_compare_and_swap(obj, 0, 1) );
	}
	// END ACQUIRE
	return res;
}



static inline long long __fifo_spin_lock(__fifo_spin_obj* obj) {
	qnode_t* prev = 0;
	qnode_t* qnode = 0;
	
	INIT_BASKET(qnode_t);
	
	// pop operation from list, if tail equals head then realloc is needed
	POP_NODE(qnode);
	
	qnode->locked = 0;
	qnode->next = 0;

	__sync_synchronize();
	prev = __sync_lock_test_and_set(&obj->next, qnode);
	if (prev == NULL) {
		// Lock acquired
		obj->owner = qnode;
		return 0LL;
	}

	prev->next = qnode;

	__sync_synchronize();

	// Spin locally
	while (!qnode->locked);
	
	// Set cs ownership
	obj->owner = qnode;

	// Lock acquired
	return 1LL;
}

static inline void __fifo_spin_unlock(queued_spinlock_t* obj) {
	struct queued_spinlock* next = NULL;
	
	qnode_t* qnode = obj->owner;

	if (obj == NULL) {
		// Invalid pointer
		return;
	}

	__sync_synchronize();
	next = qnode->next;

	if (!next) {
		if (__sync_bool_compare_and_swap(&obj->next, qnode, NULL)) {
			PUSH_NODE(qnode);
			return;	// The lock was not contended
		}

		while (!next) {
			next = qnode->next;
		}
	}

	__sync_synchronize();
	next->locked = 1;
	
	// push the node, now it can be used again
	PUSH_NODE(qnode);
	
	return;
}


#endif
