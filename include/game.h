#pragma once

#include <stdint.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <tchar.h>
#define OS_NAME "Windows"
#elif defined(__linux__)
#define OS_NAME "Linux"
#else
#define OS_NAME "Mac"
#endif

#define INITIAL_WIDTH 1280.0f
#define INITIAL_HEIGHT 720.0f

void log_sys_info(void);
const int game_run(void);

