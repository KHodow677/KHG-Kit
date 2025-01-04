#pragma once

#include <time.h>

#if defined(_WIN32) || defined(_WIN64)
#include <process.h>
#include <sys/timeb.h>
#define _CONDITION_EVENT_ONE 0
#define _CONDITION_EVENT_ALL 1
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define __UNDEF_LEAN_AND_MEAN
#endif
#include <windows.h>
#ifdef __UNDEF_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#undef __UNDEF_LEAN_AND_MEAN
#endif
#define TSS_DTOR_ITERATIONS (4)
#else
#include <sched.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#undef _FEATURES_H
#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif
#if !defined(_POSIX_C_SOURCE) || ((_POSIX_C_SOURCE - 0) < 199309L)
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif
#if !defined(_XOPEN_SOURCE) || ((_XOPEN_SOURCE - 0) < 500)
#undef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif
#define _XPG6
#define TSS_DTOR_ITERATIONS PTHREAD_DESTRUCTOR_ITERATIONS
#endif

typedef enum thd_thread_result {
  THD_THREAD_ERROR,
  THD_THREAD_SUCCESS,
  THD_THREAD_TIMEOUT,
  THD_THREAD_BUSY,
  THD_THREAD_NOMEM
} thd_thread_result;

typedef enum thd_mutex_type {
  THD_MUTEX_PLAIN,
  THD_MUTEX_TIMED,
  THD_MUTEX_RECURSIVE
} thd_mutex_type;

#if defined(_WIN32) || defined(_WIN64)
typedef struct thd_mutex {
  union {
    CRITICAL_SECTION cs;
    HANDLE mut;
  } mHandle;
  int mAlreadyLocked;
  int mRecursive;
  int mTimed;
} thd_mutex;
#else
  typedef pthread_mutex_t thd_mutex;
#endif

int thd_mutex_init(thd_mutex *mutex, int type);
int thd_mutex_lock(thd_mutex *mutex);
int thd_mutex_timed_lock(thd_mutex *mutex, const struct timespec *ts);
int thd_mutex_unlock(thd_mutex *mutex);
int thd_mutex_trylock(thd_mutex *mutex);
void thd_mutex_destroy(thd_mutex *mutex);

#if defined(_WIN32) || defined(_WIN64)
typedef struct thd_thread_condition {
  HANDLE mEvents[2];
  unsigned int mWaitersCount;
  CRITICAL_SECTION mWaitersCountLock;
} thd_thread_condition;
#else
  typedef pthread_cond_t thd_thread_condition;
#endif

int thd_condition_init(thd_thread_condition *cond);
int thd_condition_signal(thd_thread_condition *cond);
int thd_condition_broadcast(thd_thread_condition *cond);
int thd_condition_wait(thd_thread_condition *cond, thd_mutex *mutex);
int thd_condition_timedwait(thd_thread_condition *cond, thd_mutex *mutex, const struct timespec *ts);
void thd_condition_destroy(thd_thread_condition *cond);

#if defined(_WIN32) || defined(_WIN64)
typedef HANDLE thd_thread;
#else
typedef pthread_t thd_thread;
#endif

typedef int (*thd_thread_start)(void *arg);

int thd_thread_create(thd_thread *thr, thd_thread_start func, void *arg);
int thd_thread_detach(thd_thread thr);
int thd_thread_equal(thd_thread thr0, thd_thread thr1);
int thd_thread_join(thd_thread thr, int *res);
int thd_thread_sleep(const struct timespec *duration, struct timespec *remaining);
unsigned int thd_thread_current(void);
unsigned int thd_thread_hardware_concurrency(void);

void thd_thread_exit(int res);
void thd_thread_yield(void);

#if defined(_WIN32) || defined(_WIN64)
typedef DWORD thd_thread_specific;
#else
typedef pthread_key_t thd_thread_specific;
#endif

typedef void (*thd_thread_specific_destructor)(void *val);

int thd_thread_specific_create(thd_thread_specific *key, thd_thread_specific_destructor dtor);
int thd_thread_specific_set(thd_thread_specific key, void *val);
void thd_thread_specific_delete(thd_thread_specific key);
void *thd_thread_specific_get(thd_thread_specific key);

#if defined(_WIN32) || defined(_WIN64)
typedef struct {
  LONG volatile status;
  CRITICAL_SECTION lock;
} thd_once_flag;
#define THD_ONCE_FLAG_INIT {0,}
#else
#define THD_ONCE_FLAG_INIT PTHREAD_ONCE_INIT
#endif

#if defined(_WIN32) || defined(_WIN64)
void thd_call_once(thd_once_flag *flag, void (*func)(void));
#else
#define thd_call_once(flag,func) pthread_once(flag,func)
#endif

