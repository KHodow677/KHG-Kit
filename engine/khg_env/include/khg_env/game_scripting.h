#pragma once

#include "khg_math/vec2.h"
#include "GLFW/glfw3.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#if defined(_WIN32) || defined(_WIN64)
  int __stdcall QueryPerformanceCounter(unsigned long long int *lpPerformanceCount);
  int __stdcall QueryPerformanceFrequency(unsigned long long int *lpFrequency);
#elif defined(__linux__)
  #if _POSIX_C_SOURCE < 199309L
    #undef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 199309L
  #endif
  #include <sys/time.h>
#elif defined(__APPLE__)
  #include <mach/mach_time.h>
#elif defined(EMSCRIPTEN)
  #include <emscripten.h>
#endif

extern bool current_full_screen;
extern bool full_screen;
extern int window_focus;
extern int mouse_moved_flag;
extern GLFWwindow *wind;
static uint64_t frequency = 0;
static double base_time = 0.0f;
static double start_time = 0.0f;

bool init_game();
bool game_logic(float delta_time);
void close_game();
int run_game();
void set_rel_mouse_position(int x, int y);
bool is_full_screen();
bool set_full_screen(bool f);
vec2 get_window_size();
int get_window_size_x();
int get_window_size_y();
vec2 get_framebuffer_size();
int get_framebuffer_size_x();
int get_framebuffer_size_y();
vec2 get_rel_mouse_position();
void show_mouse(bool show);
bool is_focused();
bool mouse_moved();
bool write_entire_file(const char *name, void *buffer, size_t size);
bool read_entire_file(const char *name, void *buffer, size_t size);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow *window, int key, int action, int mods);
void window_focus_callback(GLFWwindow *window, int focused);
void window_size_callback(GLFWwindow *window, int x, int y);
void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
void character_callback(GLFWwindow *window, unsigned int codepoint);

