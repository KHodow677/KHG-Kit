#pragma once

#include "khg_phy/base.h"

#define B2_NULL_INDEX -1

#if defined(_WIN32) || defined(_WIN64)
	#define B2_PLATFORM_WINDOWS
#elif defined(__ANDROID__)
	#define B2_PLATFORM_ANDROID
#elif defined(__linux__)
	#define B2_PLATFORM_LINUX
#elif defined(__APPLE__)
	#include <TargetConditionals.h>
	#if defined(TARGET_OS_IPHONE) && !TARGET_OS_IPHONE
		#define B2_PLATFORM_MACOS
	#else
		#define B2_PLATFORM_IOS
	#endif
#elif defined(__EMSCRIPTEN__)
	#define B2_PLATFORM_WASM
#else
	#error Unsupported platform
#endif

#if defined(__x86_64__) || defined(_M_X64)
	#define B2_CPU_X64
#elif defined(__aarch64__) || defined(_M_ARM64)
	#define B2_CPU_ARM
#elif defined(__EMSCRIPTEN__)
	#define B2_CPU_WASM
#else
	#error Unsupported CPU
#endif

#if defined(__clang__)
	#define B2_COMPILER_CLANG
#elif defined(__GNUC__)
	#define B2_COMPILER_GCC
#elif defined(_MSC_VER)
	#define B2_COMPILER_MSVC
#endif

extern float b2_lengthUnitsPerMeter;

#define b2_huge (100000.0f * b2_lengthUnitsPerMeter)
#define b2_maxWorkers 64
#define b2_graphColorCount 12
#define b2_linearSlop (0.005f * b2_lengthUnitsPerMeter)
#define b2_maxWorlds 128
#define b2_maxTranslation (4.0f * b2_lengthUnitsPerMeter)
#define b2_maxRotation (0.25f * b2_pi)
#define b2_speculativeDistance (4.0f * b2_linearSlop)
#define b2_aabbMargin (0.1f * b2_lengthUnitsPerMeter)
#define b2_timeToSleep 0.5f
#define B2_ARRAY_COUNT(A) (int)(sizeof(A) / sizeof(A[0]))
#define B2_MAYBE_UNUSED(x) ((void)(x))
#define B2_SECRET_COOKIE 1152023
#define b2CheckDef(DEF) B2_ASSERT(DEF->internalValue == B2_SECRET_COOKIE)
