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

#include "khg_phy/phy_types.h"

extern cpSpace *SPACE;

int game_run();

