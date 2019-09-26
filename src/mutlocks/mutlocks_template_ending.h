
int  lock_mutex_lock(   lock_mutex_t *impl, lock_context_t *me){
 	return DEC_LOCK(NAME)(impl);

}

int  lock_mutex_trylock(lock_mutex_t *impl, lock_context_t *me){
 	fprintf(stderr, "Error cond_var not supported."); 
 	exit(1); 
 	return 0;
 }

void lock_mutex_unlock( lock_mutex_t *impl, lock_context_t *me){ 			DEC_UNLO(NAME)(impl);}
int  lock_mutex_destroy(lock_mutex_t *lock){ 						
	#if COND_VAR
	    REAL(pthread_mutex_destroy)(&lock->posix_lock);
	#endif
	return 	DEC_DEST(NAME)(lock);
}


inline void null_abort(){     fprintf(stderr, "Error cond_var not supported."); exit(1);}

int  lock_cond_init(lock_cond_t *cond, const pthread_condattr_t *attr){
	#if COND_VAR
    	return REAL(pthread_cond_init)(cond, attr);
	#else
		null_abort(); 
		return 0;
	#endif
}

int  lock_cond_timedwait(lock_cond_t *cond, lock_mutex_t *lock, lock_context_t *me, const struct timespec *ts){
	#if COND_VAR
	    int res;

	    DEC_UNLO(NAME)(lock);
	    DEBUG("[%d] Sleep cond=%p lock=%p posix_lock=%p\n", cur_thread_id, cond,
	          lock, &(lock->posix_lock));
	    DEBUG_PTHREAD("[%d] Cond posix = %p lock = %p\n", cur_thread_id, cond,
	                  &lock->posix_lock);

	    if (ts)
	        res = REAL(pthread_cond_timedwait)(cond, &lock->posix_lock, ts);
	    else
	        res = REAL(pthread_cond_wait)(cond, &lock->posix_lock);

	    if (res != 0 && res != ETIMEDOUT) {
	        fprintf(stderr, "Error on cond_{timed,}wait %d\n", res);
	        assert(0);
	    }

	    int ret = 0;
	    if ((ret = REAL(pthread_mutex_unlock)(&lock->posix_lock)) != 0) {
	        fprintf(stderr, "Error on mutex_unlock %d\n", ret == EPERM);
	        assert(0);
	    }

   	    DEC_LOCK(NAME)(lock);
	    return res;
	#else
		null_abort(); 
		return 0;
	#endif
}

int  lock_cond_wait(lock_cond_t *cond, lock_mutex_t *lock, lock_context_t *me){
    return lock_cond_timedwait(cond, lock, me, 0);
}

int  lock_cond_signal(lock_cond_t *cond){
	#if COND_VAR
    	return REAL(pthread_cond_signal)(cond);
	#else
		null_abort(); 
		return 0;
	#endif
}

int  lock_cond_broadcast(lock_cond_t *cond){
	#if COND_VAR
    	DEBUG("[%d] Broadcast cond=%p\n", cur_thread_id, cond);
    	return REAL(pthread_cond_broadcast)(cond);
	#else
		null_abort(); 
		return 0;
	#endif
}
int  lock_cond_destroy(lock_cond_t *cond){
	#if COND_VAR
	    return REAL(pthread_cond_destroy)(cond);
	#else
		null_abort(); 
		return 0;
	#endif
}

void lock_thread_start(void){}
void lock_thread_exit(void){}
void lock_application_init(void){}
void lock_application_exit(void){}
