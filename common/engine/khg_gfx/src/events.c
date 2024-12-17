#include "khg_gfx/events.h"
#include "khg_gfx/internal.h"

void gfx_add_key_callback(void *cb) {
  state.input.key_cbs[state.input.key_cb_count++] = (KEY_CALLBACK_t)cb;
}

void gfx_add_mouse_button_callback(void *cb) {
  state.input.mouse_button_cbs[state.input.mouse_button_cb_count++] = (MOUSE_BUTTON_CALLBACK_t)cb;
}

void gfx_add_scroll_callback(void *cb) {
  state.input.scroll_cbs[state.input.scroll_cb_count++] = (SCROLL_CALLBACK_t)cb;
}

void gfx_add_cursor_pos_callback(void *cb) {
  state.input.cursor_pos_cbs[state.input.cursor_pos_cb_count++] = (CURSOR_CALLBACK_t)cb;
}

bool gfx_key_went_down(uint32_t key) {
  return gfx_key_changed(key) && state.input.keyboard.keys[key];
}

bool gfx_key_is_down(uint32_t key) {
  return state.input.keyboard.keys[key];
}

bool gfx_key_is_released(uint32_t key) {
  return gfx_key_changed(key) && !state.input.keyboard.keys[key];
}

bool gfx_key_changed(uint32_t key) {
  bool ret = state.input.keyboard.keys_changed[key];
  state.input.keyboard.keys_changed[key] = false;
  return ret;
}

bool gfx_mouse_button_went_down(uint32_t button) {
  return gfx_mouse_button_changed(button) && state.input.mouse.buttons_current[button];
}

bool gfx_mouse_button_is_down(uint32_t button) {
  return state.input.mouse.buttons_current[button];
}

bool gfx_mouse_button_is_released(uint32_t button) {
  return gfx_mouse_button_changed(button) && !state.input.mouse.buttons_current[button];
}

bool gfx_mouse_button_changed(uint32_t button) {
  return state.input.mouse.buttons_current[button] != state.input.mouse.buttons_last[button];
}

bool gfx_mouse_button_went_down_on_div(uint32_t button) {
  return gfx_mouse_button_went_down(button) && state.scrollbar_div.id == state.current_div.id;
}

bool gfx_mouse_button_is_released_on_div(uint32_t button) {
  return gfx_mouse_button_is_released(button) && state.scrollbar_div.id == state.current_div.id;
}

bool gfx_mouse_button_changed_on_div(uint32_t button) {
  return gfx_mouse_button_changed(button) && state.scrollbar_div.id == state.current_div.id;
}

double gfx_get_mouse_x() {
  return state.input.mouse.xpos;
}

double gfx_get_mouse_y() {
  return state.input.mouse.ypos;
}

double gfx_get_mouse_x_delta() {
  return state.input.mouse.xpos_delta;
}

double gfx_get_mouse_y_delta() {
  return state.input.mouse.ypos_delta;
}

double gfx_get_mouse_scroll_x() {
  return state.input.mouse.xscroll_delta;
}

double gfx_get_mouse_scroll_y() {
  return state.input.mouse.yscroll_delta;
}

gfx_cursor_pos_event gfx_mouse_move_event_occur() {
  return state.cp_ev;
}

gfx_mouse_button_event gfx_mouse_button_event_occur() {
  return state.mb_ev;
}

gfx_scroll_event gfx_mouse_scroll_event_occur() {
  return state.scr_ev;
}

gfx_key_event gfx_key_event_occur() {
  return state.key_ev;
}

gfx_char_event gfx_char_event_occur() { 
  return state.ch_ev;
}

void gfx_set_cull_end_x(float x) {
  state.cull_end.x = x; 
}

void gfx_set_cull_end_y(float y) {
  state.cull_end.y = y; 
}

void gfx_set_cull_start_x(float x) {
  state.cull_start.x = x;
}

void gfx_set_cull_start_y(float y) {
  state.cull_start.y = y;
}

void gfx_unset_cull_start_x() {
  state.cull_start.x = -1;
}

void gfx_unset_cull_start_y() {
  state.cull_start.y = -1;
}

void gfx_unset_cull_end_x() {
  state.cull_end.x = -1;
}

void gfx_unset_cull_end_y() {
  state.cull_end.y = -1;
}

