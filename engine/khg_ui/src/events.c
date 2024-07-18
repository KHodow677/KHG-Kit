#include "khg_ui/events.h"
#include "khg_ui/internal.h"

void ui_add_key_callback(void *cb) {
  state.input.key_cbs[state.input.key_cb_count++] = (KEY_CALLBACK_t)cb;
}

void ui_add_mouse_button_callback(void *cb) {
  state.input.mouse_button_cbs[state.input.mouse_button_cb_count++] = (MOUSE_BUTTON_CALLBACK_t)cb;
}

void ui_add_scroll_callback(void *cb) {
  state.input.scroll_cbs[state.input.scroll_cb_count++] = (SCROLL_CALLBACK_t)cb;
}

void ui_add_cursor_pos_callback(void *cb) {
  state.input.cursor_pos_cbs[state.input.cursor_pos_cb_count++] = (CURSOR_CALLBACK_t)cb;
}

bool ui_key_went_down(uint32_t key) {
  return ui_key_changed(key) && state.input.keyboard.keys[key];
}

bool ui_key_is_down(uint32_t key) {
  return state.input.keyboard.keys[key];
}

bool ui_key_is_released(uint32_t key) {
  return ui_key_changed(key) && !state.input.keyboard.keys[key];
}

bool ui_key_changed(uint32_t key) {
  bool ret = state.input.keyboard.keys_changed[key];
  state.input.keyboard.keys_changed[key] = false;
  return ret;
}

bool ui_mouse_button_went_down(uint32_t button) {
  return ui_mouse_button_changed(button) && state.input.mouse.buttons_current[button];
}

bool ui_mouse_button_is_down(uint32_t button) {
  return state.input.mouse.buttons_current[button];
}

bool ui_mouse_button_is_released(uint32_t button) {
  return ui_mouse_button_changed(button) && !state.input.mouse.buttons_current[button];
}

bool ui_mouse_button_changed(uint32_t button) {
  return state.input.mouse.buttons_current[button] != state.input.mouse.buttons_last[button];
}

bool ui_mouse_button_went_down_on_div(uint32_t button) {
  return ui_mouse_button_went_down(button) && state.scrollbar_div.id == state.current_div.id;
}

bool ui_mouse_button_is_released_on_div(uint32_t button) {
  return ui_mouse_button_is_released(button) && state.scrollbar_div.id == state.current_div.id;
}

bool ui_mouse_button_changed_on_div(uint32_t button) {
  return ui_mouse_button_changed(button) && state.scrollbar_div.id == state.current_div.id;
}

double ui_get_mouse_x() {
  return state.input.mouse.xpos;
}

double ui_get_mouse_y() {
  return state.input.mouse.ypos;
}

double ui_get_mouse_x_delta() {
  return state.input.mouse.xpos_delta;
}

double ui_get_mouse_y_delta() {
  return state.input.mouse.ypos_delta;
}

double ui_get_mouse_scroll_x() {
  return state.input.mouse.xscroll_delta;
}

double ui_get_mouse_scroll_y() {
  return state.input.mouse.yscroll_delta;
}


