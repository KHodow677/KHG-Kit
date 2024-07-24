#pragma once

#include <stdint.h>

#define BOX2D_EXPORT
#define B2_API extern
#define B2_INLINE static inline
#define B2_LITERAL(T) (T)
#define B2_ZERO_INIT {0}

typedef void *b2AllocFcn(unsigned int size, int alignment);
typedef void b2FreeFcn(void*mem);
typedef int b2AssertFcn(const char*condition, const char *fileName, int lineNumber);
B2_API void b2SetAllocator(b2AllocFcn *allocFcn, b2FreeFcn *freeFcn);
B2_API int b2GetByteCount(void);
B2_API void b2SetAssertFcn(b2AssertFcn *assertFcn);

typedef struct b2Version {
	int major;
	int minor;
	int revision;
} b2Version;

B2_API b2Version b2GetVersion(void);
typedef struct b2Timer {
#if defined(_WIN32)
	int64_t start;
#elif defined(__linux__) || defined(__APPLE__)
	unsigned long long start_sec;
	unsigned long long start_usec;
#endif
} b2Timer;

B2_API b2Timer b2CreateTimer(void);
B2_API int64_t b2GetTicks(b2Timer *timer);
B2_API float b2GetMilliseconds(const b2Timer *timer);
B2_API float b2GetMillisecondsAndReset(b2Timer *timer);
B2_API void b2SleepMilliseconds(int milliseconds);
B2_API void b2Yield();
