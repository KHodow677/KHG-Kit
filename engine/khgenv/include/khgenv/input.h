#pragma once

#include "khgmath/vec2.h"
#include "khgutils/bool.h"
#include "GLFW/glfw3.h"

typedef struct {
  char pressed;
  char held;
  char released;
  char newState;
  char typed;
  float typedTime;
  enum {
    key_A = 0,
    key_B, key_C, key_D, key_E, key_F, key_G, 
    key_H, key_I, key_J, key_K, key_L, key_M, 
    key_N, key_O, key_P, key_Q, key_R, key_S, 
    key_T, key_U, key_V, key_W, key_X, key_Y, key_Z,
    key_0, key_1, key_2, key_3, key_4, key_5, key_6, key_7, key_8, key_9,
    key_space, key_enter, key_escape,
    key_up, key_down, key_left, key_right,
    key_left_ctrl, key_tab,
    BUTTONS_COUNT,
  };
} key_button;

void merge(key_button *b, const key_button *b_other);
void reset_button_to_zero(key_button *b);

typedef struct {
  enum {
    con_A = GLFW_GAMEPAD_BUTTON_A,           
    con_B = GLFW_GAMEPAD_BUTTON_B,           
    con_X = GLFW_GAMEPAD_BUTTON_X,           
    con_Y = GLFW_GAMEPAD_BUTTON_Y,           
    con_left_bumper = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER, 
    con_right_bumper = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
    con_back = GLFW_GAMEPAD_BUTTON_BACK,
    con_start = GLFW_GAMEPAD_BUTTON_START,       
    con_guide = GLFW_GAMEPAD_BUTTON_GUIDE,      
    con_left_thumb = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,  
    con_right_thumb = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB, 
    con_up = GLFW_GAMEPAD_BUTTON_DPAD_UP,   
    con_right = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,  
    con_down = GLFW_GAMEPAD_BUTTON_DPAD_DOWN, 
    con_left = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,  
  };
  key_button con_buttons[GLFW_GAMEPAD_BUTTON_LAST + 1];
  float con_lt;
  float con_rt;
  vec2 con_l_stick, con_r_stick;
} controller_buttons;

void set_all_to_zero(controller_buttons *c);

int is_button_held(int key);
int is_button_pressed_on(int key);
int is_button_released(int key);
int is_button_typed(int key);
int is_left_mouse_pressed();
int is_right_mouse_pressed();
int is_left_mouse_released();
int is_right_mouse_released();
int is_left_mouse_held();
int is_right_mouse_held();
controller_buttons get_controller_buttons();
char **get_typed_input();
void set_button_state(int button, int newState);
void set_left_mouse_state(int newState);
void set_right_mouse_state(int newState);
void process_event_button(key_button *b, bool newState);
void update_button(key_button *b, float deltaTime);
void update_all_buttons(float deltaTime);
void reset_inputs_to_zero();
void add_to_typed_input(char c);
void reset_typed_input();

