#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <tchar.h>
#define OS_NAME "Windows"
#elif defined(__linux__)
#define OS_NAME "Linux"
#else
#define OS_NAME "Mac"
#endif

void log_sys_info(void);
int game_run(void);

