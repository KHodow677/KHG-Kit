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

#define INITIAL_WIDTH 1920.0f
#define INITIAL_HEIGHT 1080.0f

extern float SCREEN_WIDTH;
extern float SCREEN_HEIGHT;
extern float SCREEN_SCALE;

void log_sys_info(void);
const int game_run(void);

