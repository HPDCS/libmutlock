
int  lock_mutex_lock(   lock_mutex_t *impl, lock_context_t *me){ 	return 	DEC_LOCK(NAME)(impl);}
int  lock_mutex_trylock(lock_mutex_t *impl, lock_context_t *me){ 	return 	DEC_LOCK(NAME)(impl);}
void lock_mutex_unlock( lock_mutex_t *impl, lock_context_t *me){ 			DEC_UNLO(NAME)(impl);}
int  lock_mutex_destroy(lock_mutex_t *lock){ 						return 	DEC_DEST(NAME)(lock);}


inline void null_abort(){     fprintf(stderr, "Error cond_var not supported."); exit(1);}
int  lock_cond_init(lock_cond_t *cond, const pthread_condattr_t *attr){null_abort(); return 0;}
int  lock_cond_timedwait(lock_cond_t *cond, lock_mutex_t *lock, lock_context_t *me, const struct timespec *ts){null_abort(); return 0;}
int  lock_cond_wait(lock_cond_t *cond, lock_mutex_t *lock, lock_context_t *me){null_abort(); return 0;}
int  lock_cond_signal(lock_cond_t *cond){null_abort(); return 0;}
int  lock_cond_broadcast(lock_cond_t *cond){null_abort(); return 0;}
int  lock_cond_destroy(lock_cond_t *cond){null_abort(); return 0;}

void lock_thread_start(void){}
void lock_thread_exit(void){}
void lock_application_init(void){}
void lock_application_exit(void){}
