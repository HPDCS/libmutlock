#ifndef _NODE_BASKET_H_
#define _NODE_BASKET_H_

#include <stdbool.h>


#define LIST_SIZE 100

// Variables head and tail must be defined as thread-local variables for each thread

#define DECLARE_THREAD_LOCAL_NODES(NODE_TYPE)\
	 __thread NODE_TYPE* head = 0;\
	 __thread NODE_TYPE* tail = 0;\


#define DECLARE_EXTERN_THREAD_LOCAL_NODES(NODE_TYPE)\
	extern __thread NODE_TYPE* head;\
	extern __thread NODE_TYPE* tail;\


#define INIT_BASKET(NODE_TYPE)\
{\
	bool init_list = (tail == 0 && head == 0);\
	if (init_list) {\
		int i;\
		for (i = 0; i < LIST_SIZE; ++i) {\
			NODE_TYPE* node = (NODE_TYPE*)malloc(sizeof(NODE_TYPE));\
			PUSH_NODE(node);\
		}\
\
		init_list = false;\
	}\
}

// Pop from tail
#define POP_NODE(qnode)\
{\
	qnode = tail;\
	if (qnode != NULL) {\
		tail = qnode->list_pred;\
		if (head == qnode) {\
			head = 0;\
		}\
	}\
	else{\
		/*printf("NON CI SONO NODI NEL MCSLOCK\n");*/\
	}\
}

// Push on head
#define PUSH_NODE(node)\
{\
	node->list_pred = 0;\
	if (head != 0) {\
		head->list_pred = node;\
	}\
\
	head = node;\
\
	if (tail == 0) {\
		tail = node;\
	}\
}

#endif /* NODE_BASKET_H_ */
