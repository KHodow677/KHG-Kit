#include "khgenv/input.h"
#include "khgenv/gameScripting.h"
#include <string.h>

key_button keyboard[BUTTONS_COUNT];
key_button left_mouse;
key_button right_mouse;

controller_buttons c_buttons;
char ** typed_input;

void merge(key_button *b, const key_button *b_other) {
  b->pressed |= b_other->pressed;
  b->released |= b_other->released;
  b->held |= b_other->held;
}

void reset_button_to_zero(key_button *b) {
  b->pressed = 0;
  b->held = 0;
  b->released = 0;
}

void set_all_to_zero(controller_buttons *c) {
  key_button buttons[GLFW_GAMEPAD_BUTTON_LAST + 1];
  vec2 zero_vec2 = { 0.0f, 0.0f };
  *c->con_buttons = *buttons;
  c->con_lt = 0.0f;
  c->con_rt = 0.0f;
  c->con_l_stick = zero_vec2;
  c->con_r_stick = zero_vec2;
}

int is_button_held(int key) {
  if (key < key_A || key >= BUTTONS_COUNT) {
    return 0;
  }
  return keyboard[key].held;
}

int is_button_pressed_on(int key) {
  if (key < key_A || key >= BUTTONS_COUNT) {
    return 0;
  }
  return keyboard[key].pressed;
}

int is_button_released(int key) {
  if (key < key_A || key >= BUTTONS_COUNT) {
    return 0;
  }
  return keyboard[key].released;
}

int is_button_typed(int key) {
  if (key < key_A || key >= BUTTONS_COUNT) {
    return 0;
  }
  return keyboard[key].typed;
}

int is_left_mouse_pressed() {
  return left_mouse.pressed;
}

int is_right_mouse_pressed() {
  return right_mouse.pressed;
}

int is_left_mouse_released() {
  return left_mouse.released;
}

int is_right_mouse_released() {
  return right_mouse.released;
}

int is_left_mouse_held() {
  return left_mouse.held;
}

int is_right_mouse_held() {
  return right_mouse.held;
}

controller_buttons get_controller_buttons() {
  if (isFocused()) {
    return c_buttons;
  }
  controller_buttons c;
  set_all_to_zero(&c);
  return c;
}

char **get_typed_input() {
  return typed_input;
}

void set_button_state(int button, int new_state) {
  process_event_button(&keyboard[button], new_state);
}

void set_left_mouse_state(int new_state) {
  process_event_button(&left_mouse, new_state);
}
void set_right_mouse_state(int new_state) {
  process_event_button(&right_mouse, new_state);
}

void process_event_button(key_button *b, bool new_state) {
  b->newState = new_state;
}

void update_button(key_button *b, float delta_time) {
  if (b->newState == 1) {
    if (b->held) {
      b->pressed = false;
    }
    else {
      b->pressed = true;
    }
    b->held = true;
    b->released = false;
  }
  else if (b->newState == 0) {
    b->held = false;
    b->pressed = false;
    b->released = true;
  }
  else {
    b->pressed = false;
    b->released = false;
  }
  if (b->pressed) {
    b->typed = true;
    b->typedTime = 0.48f;
  }
  else if (b->held) {
    b->typedTime -= delta_time;
    if (b->typedTime < 0.0f) {
      b->typedTime += 0.07f;
      b->typed = true;
    }
    else {
      b->typed = false;
    }
  }
  else {
    b->typedTime = 0;
    b->typed = false;
  }
  b->newState = -1;
}

void update_all_buttons(float delta_time) {
	for (int i = 0; i < BUTTONS_COUNT; i++) {
		update_button(&keyboard[i], delta_time);
	}
	update_button(&left_mouse, delta_time);
	update_button(&right_mouse, delta_time);
	for(int i=0; i<=GLFW_JOYSTICK_LAST; i++) {
		if(glfwJoystickPresent(i) && glfwJoystickIsGamepad(i)) {
			GLFWgamepadstate state;
			if (glfwGetGamepadState(i, &state)) {
				for (int b = 0; b <= GLFW_GAMEPAD_BUTTON_LAST; b++) {
          if(state.buttons[b] == GLFW_PRESS) {
						process_event_button(&c_buttons.con_buttons[b], 1);
					}
          else if (state.buttons[b] == GLFW_RELEASE) {
						process_event_button(&c_buttons.con_buttons[b], 0);
					}
					update_button(&c_buttons.con_buttons[b], delta_time);
				}
				c_buttons.con_rt = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
				c_buttons.con_lt = state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
				c_buttons.con_l_stick.x = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
				c_buttons.con_l_stick.y = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
				c_buttons.con_r_stick.x = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
				c_buttons.con_r_stick.y = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
				break;
			}
		}
	}
}

void reset_inputs_to_zero() {
  reset_typed_input();
  for (int i = 0; i < BUTTONS_COUNT; i++) {
    reset_button_to_zero(&keyboard[i]);
  }
  reset_button_to_zero(&left_mouse);
  reset_button_to_zero(&right_mouse);
  set_all_to_zero(&c_buttons);
}

void add_to_typed_input(char c) {
  strcat(*typed_input, &c);
}

void reset_typed_input() {
  memset(&typed_input, 0, sizeof(typed_input));
}
