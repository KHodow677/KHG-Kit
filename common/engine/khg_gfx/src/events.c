#include "khg_gfx/events.h"
#include "khg_gfx/internal.h"

void gfx_add_key_callback(void *cb) {
  GFX_STATE.input.key_cbs[GFX_STATE.input.key_cb_count++] = (GFX_KEY_CALLBACK)cb;
}

void gfx_add_mouse_button_callback(void *cb) {
  GFX_STATE.input.mouse_button_cbs[GFX_STATE.input.mouse_button_cb_count++] = (GFX_MOUSE_BUTTON_CALLBACK)cb;
}

void gfx_add_scroll_callback(void *cb) {
  GFX_STATE.input.scroll_cbs[GFX_STATE.input.scroll_cb_count++] = (GFX_SCROLL_CALLBACK)cb;
}

void gfx_add_cursor_pos_callback(void *cb) {
  GFX_STATE.input.cursor_pos_cbs[GFX_STATE.input.cursor_pos_cb_count++] = (GFX_CURSOR_CALLBACK)cb;
}

bool gfx_key_went_down(unsigned int key) {
  return gfx_key_changed(key) && GFX_STATE.input.keyboard.keys[key];
}

bool gfx_key_is_down(unsigned int key) {
  return GFX_STATE.input.keyboard.keys[key];
}

bool gfx_key_is_released(unsigned int key) {
  return gfx_key_changed(key) && !GFX_STATE.input.keyboard.keys[key];
}

bool gfx_key_changed(unsigned int key) {
  bool ret = GFX_STATE.input.keyboard.keys_changed[key];
  GFX_STATE.input.keyboard.keys_changed[key] = false;
  return ret;
}

bool gfx_mouse_button_went_down(unsigned int button) {
  return gfx_mouse_button_changed(button) && GFX_STATE.input.mouse.buttons_current[button];
}

bool gfx_mouse_button_is_down(unsigned int button) {
  return GFX_STATE.input.mouse.buttons_current[button];
}

bool gfx_mouse_button_is_released(unsigned int button) {
  return gfx_mouse_button_changed(button) && !GFX_STATE.input.mouse.buttons_current[button];
}

bool gfx_mouse_button_changed(unsigned int button) {
  return GFX_STATE.input.mouse.buttons_current[button] != GFX_STATE.input.mouse.buttons_last[button];
}

bool gfx_mouse_button_went_down_on_div(unsigned int button) {
  return gfx_mouse_button_went_down(button) && GFX_STATE.scrollbar_div.id == GFX_STATE.current_div.id;
}

bool gfx_mouse_button_is_released_on_div(unsigned int button) {
  return gfx_mouse_button_is_released(button) && GFX_STATE.scrollbar_div.id == GFX_STATE.current_div.id;
}

bool gfx_mouse_button_changed_on_div(unsigned int button) {
  return gfx_mouse_button_changed(button) && GFX_STATE.scrollbar_div.id == GFX_STATE.current_div.id;
}

float gfx_get_mouse_x() {
  return GFX_STATE.input.mouse.xpos;
}

float gfx_get_mouse_y() {
  return GFX_STATE.input.mouse.ypos;
}

float gfx_get_mouse_x_delta() {
  return GFX_STATE.input.mouse.xpos_delta;
}

float gfx_get_mouse_y_delta() {
  return GFX_STATE.input.mouse.ypos_delta;
}

float gfx_get_mouse_scroll_x() {
  return GFX_STATE.input.mouse.xscroll_delta;
}

float gfx_get_mouse_scroll_y() {
  return GFX_STATE.input.mouse.yscroll_delta;
}

gfx_cursor_pos_event gfx_mouse_move_event_occur() {
  return GFX_STATE.cp_ev;
}

gfx_mouse_button_event gfx_mouse_button_event_occur() {
  return GFX_STATE.mb_ev;
}

gfx_scroll_event gfx_mouse_scroll_event_occur() {
  return GFX_STATE.scr_ev;
}

gfx_key_event gfx_key_event_occur() {
  return GFX_STATE.key_ev;
}

gfx_char_event gfx_char_event_occur() { 
  return GFX_STATE.ch_ev;
}

void gfx_set_cull_end_x(float x) {
  GFX_STATE.cull_end.x = x; 
}

void gfx_set_cull_end_y(float y) {
  GFX_STATE.cull_end.y = y; 
}

void gfx_set_cull_start_x(float x) {
  GFX_STATE.cull_start.x = x;
}

void gfx_set_cull_start_y(float y) {
  GFX_STATE.cull_start.y = y;
}

void gfx_unset_cull_start_x() {
  GFX_STATE.cull_start.x = -1;
}

void gfx_unset_cull_start_y() {
  GFX_STATE.cull_start.y = -1;
}

void gfx_unset_cull_end_x() {
  GFX_STATE.cull_end.x = -1;
}

void gfx_unset_cull_end_y() {
  GFX_STATE.cull_end.y = -1;
}

