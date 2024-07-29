#pragma once

#include "gfx/gfx.h"

void glfw_key_callback(GLFWwindow *window, int32_t key, int scancode, int action, int mods);
void glfw_mouse_button_callback(GLFWwindow *window, int32_t button, int action, int mods); 
void glfw_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void glfw_cursor_callback(GLFWwindow *window, double xpos, double ypos);
void glfw_char_callback(GLFWwindow *window, uint32_t charcode);

void update_input(void);
void clear_events(void);

uint64_t djb2_hash(uint64_t hash, const void *buf, size_t size);

void props_stack_create(PropsStack *stack); 
void props_stack_resize(PropsStack *stack, uint32_t newcap); 
void props_stack_push(PropsStack *stack, LfUIElementProps props); 
LfUIElementProps props_stack_pop(PropsStack *stack); 
LfUIElementProps props_stack_peak(PropsStack *stack); 
bool props_stack_empty(PropsStack *stack);

LfUIElementProps get_props_for(LfUIElementProps props);
