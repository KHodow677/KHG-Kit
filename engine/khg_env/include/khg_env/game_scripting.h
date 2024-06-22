#pragma once

#include "khg_math/vec2.h"
#include <stdbool.h>
#include "GLFW/glfw3.h"

extern bool current_full_screen;
extern bool full_screen;
extern int window_focus;
extern int mouse_moved_flag;
extern GLFWwindow *wind;

bool init_game();
bool game_logic(float delta_time);
void close_game();
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

