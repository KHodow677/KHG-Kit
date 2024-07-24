#include "khg_phy/allocate.h"
#include "khg_phy/core.h"
#include "khg_phy/base.h"

#if defined(B2_COMPILER_MSVC)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>
#else
#include <stdlib.h>
#endif

#include <stdatomic.h>
#include <stdint.h>

static b2AllocFcn *b2_allocFcn = NULL;
static b2FreeFcn *b2_freeFcn = NULL;
static _Atomic int b2_byteCount;

void b2SetAllocator(b2AllocFcn *allocFcn, b2FreeFcn *freeFcn) {
	b2_allocFcn = allocFcn;
	b2_freeFcn = freeFcn;
}

#define B2_ALIGNMENT 32

void *b2Alloc(uint32_t size) {
	atomic_fetch_add_explicit(&b2_byteCount, size, memory_order_relaxed);
	uint32_t size32 = ((size - 1) | 0x1F) + 1;
	if (b2_allocFcn != NULL) {
		void *ptr = b2_allocFcn(size32, B2_ALIGNMENT);
		return ptr;
	}
#ifdef B2_PLATFORM_WINDOWS
	void *ptr = _aligned_malloc(size32, B2_ALIGNMENT);
#else
	void *ptr = aligned_alloc(B2_ALIGNMENT, size32);
#endif
	return ptr;
}

void b2Free(void *mem, uint32_t size) {
	if (mem == NULL) {
		return;
	}
	if (b2_freeFcn != NULL) {
		b2_freeFcn(mem);
	}
	else {
#ifdef B2_PLATFORM_WINDOWS
		_aligned_free(mem);
#else
		free(mem);
#endif
	}
	atomic_fetch_sub_explicit(&b2_byteCount, size, memory_order_relaxed);
}

int b2GetByteCount(void) {
	return atomic_load_explicit(&b2_byteCount, memory_order_relaxed);
}
