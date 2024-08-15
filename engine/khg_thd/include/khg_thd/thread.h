#pragma once

struct cthreads_args {
  void *(*func)(void *data);
  void *data;
};

#if defined(_WIN32) || defined(_WIN64)
  #include <windows.h>
#else
  #include <pthread.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
#define CTHREADS_THREAD_DWCREATIONFLAGS 1
#define CTHREADS_RWLOCK 1
#else
#define CTHREADS_THREAD_STACKADDR 1
#define CTHREADS_THREAD_DETACHSTATE 1
#define CTHREADS_THREAD_GUARDSIZE 1
#define CTHREADS_THREAD_INHERITSCHED 1
#define CTHREADS_THREAD_SCHEDPOLICY 1
#define CTHREADS_THREAD_SCOPE 1
#if _POSIX_C_SOURCE >= 200112L
#define CTHREADS_THREAD_STACK 1
#endif
#define CTHREADS_MUTEX_ATTR 1
#define CTHREADS_MUTEX_PSHARED 1
#if _POSIX_C_SOURCE >= 200809L
#define CTHREADS_MUTEX_TYPE 1
#endif
#if _POSIX_C_SOURCE >= 200112L
#if defined (__linux__)
#define CTHREADS_MUTEX_ROBUST 1
#endif
#endif
#define CTHREADS_MUTEX_PROTOCOL 1
#define CTHREADS_MUTEX_PRIOCEILING 1
#define CTHREADS_COND_ATTR 1
#define CTHREADS_COND_PSHARED 1
#if _POSIX_C_SOURCE >= 200112L
#define CTHREADS_COND_CLOCK 1
#endif
#if _POSIX_C_SOURCE >= 200112L
#define CTHREADS_RWLOCK 1
#endif
#endif

struct cthreads_thread {
#if defined(_WIN32) || defined(_WIN64)
  HANDLE wThread;
#else
  pthread_t pThread;
#endif
};

struct cthreads_thread_attr {
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
#ifdef CTHREADS_THREAD_STACK
  size_t stack;
#endif
#endif
};

struct cthreads_mutex {
#if defined(_WIN32) || defined(_WIN64)
  CRITICAL_SECTION wMutex;
#else
  pthread_mutex_t pMutex;
#endif
};

#ifdef CTHREADS_MUTEX_ATTR
struct cthreads_mutex_attr {
#if !defined(_WIN32) && !defined(_WIN64)
  int pshared;
#ifdef CTHREADS_MUTEX_TYPE
  int type;
#endif
#ifdef CTHREADS_MUTEX_ROBUST
  int robust;
#endif
#ifdef CTHREADS_MUTEX_PROTOCOL
  int protocol;
#endif
#ifdef CTHREADS_MUTEX_PRIOCEILING
  int prioceiling;
#endif
#endif
  };
#endif

struct cthreads_cond {
#if defined(_WIN32) || defined(_WIN64)
  CONDITION_VARIABLE wCond;
#else
  pthread_cond_t pCond;
#endif
};

#ifdef CTHREADS_COND_ATTR
struct cthreads_cond_attr {
#if !defined(_WIN32) && !defined(_WIN64)
  int pshared;
#ifdef CTHREADS_COND_CLOCK
  int clock;
#endif
#endif
  };
#endif

#ifdef CTHREADS_RWLOCK
struct cthreads_rwlock {
#if defined(_WIN32) || defined(_WIN64)
  int type;
  PSRWLOCK wRWLock;
#else
  pthread_rwlock_t pRWLock;
#endif
};
#endif

int cthreads_thread_create(struct cthreads_thread *thread, struct cthreads_thread_attr *attr, void *(*func)(void *data), void *data, struct cthreads_args *args);
int cthreads_thread_detach(struct cthreads_thread thread);
int cthreads_thread_join(struct cthreads_thread thread, void *code);

int cthreads_thread_equal(struct cthreads_thread thread1, struct cthreads_thread thread2);
struct cthreads_thread cthreads_thread_self(void);
unsigned long cthreads_thread_id(struct cthreads_thread thread);

void cthreads_thread_exit(void *code);
int cthreads_thread_cancel(struct cthreads_thread thread);

#ifdef CTHREADS_MUTEX_ATTR
int cthreads_mutex_init(struct cthreads_mutex *mutex, struct cthreads_mutex_attr *attr);
#else
int cthreads_mutex_init(struct cthreads_mutex *mutex, void *attr);
#endif

int cthreads_mutex_lock(struct cthreads_mutex *mutex);
int cthreads_mutex_trylock(struct cthreads_mutex *mutex);
int cthreads_mutex_unlock(struct cthreads_mutex *mutex);

int cthreads_mutex_destroy(struct cthreads_mutex *mutex);

#ifdef CTHREADS_COND_ATTR
int cthreads_cond_init(struct cthreads_cond *cond, struct cthreads_cond_attr *attr);
#else
int cthreads_cond_init(struct cthreads_cond *cond, void *attr);
#endif
int cthreads_cond_signal(struct cthreads_cond *cond);
int cthreads_cond_broadcast(struct cthreads_cond *cond);
int cthreads_cond_destroy(struct cthreads_cond *cond);
int cthreads_cond_wait(struct cthreads_cond *cond, struct cthreads_mutex *mutex);

#ifdef CTHREADS_RWLOCK
int cthreads_rwlock_init(struct cthreads_rwlock *rwlock);
int cthreads_rwlock_rdlock(struct cthreads_rwlock *rwlock);
int cthreads_rwlock_unlock(struct cthreads_rwlock *rwlock);
int cthreads_rwlock_wrlock(struct cthreads_rwlock *rwlock);
int cthreads_rwlock_destroy(struct cthreads_rwlock *rwlock);
#endif

