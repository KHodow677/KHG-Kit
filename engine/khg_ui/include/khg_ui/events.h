#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  int32_t key_code;
  bool happened, pressed;
} ui_key_event;

typedef struct {
  int32_t button_code;
  bool happened, pressed;
} ui_mouse_button_event;

typedef struct {
  int32_t x, y;
  bool happened;
} ui_cursor_pos_event;

typedef struct {
  int32_t x_offset, y_offset;
  bool happened;
} ui_scroll_event;

typedef struct {
  int32_t char_code;
  bool happened;
} ui_char_event;

void ui_add_key_callback(void *cb);
void ui_add_mouse_button_callback(void *cb);
void ui_add_scroll_callback(void *cb);
void ui_add_cursor_pos_callback(void *cb);

bool ui_key_went_down(uint32_t key);
bool ui_key_is_down(uint32_t key);
bool ui_key_is_released(uint32_t key);
bool ui_key_changed(uint32_t key);

bool ui_mouse_button_went_down(uint32_t button);
bool ui_mouse_button_is_down(uint32_t button);
bool ui_mouse_button_is_released(uint32_t button);
bool ui_mouse_button_changed(uint32_t button);
bool ui_mouse_button_went_down_on_div(uint32_t button);
bool ui_mouse_button_is_released_on_div(uint32_t button);
bool ui_mouse_button_changed_on_div(uint32_t button);

double ui_get_mouse_x(void);
double ui_get_mouse_y(void);
double ui_get_mouse_x_delta(void);
double ui_get_mouse_y_delta(void);
double ui_get_mouse_scroll_x(void);
double ui_get_mouse_scroll_y(void);

ui_cursor_pos_event ui_mouse_move_event_occur(void);
ui_mouse_button_event ui_mouse_button_event_occur(void);
ui_scroll_event ui_mouse_scroll_event_occur(void);
ui_key_event ui_key_event_occur(void);
ui_char_event ui_char_event_occur(void);

void ui_set_cull_start_x(float x);
void ui_set_cull_start_y(float y);
void ui_set_cull_end_x(float x);
void ui_set_cull_end_y(float y);  
void ui_unset_cull_start_x(void);
void ui_unset_cull_start_y(void);
void ui_unset_cull_end_x(void);
void ui_unset_cull_end_y(void);

