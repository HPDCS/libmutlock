lock_mutex_t *lock_mutex_create(const pthread_mutexattr_t *attr){
    lock_mutex_t *impl = (lock_mutex_t *)alloc_cache_align(sizeof(lock_mutex_t));
	__HLOCK_PARAM_OBJ(NAME) params;
	params.initial_sws = 1;
	DEC_INIT(NAME)(impl, &params); // attiva euristica
	#if COND_VAR
    REAL(pthread_mutex_init)(&impl->posix_lock, /*&errattr */ attr);
    DEBUG("Mutex init lock=%p posix_lock=%p\n", impl, &impl->posix_lock);
	#endif

	return impl;
}

int  lock_mutex_lock(   lock_mutex_t *impl, lock_context_t *me){
 	DEC_LOCK(NAME)(impl);
 	#if COND_VAR
    DEBUG_PTHREAD("[%d] Lock posix=%p\n", cur_thread_id, &impl->posix_lock);
    //int ret = 
    REAL(pthread_mutex_lock)(&impl->posix_lock);
    //assert(ret == 0);
    #endif
	return 0;

}

int  lock_mutex_trylock(lock_mutex_t *impl, lock_context_t *me){
 	fprintf(stderr, "Error cond_var not supported."); 
 	exit(1); 
 	return 0;
 }

void lock_mutex_unlock( lock_mutex_t *impl, lock_context_t *me){
 #if COND_VAR
    DEBUG_PTHREAD("[%d] Unlock posix=%p\n", cur_thread_id, &impl->posix_lock);
    //int ret = 
    REAL(pthread_mutex_unlock)(&impl->posix_lock);
    assert(ret == 0);
#endif
	DEC_UNLO(NAME)(impl);
}

int  lock_mutex_destroy(lock_mutex_t *lock){ 						
	#if COND_VAR
	    REAL(pthread_mutex_destroy)(&lock->posix_lock);
	#endif
    //printf ("HERE WE ARE\n");
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

    lock_mutex_lock(lock, me);
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
