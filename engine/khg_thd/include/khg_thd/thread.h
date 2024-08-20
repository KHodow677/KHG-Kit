#pragma once

#if defined(_WIN32) || defined(_WIN64) 
#include <windows.h>
#else
#include <pthread.h>
#endif

typedef void (*thd_thread_method)(void *);

#if defined(_WIN32) || defined(_WIN64) 
typedef HANDLE thd_thread;
#else
typedef pthread_t thd_thread;
#endif

int thd_thread_detach(thd_thread* thread, thd_thread_method method, void* data);
int thd_thread_join(thd_thread* thread);

#if defined(_WIN32) || defined(_WIN64) 
typedef CRITICAL_SECTION thd_mutex;
#else
typedef pthread_mutex_t thd_mutex;
#endif

int thd_mutex_init(thd_mutex* mutex);
int thd_mutex_lock(thd_mutex* mutex);
int thd_mutex_trylock(thd_mutex* mutex);
int thd_mutex_unlock(thd_mutex* mutex);
int thd_mutex_destroy(thd_mutex* mutex);

#if defined(_WIN32) || defined(_WIN64) 
typedef CONDITION_VARIABLE thd_condition;
#else
typedef pthread_cond_t thd_condition;
#endif

int thd_condition_init(thd_condition* cond);
int thd_condition_signal(thd_condition* cond);
int thd_condition_wait(thd_condition* cond, thd_mutex* mutex);
int thd_condition_destroy(thd_condition* cond);
