#include "khg_thd/concurrent.h"
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

int thd_mutex_init(thd_mutex *mtx, int type) {
#if defined(_WIN32) || defined(_WIN64)
  mtx->mAlreadyLocked = false;
  mtx->mRecursive = type & MUTEX_RECURSIVE;
  mtx->mTimed = type & MUTEX_TIMED;
  if (!mtx->mTimed) {
    InitializeCriticalSection(&(mtx->mHandle.cs));
  }
  else {
    mtx->mHandle.mut = CreateMutex(NULL, false, NULL);
    if (mtx->mHandle.mut == NULL) {
      return THD_THREAD_ERROR;
    }
  }
  return THD_THREAD_SUCCESS;
#else
  int ret;
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  if (type & THD_MUTEX_RECURSIVE) {
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  }
  ret = pthread_mutex_init(mtx, &attr);
  pthread_mutexattr_destroy(&attr);
  return ret == 0 ? THD_THREAD_SUCCESS : THD_THREAD_ERROR;
#endif
}

void thd_mutex_destroy(thd_mutex *mtx) {
#if defined(_WIN32) || defined(_WIN64)
  if (!mtx->mTimed) {
    DeleteCriticalSection(&(mtx->mHandle.cs));
  }
  else {
    CloseHandle(mtx->mHandle.mut);
  }
#else
  pthread_mutex_destroy(mtx);
#endif
}

int thd_mutex_lock(thd_mutex *mtx) {
#if defined(_WIN32) || defined(_WIN64)
  if (!mtx->mTimed) {
    EnterCriticalSection(&(mtx->mHandle.cs));
  }
  else {
    switch (WaitForSingleObject(mtx->mHandle.mut, INFINITE)) {
      case WAIT_OBJECT_0:
        break;
      case WAIT_ABANDONED:
      default:
        return THD_THREAD_ERROR;
    }
  }

  if (!mtx->mRecursive) {
    while(mtx->mAlreadyLocked) {
      Sleep(1);
    }
    mtx->mAlreadyLocked = true;
  }
  return THD_THREAD_SUCCESS;
#else
  return pthread_mutex_lock(mtx) == 0 ? THD_THREAD_SUCCESS : THD_THREAD_ERROR;
#endif
}

int thd_mutex_timed_lock(thd_mutex *mtx, const struct timespec *ts) {
#if defined(_WIN32) || defined(_WIN64)
  struct timespec current_ts;
  DWORD timeoutMs;
  if (!mtx->mTimed) {
    return THD_THREAD_ERROR;
  }
  timespec_get(&current_ts, TIME_UTC);
  if ((current_ts.tv_sec > ts->tv_sec) || ((current_ts.tv_sec == ts->tv_sec) && (current_ts.tv_nsec >= ts->tv_nsec))) {
    timeoutMs = 0;
  }
  else {
    timeoutMs  = (DWORD)(ts->tv_sec  - current_ts.tv_sec)  * 1000;
    timeoutMs += (ts->tv_nsec - current_ts.tv_nsec) / 1000000;
    timeoutMs += 1;
  }
  switch (WaitForSingleObject(mtx->mHandle.mut, timeoutMs)) {
    case WAIT_OBJECT_0:
      break;
    case WAIT_TIMEOUT:
      return THD_THREAD_TIMEOUT;
    case WAIT_ABANDONED:
    default:
      return THD_THREAD_ERROR;
  }
  if (!mtx->mRecursive) {
    while(mtx->mAlreadyLocked) {
      Sleep(1);
    }
    mtx->mAlreadyLocked = true;
  }
  return THD_THREAD_SUCCESS;
#else
  switch (pthread_mutex_timedlock(mtx, ts)) {
    case 0:
      return THD_THREAD_SUCCESS;
    case ETIMEDOUT:
      return THD_THREAD_TIMEOUT;
    default:
      return THD_THREAD_ERROR;
  }
#endif
}

int thd_mutex_trylock(thd_mutex *mtx) {
#if defined(_WIN32) || defined(_WIN64)
  int ret;
  if (!mtx->mTimed) {
    ret = TryEnterCriticalSection(&(mtx->mHandle.cs)) ? THD_THREAD_SUCCESS : THD_THREAD_BUSY;
  }
  else {
    ret = (WaitForSingleObject(mtx->mHandle.mut, 0) == WAIT_OBJECT_0) ? THD_THREAD_SUCCESS : THD_THREAD_BUSY;
  }
  if ((!mtx->mRecursive) && (ret == THD_THREAD_SUCCESS)){
    if (mtx->mAlreadyLocked) {
      LeaveCriticalSection(&(mtx->mHandle.cs));
      ret = THD_THREAD_BUSY;
    }
    else {
      mtx->mAlreadyLocked = true;
    }
  }
  return ret;
#else
  return (pthread_mutex_trylock(mtx) == 0) ? THD_THREAD_SUCCESS : THD_THREAD_BUSY;
#endif
}

int thd_mutex_unlock(thd_mutex *mtx) {
#if defined(_WIN32) || defined(_WIN64)
  mtx->mAlreadyLocked = false;
  if (!mtx->mTimed) {
    LeaveCriticalSection(&(mtx->mHandle.cs));
  }
  else {
    if (!ReleaseMutex(mtx->mHandle.mut)) {
      return THD_THREAD_ERROR;
    }
  }
  return THD_THREAD_SUCCESS;
#else
  return pthread_mutex_unlock(mtx) == 0 ? THD_THREAD_SUCCESS : THD_THREAD_ERROR;;
#endif
}

int thd_condition_init(thd_thread_condition *cond) {
#if defined(_WIN32) || defined(_WIN64)
  cond->mWaitersCount = 0;
  InitializeCriticalSection(&cond->mWaitersCountLock);
  cond->mEvents[_CONDITION_EVENT_ONE] = CreateEvent(NULL, false, false, NULL);
  if (cond->mEvents[_CONDITION_EVENT_ONE] == NULL) {
    cond->mEvents[_CONDITION_EVENT_ALL] = NULL;
    return THD_THREAD_ERROR;
  }
  cond->mEvents[_CONDITION_EVENT_ALL] = CreateEvent(NULL, true, false, NULL);
  if (cond->mEvents[_CONDITION_EVENT_ALL] == NULL) {
    CloseHandle(cond->mEvents[_CONDITION_EVENT_ONE]);
    cond->mEvents[_CONDITION_EVENT_ONE] = NULL;
    return THD_THREAD_ERROR;
  }
  return THD_THREAD_SUCCESS;
#else
  return pthread_cond_init(cond, NULL) == 0 ? THD_THREAD_SUCCESS : THD_THREAD_ERROR;
#endif
}

void thd_condition_destroy(thd_thread_condition *cond) {
#if defined(_WIN32) || defined(_WIN64)
  if (cond->mEvents[_CONDITION_EVENT_ONE] != NULL) {
    CloseHandle(cond->mEvents[_CONDITION_EVENT_ONE]);
  }
  if (cond->mEvents[_CONDITION_EVENT_ALL] != NULL) {
    CloseHandle(cond->mEvents[_CONDITION_EVENT_ALL]);
  }
  DeleteCriticalSection(&cond->mWaitersCountLock);
#else
  pthread_cond_destroy(cond);
#endif
}

int thd_condition_signal(thd_thread_condition *cond) {
#if defined(_WIN32) || defined(_WIN64)
  int haveWaiters;
  EnterCriticalSection(&cond->mWaitersCountLock);
  haveWaiters = (cond->mWaitersCount > 0);
  LeaveCriticalSection(&cond->mWaitersCountLock);
  if(haveWaiters) {
    if (SetEvent(cond->mEvents[_CONDITION_EVENT_ONE]) == 0) {
      return THREAD_ERROR;
    }
  }
  return THREAD_SUCCESS;
#else
  return pthread_cond_signal(cond) == 0 ? THD_THREAD_SUCCESS : THD_THREAD_ERROR;
#endif
}

int thd_condition_broadcast(thd_thread_condition *cond) {
#if defined(_WIN32) || defined(_WIN64)
  int haveWaiters;
  EnterCriticalSection(&cond->mWaitersCountLock);
  haveWaiters = (cond->mWaitersCount > 0);
  LeaveCriticalSection(&cond->mWaitersCountLock);
  if(haveWaiters) {
    if (SetEvent(cond->mEvents[_CONDITION_EVENT_ALL]) == 0) {
      return THD_THREAD_ERROR;
    }
  }
  return THD_THREAD_SUCCESS;
#else
  return pthread_cond_broadcast(cond) == 0 ? THD_THREAD_SUCCESS : THD_THREAD_ERROR;
#endif
}

#if defined(_WIN32) || defined(_WIN64)
static int _cnd_timedwait_win32(ThreadCondition *cond, thd_mutex *mtx, DWORD timeout) {
  DWORD result;
  int lastWaiter;
  EnterCriticalSection(&cond->mWaitersCountLock);
  ++cond->mWaitersCount;
  LeaveCriticalSection(&cond->mWaitersCountLock);
  mutex_unlock(mtx);
  result = WaitForMultipleObjects(2, cond->mEvents, false, timeout);
  if (result == WAIT_TIMEOUT) {
    mutex_lock(mtx);
    return THD_THREAD_TIMEOUT;
  }
  else if (result == WAIT_FAILED) {
    mutex_lock(mtx);
    return THD_THREAD_ERROR;
  }
  EnterCriticalSection(&cond->mWaitersCountLock);
  --cond->mWaitersCount;
  lastWaiter = (result == (WAIT_OBJECT_0 + _CONDITION_EVENT_ALL)) && (cond->mWaitersCount == 0);
  LeaveCriticalSection(&cond->mWaitersCountLock);
  if (lastWaiter) {
    if (ResetEvent(cond->mEvents[_CONDITION_EVENT_ALL]) == 0) {
      mutex_lock(mtx);
      return THD_THREAD_ERROR;
    }
  }
  mutex_lock(mtx);
  return THD_THREAD_SUCCESS;
}
#endif

int thd_condition_wait(thd_thread_condition *cond, thd_mutex *mtx) {
#if defined(_WIN32) || defined(_WIN64)
  return _cnd_timedwait_win32(cond, mtx, INFINITE);
#else
  return pthread_cond_wait(cond, mtx) == 0 ? THD_THREAD_SUCCESS: THD_THREAD_ERROR;
#endif
}

int thd_condition_timedwait(thd_thread_condition *cond, thd_mutex *mtx, const struct timespec *ts) {
#if defined(_WIN32) || defined(_WIN64)
  struct timespec now;
  if (timespec_get(&now, TIME_UTC) == TIME_UTC) {
    unsigned long long nowInMilliseconds = now.tv_sec * 1000 + now.tv_nsec / 1000000;
    unsigned long long tsInMilliseconds  = ts->tv_sec * 1000 + ts->tv_nsec / 1000000;
    DWORD delta = (tsInMilliseconds > nowInMilliseconds) ? (DWORD)(tsInMilliseconds - nowInMilliseconds) : 0;

    return _cnd_timedwait_win32(cond, mtx, delta);
  }
  else {
    return THD_THREAD_ERROR;
  }
  #else
  int ret;
  ret = pthread_cond_timedwait(cond, mtx, ts);
  if (ret == ETIMEDOUT) {
    return THD_THREAD_TIMEOUT;
  }
  return ret == 0 ? THD_THREAD_SUCCESS : THD_THREAD_ERROR;
#endif
}

#if defined(_WIN32) || defined(_WIN64)

struct CThreadTSSData {
  void* value;
  ThreadSpecific key;
  struct CThreadTSSData* next;
};

static ThreadSpecificDestructor _cthread_tss_dtors[1088] = { NULL, };
static _Thread_local struct CThreadTSSData* _cthread_tss_head = NULL;
static _Thread_local struct CThreadTSSData* _cthread_tss_tail = NULL;
static void _cthread_tss_cleanup (void);

static void _cthread_tss_cleanup (void) {
  struct CThreadTSSData* data;
  int iteration;
  unsigned int again = 1;
  void* value;
  for (iteration = 0 ; iteration < TSS_DTOR_ITERATIONS && again > 0 ; iteration++) {
    again = 0;
    for (data = _cthread_tss_head ; data != NULL ; data = data->next) {
      if (data->value != NULL) {
        value = data->value;
        data->value = NULL;
        if (_cthread_tss_dtors[data->key] != NULL) {
          again = 1;
          _cthread_tss_dtors[data->key](value);
        }
      }
    }
  }
  while (_cthread_tss_head != NULL) {
    data = _cthread_tss_head->next;
    free (_cthread_tss_head);
    _cthread_tss_head = data;
  }
  _cthread_tss_head = NULL;
  _cthread_tss_tail = NULL;
}

static void NTAPI _cthread_tss_callback(PVOID h, DWORD dwReason, PVOID pv) {
  (void)h;
  (void)pv;
  if (_cthread_tss_head != NULL && (dwReason == DLL_THREAD_DETACH || dwReason == DLL_PROCESS_DETACH)) {
    _cthread_tss_cleanup();
  }
}

#if defined(_MSC_VER)
#ifdef _M_X64
#pragma const_seg(".CRT$XLB")
#else
#pragma data_seg(".CRT$XLB")
#endif
PIMAGE_TLS_CALLBACK p_thread_callback = _cthread_tss_callback;
#ifdef _M_X64
#pragma data_seg()
#else
#pragma const_seg()
#endif
#else
PIMAGE_TLS_CALLBACK p_thread_callback __attribute__((section(".CRT$XLB"))) = _cthread_tss_callback;
#endif

#endif

typedef struct {
  thd_thread_start mFunction;
  void * mArg;
} ThreadStartInfo;


#if defined(_WIN32) || defined(_WIN64)
static DWORD WINAPI _thrd_wrapper_function(LPVOID aArg) {
#else
static void * _thrd_wrapper_function(void * aArg) {
#endif
  thd_thread_start fun;
  void *arg;
  int  res;
  ThreadStartInfo *ti = (ThreadStartInfo *) aArg;
  fun = ti->mFunction;
  arg = ti->mArg;
  free((void *)ti);
  res = fun(arg);
#if defined(_WIN32) || defined(_WIN64)
  if (_cthread_tss_head != NULL){
    _cthread_tss_cleanup();
  }
  return (DWORD)res;
#else
  return (void*)(intptr_t)res;
#endif
}

int thd_thread_create(thd_thread *thr, thd_thread_start func, void *arg) {
  ThreadStartInfo* ti = (ThreadStartInfo*)malloc(sizeof(ThreadStartInfo));
  if (ti == NULL) {
    return THD_THREAD_NOMEM;
  }
  ti->mFunction = func;
  ti->mArg = arg;
#if defined(_WIN32) || defined(_WIN64)
  *thr = CreateThread(NULL, 0, _thrd_wrapper_function, (LPVOID) ti, 0, NULL);
  #else
  if(pthread_create(thr, NULL, _thrd_wrapper_function, (void *)ti) != 0) {
    *thr = 0;
  }
  #endif
  if(!*thr) {
    free(ti);
    return THD_THREAD_ERROR;
  }
  return THD_THREAD_SUCCESS;
}

unsigned int thd_thread_current(void) {
#if defined(_WIN32) || defined(_WIN64)
  return GetCurrentThreadId();
#else
  return (unsigned long)pthread_self();
#endif
}

unsigned int thd_thread_hardware_concurrency(void) {
#if defined(_WIN32) || defined(_WIN64)
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
#else
  return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

int thd_thread_detach(thd_thread thr) {
#if defined(_WIN32) || defined(_WIN64)
  return CloseHandle(thr) != 0 ? THD_THREAD_SUCCESS : THD_THREAD_ERROR;
#else
  return pthread_detach(thr) == 0 ? THD_THREAD_SUCCESS : THD_THREAD_ERROR;
#endif
}

int thd_thread_equal(thd_thread thr0, thd_thread thr1) {
#if defined(_WIN32) || defined(_WIN64)
  return GetThreadId(thr0) == GetThreadId(thr1);
#else
  return pthread_equal(thr0, thr1);
#endif
}

void thd_thread_exit(int res) {
#if defined(_WIN32) || defined(_WIN64)
  if (_cthread_tss_head != NULL) {
    _cthread_tss_cleanup();
  }
  ExitThread((DWORD)res);
#else
  pthread_exit((void *)&res);
#endif
}

int thd_thread_join(thd_thread thr, int *res) {
#if defined(_WIN32) || defined(_WIN64)
  DWORD dwRes;
  if (WaitForSingleObject(thr, INFINITE) == WAIT_FAILED) {
    return THD_THREAD_ERROR;
  }
  if (res != NULL) {
    if (GetExitCodeThread(thr, &dwRes) != 0) {
      *res = (int) dwRes;
    }
    else {
      return THD_THREAD_ERROR;
    }
  }
  CloseHandle(thr);
#else
  void *pres;
  if (pthread_join(thr, &pres) != 0) {
    return THD_THREAD_ERROR;
  }
  if (res != NULL) {
    *res = (int)(*(int *)pres);
  }
#endif
  return THD_THREAD_SUCCESS;
}

int thd_thread_sleep(const struct timespec *duration, struct timespec *remaining) {
#if !defined(_WIN32) && !defined(_WIN64)
  int res = nanosleep(duration, remaining);
  if (res == 0) {
    return 0;
  } 
  else if (errno == EINTR) {
    return -1;
  } 
  else {
    return -2;
  }
#else
  struct timespec start;
  DWORD t;
  timespec_get(&start, TIME_UTC);
  t = SleepEx((DWORD)(duration->tv_sec * 1000 + duration->tv_nsec / 1000000 + (((duration->tv_nsec % 1000000) == 0) ? 0 : 1)), true);
  if (t == 0) {
    return 0;
  } 
  else {
    if (remaining != NULL) {
      timespec_get(remaining, TIME_UTC);
      remaining->tv_sec -= start.tv_sec;
      remaining->tv_nsec -= start.tv_nsec;
      if (remaining->tv_nsec < 0) {
        remaining->tv_nsec += 1000000000;
        remaining->tv_sec -= 1;
      }
    }
    return (t == WAIT_IO_COMPLETION) ? -1 : -2;
  }
  #endif
}

void thd_thread_yield(void) {
#if defined(_WIN32) || defined(_WIN64)
  Sleep(0);
#else
  sched_yield();
#endif
}

int thd_thread_specific_create(thd_thread_specific *key, thd_thread_specific_destructor dtor) {
#if defined(_WIN32) || defined(_WIN64)
  *key = TlsAlloc();
  if (*key == TLS_OUT_OF_INDEXES) {
    return THD_THREAD_ERROR;
  }
  _cthread_tss_dtors[*key] = dtor;
#else
  if (pthread_key_create(key, dtor) != 0) {
    return THD_THREAD_ERROR;
  }
#endif
  return THD_THREAD_SUCCESS;
}

void thd_thread_specific_delete(thd_thread_specific key) {
#if defined(_WIN32) || defined(_WIN64)
  struct CThreadTSSData* data = (struct CThreadTSSData*) TlsGetValue (key);
  struct CThreadTSSData* prev = NULL;
  if (data != NULL) {
    if (data == _cthread_tss_head) {
      _cthread_tss_head = data->next;
    }
    else {
      prev = _cthread_tss_head;
      if (prev != NULL) {
        while (prev->next != data) {
          prev = prev->next;
        }
      }
    }
    if (data == _cthread_tss_tail) {
      _cthread_tss_tail = prev;
    }
    free (data);
  }
  _cthread_tss_dtors[key] = NULL;
  TlsFree(key);
#else
  pthread_key_delete(key);
#endif
}

void *thd_thread_specific_get(thd_thread_specific key) {
#if defined(_WIN32) || defined(_WIN64)
  struct CThreadTSSData* data = (struct CThreadTSSData*)TlsGetValue(key);
  if (data == NULL) {
    return NULL;
  }
  return data->value;
#else
  return pthread_getspecific(key);
#endif
}

int thd_thread_specific_set(thd_thread_specific key, void *val) {
#if defined(_WIN32) || defined(_WIN64)
  struct CThreadTSSData* data = (struct CThreadTSSData*)TlsGetValue(key);
  if (data == NULL) {
      data = (struct CThreadTSSData*)malloc(sizeof(struct CThreadTSSData));
      if (data == NULL) {
        return THD_THREAD_ERROR;
      }

    data->value = NULL;
    data->key = key;
    data->next = NULL;

    if (_cthread_tss_tail != NULL) {
      _cthread_tss_tail->next = data;
    }
    else {
      _cthread_tss_tail = data;
    }

    if (_cthread_tss_head == NULL){
        _cthread_tss_head = data;
    }

    if (!TlsSetValue(key, data)){
        free (data);
        return THD_THREAD_ERROR;
    }
  }
  data->value = val;
#else
  if (pthread_setspecific(key, val) != 0){
    return THD_THREAD_ERROR;
  }
#endif
  return THD_THREAD_SUCCESS;
}

#if defined(_WIN32) || defined(_WIN64)
void thd_call_once(thd_once_flag *flag, void (*func)(void)) {
  while (flag->status < 3) {
    switch (flag->status) {
      case 0: {
        if (InterlockedCompareExchange (&(flag->status), 1, 0) == 0) {
          InitializeCriticalSection(&(flag->lock));
          EnterCriticalSection(&(flag->lock));
          flag->status = 2;
          func();
          flag->status = 3;
          LeaveCriticalSection(&(flag->lock));
          return;
        }
        break;
      }
      case 1: 
        break;
      case 2:
        EnterCriticalSection(&(flag->lock));
        LeaveCriticalSection(&(flag->lock));
        break;
    }
  }
}
#endif

