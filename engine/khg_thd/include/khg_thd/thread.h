#pragma once

struct thd_args {
  void *(*func)(void *data);
  void *data;
};

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <pthread.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
#define THD_THREAD_DWCREATIONFLAGS 1
#define THD_RWLOCK 1
#else
#define THD_THREAD_STACKADDR 1
#define THD_THREAD_DETACHSTATE 1
#define THD_THREAD_GUARDSIZE 1
#define THD_THREAD_INHERITSCHED 1
#define THD_THREAD_SCHEDPOLICY 1
#define THD_THREAD_SCOPE 1
#if _POSIX_C_SOURCE >= 200112L
#define THD_THREAD_STACK 1
#endif
#define THD_MUTEX_ATTR 1
#define THD_MUTEX_PSHARED 1
#if _POSIX_C_SOURCE >= 200809L
#define THD_MUTEX_TYPE 1
#endif
#if _POSIX_C_SOURCE >= 200112L
#if defined (__linux__)
#define THD_MUTEX_ROBUST 1
#endif
#endif
#define THD_MUTEX_PROTOCOL 1
#define THD_MUTEX_PRIOCEILING 1
#define THD_COND_ATTR 1
#define THD_COND_PSHARED 1
#if _POSIX_C_SOURCE >= 200112L
#define THD_COND_CLOCK 1
#endif
#if _POSIX_C_SOURCE >= 200112L
#define THD_RWLOCK 1
#endif
#endif

struct thd_thread {
#if defined(_WIN32) || defined(_WIN64)
  HANDLE wThread;
#else
  pthread_t pThread;
#endif
};

struct thd_thread_attr {
  size_t stacksize;
#if defined(_WIN32) || defined(_WIN64)
  int dwCreationFlags;
#else
  void *stackaddr;
  int detachstate;
  size_t guardsize;
  int inheritsched;
  int schedpolicy;
  int scope;
#ifdef THD_THREAD_STACK
  size_t stack;
#endif
#endif
};

struct thd_mutex {
#if defined(_WIN32) || defined(_WIN64)
  CRITICAL_SECTION wMutex;
#else
  pthread_mutex_t pMutex;
#endif
};

#ifdef THD_MUTEX_ATTR
struct thd_mutex_attr {
#if !defined(_WIN32) && !defined(_WIN64)
  int pshared;
#ifdef THD_MUTEX_TYPE
  int type;
#endif
#ifdef THD_MUTEX_ROBUST
  int robust;
#endif
#ifdef THD_MUTEX_PROTOCOL
  int protocol;
#endif
#ifdef THD_MUTEX_PRIOCEILING
  int prioceiling;
#endif
#endif
  };
#endif

struct thd_cond {
#if defined(_WIN32) || defined(_WIN64)
  CONDITION_VARIABLE wCond;
#else
  pthread_cond_t pCond;
#endif
};

#ifdef THD_COND_ATTR
struct thd_cond_attr {
#if !defined(_WIN32) && !defined(_WIN64)
  int pshared;
#ifdef THD_COND_CLOCK
  int clock;
#endif
#endif
  };
#endif

#ifdef THD_RWLOCK
struct thd_rwlock {
#if defined(_WIN32) || defined(_WIN64)
  int type;
  PSRWLOCK wRWLock;
#else
  pthread_rwlock_t pRWLock;
#endif
};
#endif

int thd_thread_create(struct thd_thread *thread, struct thd_thread_attr *attr, void *(*func)(void *data), void *data, struct thd_args *args);
int thd_thread_detach(struct thd_thread thread);
int thd_thread_join(struct thd_thread thread, void *code);

int thd_thread_equal(struct thd_thread thread1, struct thd_thread thread2);
struct thd_thread thd_thread_self(void);
unsigned long thd_thread_id(struct thd_thread thread);

void thd_thread_exit(void *code);
int thd_thread_cancel(struct thd_thread thread);

#ifdef THD_MUTEX_ATTR
int thd_mutex_init(struct thd_mutex *mutex, struct thd_mutex_attr *attr);
#else
int thd_mutex_init(struct thd_mutex *mutex, void *attr);
#endif

int thd_mutex_lock(struct thd_mutex *mutex);
int thd_mutex_trylock(struct thd_mutex *mutex);
int thd_mutex_unlock(struct thd_mutex *mutex);

int thd_mutex_destroy(struct thd_mutex *mutex);

#ifdef THD_COND_ATTR
int thd_cond_init(struct thd_cond *cond, struct thd_cond_attr *attr);
#else
int thd_cond_init(struct thd_cond *cond, void *attr);
#endif
int thd_cond_signal(struct thd_cond *cond);
int thd_cond_broadcast(struct thd_cond *cond);
int thd_cond_destroy(struct thd_cond *cond);
int thd_cond_wait(struct thd_cond *cond, struct thd_mutex *mutex);

#ifdef THD_RWLOCK
int thd_rwlock_init(struct thd_rwlock *rwlock);
int thd_rwlock_rdlock(struct thd_rwlock *rwlock);
int thd_rwlock_unlock(struct thd_rwlock *rwlock);
int thd_rwlock_wrlock(struct thd_rwlock *rwlock);
int thd_rwlock_destroy(struct thd_rwlock *rwlock);
#endif

