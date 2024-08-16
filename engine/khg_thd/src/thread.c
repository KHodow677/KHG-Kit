#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "khg_thd/thread.h"

#ifdef _WIN32
#include <windows.h>
DWORD WINAPI __thd_winthreads_function_wrapper(void *data) {
  struct thd_args *args = data;
  args->func(args->data);

  return TRUE;
}
#else
#include <pthread.h>
#endif

int thd_thread_create(struct thd_thread *thread, struct thd_thread_attr *attr, void *(*func)(void *data), void *data, struct thd_args *args) {
#if defined(_WIN32) || defined(_WIN64)
  args->func = func;
  args->data = data;
  if (attr) { 
    thread->wThread = CreateThread(NULL, attr->stacksize ? attr->stacksize : 0, __thd_winthreads_function_wrapper, args, attr->dwCreationFlags ? (DWORD)attr->dwCreationFlags : 0, NULL);
    }
  else {
    thread->wThread = CreateThread(NULL, 0, __thd_winthreads_function_wrapper, args, 0, NULL);
  }
  return thread->wThread == NULL;
#else
  pthread_attr_t pAttr;
  (void) args;
  if (attr) {
    if (pthread_attr_init(&pAttr)) {
      return 1;
    }
    if (attr->detachstate) {
      pthread_attr_setdetachstate(&pAttr, attr->detachstate);
    }
    if (attr->guardsize) {
      pthread_attr_setguardsize(&pAttr, attr->guardsize);
    }
#ifdef THD_THREAD_INHERITSCHED
    if (attr->inheritsched) {
      pthread_attr_setinheritsched(&pAttr, attr->inheritsched);
    }
#endif
    if (attr->schedpolicy) {
      pthread_attr_setschedpolicy(&pAttr, attr->schedpolicy);
    }
    if (attr->scope) {
      pthread_attr_setscope(&pAttr, attr->scope);
    }
#ifdef THD_THREAD_STACK
      if (attr->stack) {
        pthread_attr_setstack(&pAttr, attr->stackaddr, attr->stack);
      }
#endif
    if (attr->stacksize) {
      pthread_attr_setstacksize(&pAttr, attr->stacksize);
    }
  }
  return pthread_create(&thread->pThread, attr ? &pAttr : NULL, func, data);
#endif
}

int thd_thread_detach(struct thd_thread thread) {
#if defined(_WIN32) || defined(_WIN64)
  return CloseHandle(thread.wThread);
#else
  return pthread_detach(thread.pThread);
#endif
}

int thd_thread_join(struct thd_thread thread, void *code) {
#if defined(_WIN32) || defined(_WIN64)
  if (WaitForSingleObject(thread.wThread, INFINITE) == WAIT_FAILED) {
    return 0;
  }
  return GetExitCodeThread(thread.wThread, (LPDWORD)&code) == 0;
#else
  return pthread_join(thread.pThread, code ? &code : NULL);
#endif
}

int thd_thread_equal(struct thd_thread thread1, struct thd_thread thread2) {
#if defined(_WIN32) || defined(_WIN64)
  return thread1.wThread == thread2.wThread;
#else
  return pthread_equal(thread1.pThread, thread2.pThread);
#endif
}

struct thd_thread thd_thread_self(void) {
  struct thd_thread t;
#if defined(_WIN32) || defined(_WIN64)
  t.wThread = GetCurrentThread();
#else
  t.pThread = pthread_self();
#endif
  return t;
}

unsigned long thd_thread_id(struct thd_thread thread) {
#if defined(_WIN32) || defined(_WIN64)
  return GetThreadId(thread.wThread);
#else
  return (unsigned long)thread.pThread;
#endif
}

void thd_thread_exit(void *code) {
#if defined(_WIN32) || defined(_WIN64)
#if defined  __WATCOMC__ || _MSC_VER || __DMC__
  ExitThread((DWORD)code);
#else
  ExitThread((DWORD)(uintptr_t)code);
#endif
#else
  pthread_exit(code);
#endif
}

int thd_thread_cancel(struct thd_thread thread) {
#if defined(_WIN32) || defined(_WIN64)
  return TerminateThread(thread.wThread, 0);
#else
  return pthread_cancel(thread.pThread);
#endif
}

#ifdef THD_MUTEX_ATTR
int thd_mutex_init(struct thd_mutex *mutex, struct thd_mutex_attr *attr) {
#else
int thd_mutex_init(struct thd_mutex *mutex, void *attr) {
#endif
#if defined(_WIN32) || defined(_WIN64)
  (void) attr;
  InitializeCriticalSection(&mutex->wMutex);
  return 0;
#else
  pthread_mutexattr_t pAttr;
  if (attr) {
    if (pthread_mutexattr_init(&pAttr)) {
      return 1;
    }
    if (attr->pshared) {
      pthread_mutexattr_setpshared(&pAttr, attr->pshared);
    }
#ifdef THD_MUTEX_TYPE
    if (attr->type) {
      pthread_mutexattr_settype(&pAttr, attr->type);
    }
#endif
#ifdef THD_MUTEX_ROBUST
    if (attr->robust) {
      pthread_mutexattr_setrobust(&pAttr, attr->robust);
    }
#endif
#ifdef THD_MUTEX_PROTOCOL
    if (attr->protocol) {
      pthread_mutexattr_setprotocol(&pAttr, attr->protocol);
    }
#endif
#ifdef THD_MUTEX_PRIOCEILING
    if (attr->prioceiling) {
      pthread_mutexattr_setprioceiling(&pAttr, attr->prioceiling);
    }
#endif
  }
  return pthread_mutex_init(&mutex->pMutex, attr ? &pAttr : NULL);
#endif
}

int thd_mutex_lock(struct thd_mutex *mutex) {
#if defined(_WIN32) || defined(_WIN64)
  EnterCriticalSection(&mutex->wMutex);
  return 0;
#else
  return pthread_mutex_lock(&mutex->pMutex);
#endif
}

int thd_mutex_trylock(struct thd_mutex *mutex) {
#if defined(_WIN32) || defined(_WIN64)
  TryEnterCriticalSection(&mutex->wMutex);
  return 0;
#else
  return pthread_mutex_trylock(&mutex->pMutex);
#endif
}

int thd_mutex_unlock(struct thd_mutex *mutex) {
#if defined(_WIN32) || defined(_WIN64)
  LeaveCriticalSection(&mutex->wMutex);
  return 0;
#else
  return pthread_mutex_unlock(&mutex->pMutex);
#endif
}

int thd_mutex_destroy(struct thd_mutex *mutex) {
#if defined(_WIN32) || defined(_WIN64)
  DeleteCriticalSection(&mutex->wMutex);
  return 0;
#else
  return pthread_mutex_destroy(&mutex->pMutex);
#endif
}

#ifdef THD_COND_ATTR
int thd_cond_init(struct thd_cond *cond, struct thd_cond_attr *attr) {
#else
int thd_cond_init(struct thd_cond *cond, void *attr) {
#endif
#if defined(_WIN32) || defined(_WIN64)
  (void) attr;
  InitializeConditionVariable(&cond->wCond);
  return 0;
#else
  pthread_condattr_t pAttr;
  if (attr) {
    if (pthread_condattr_init(&pAttr) != 0) {
      return 1;
    }
    if (attr->pshared) {
      pthread_condattr_setpshared(&pAttr, attr->pshared);
    }
#ifdef THD_COND_CLOCK
    if (attr->clock) {
      pthread_condattr_setclock(&pAttr, attr->clock);
    }
#endif
  }
  return pthread_cond_init(&cond->pCond, attr ? &pAttr : NULL);
#endif
}

int thd_cond_signal(struct thd_cond *cond) {
#if defined(_WIN32) || defined(_WIN64)
  WakeConditionVariable(&cond->wCond);
  return 0;
#else
  return pthread_cond_signal(&cond->pCond);
#endif
}

int thd_cond_broadcast(struct thd_cond *cond) {
#if defined(_WIN32) || defined(_WIN64)
  WakeAllConditionVariable(&cond->wCond);
  return 0;
#else
  return pthread_cond_broadcast(&cond->pCond);
#endif
}

int thd_cond_destroy(struct thd_cond *cond) {
#if defined(_WIN32) || defined(_WIN64)
  return 0;
#else
  return pthread_cond_destroy(&cond->pCond);
#endif
}

int thd_cond_wait(struct thd_cond *cond, struct thd_mutex *mutex) {
#if defined(_WIN32) || defined(_WIN64)
  return SleepConditionVariableCS(&cond->wCond, &mutex->wMutex, INFINITE) == 0;
#else
  return pthread_cond_wait(&cond->pCond, &mutex->pMutex);
#endif
}

#ifdef THD_RWLOCK
int thd_rwlock_init(struct thd_rwlock *rwlock) {
#if defined(_WIN32) || defined(_WIN64)
  rwlock->wRWLock = malloc(sizeof(SRWLOCK));
  if (!rwlock->wRWLock) {
    return 1;
  }
  InitializeSRWLock(rwlock->wRWLock);
  return 0;
#else
  return pthread_rwlock_init(&rwlock->pRWLock, NULL);
  #endif
}

int thd_rwlock_rdlock(struct thd_rwlock *rwlock) {
#if defined(_WIN32) || defined(_WIN64)
  AcquireSRWLockShared(rwlock->wRWLock);
  rwlock->type = 1;
  return 0;
#else
  return pthread_rwlock_rdlock(&rwlock->pRWLock);
#endif
}

int thd_rwlock_unlock(struct thd_rwlock *rwlock) {
#if defined(_WIN32) || defined(_WIN64)
  switch (rwlock->type) {
    case 1: {
      ReleaseSRWLockShared(rwlock->wRWLock);
      break;
    }
    case 2: {
      ReleaseSRWLockExclusive(rwlock->wRWLock);
      break;
    }
  }
  rwlock->type = 0;
  return 0;
#else
  return pthread_rwlock_unlock(&rwlock->pRWLock);
#endif
}

int thd_rwlock_wrlock(struct thd_rwlock *rwlock) {
#if defined(_WIN32) || defined(_WIN64)
  AcquireSRWLockExclusive(rwlock->wRWLock);
  rwlock->type = 2;
  return 0;
#else
  return pthread_rwlock_wrlock(&rwlock->pRWLock);
#endif
}

int thd_rwlock_destroy(struct thd_rwlock *rwlock) {
#if defined(_WIN32) || defined(_WIN64)
  free(rwlock->wRWLock);
  rwlock->wRWLock = NULL;
  rwlock->type = 0;
  return 0;
#else
  return pthread_rwlock_destroy(&rwlock->pRWLock);
#endif
}
#endif
