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

double ui_get_mouse_x();
double ui_get_mouse_y();
double ui_get_mouse_x_delta();
double ui_get_mouse_y_delta();
double ui_get_mouse_scroll_x();
double ui_get_mouse_scroll_y();
