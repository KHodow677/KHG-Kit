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

typedef enum {
  THREAD_ERROR = 0,
  THREAD_SUCCESS,
  THREAD_TIMEOUT,
  THREAD_BUSY,
  THREAD_NOMEM
} ThreadResult;

typedef enum {
  MUTEX_PLAIN = 0,
  MUTEX_TIMED,
  MUTEX_RECURSIVE
} MutexType;

#if defined(_WIN32) || defined(_WIN64)
typedef struct {
  union {
    CRITICAL_SECTION cs;
    HANDLE mut;
  } mHandle;
  int mAlreadyLocked;
  int mRecursive;
  int mTimed;
} Mutex;
#else
  typedef pthread_mutex_t Mutex;
#endif

int mutex_init(Mutex *mutex, int type);
int mutex_lock(Mutex *mutex);
int MUTEX_TIMEDlock(Mutex *mutex, const struct timespec *ts);
int mutex_unlock(Mutex *mutex);
int mutex_trylock(Mutex *mutex);
void mutex_destroy(Mutex *mutex);

#if defined(_WIN32) || defined(_WIN64)
typedef struct {
  HANDLE mEvents[2];
  unsigned int mWaitersCount;
  CRITICAL_SECTION mWaitersCountLock;
} ThreadCondition;
#else
  typedef pthread_cond_t ThreadCondition;
#endif

int condition_init(ThreadCondition *cond);
int condition_signal(ThreadCondition *cond);
int condition_broadcast(ThreadCondition *cond);
int condition_wait(ThreadCondition *cond, Mutex *mutex);
int condition_timedwait(ThreadCondition *cond, Mutex *mutex, const struct timespec *ts);
void condition_destroy(ThreadCondition *cond);

#if defined(_WIN32) || defined(_WIN64)
typedef HANDLE Thread;
#else
typedef pthread_t Thread;
#endif

typedef int (*ThreadStart)(void *arg);

int thread_create(Thread *thr, ThreadStart func, void *arg);
int thread_detach(Thread thr);
int thread_equal(Thread thr0, Thread thr1);
int thread_join(Thread thr, int *res);
int thread_sleep(const struct timespec *duration, struct timespec *remaining);
unsigned long thread_current(void);
unsigned long thread_hardware_concurrency(void);

void thread_exit(int res);
void thread_yield(void);

#if defined(_WIN32) || defined(_WIN64)
typedef DWORD ThreadSpecific;
#else
typedef pthread_key_t ThreadSpecific;
#endif

typedef void (*ThreadSpecificDestructor)(void *val);

int thread_specific_create(ThreadSpecific *key, ThreadSpecificDestructor dtor);
int thread_specific_set(ThreadSpecific key, void *val);
void thread_specific_delete(ThreadSpecific key);
void *thread_specific_get(ThreadSpecific key);

#if defined(_WIN32) || defined(_WIN64)
typedef struct {
  LONG volatile status;
  CRITICAL_SECTION lock;
} OnceFlag;
#define ONCE_FLAG_INIT {0,}
#else
#define ONCE_FLAG_INIT PTHREAD_ONCE_INIT
#endif

#if defined(_WIN32) || defined(_WIN64)
void call_once(OnceFlag *flag, void (*func)(void));
#else
#define call_once(flag,func) pthread_once(flag,func)
#endif

