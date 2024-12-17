#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  int32_t key_code;
  bool happened, pressed;
} gfx_key_event;

typedef struct {
  int32_t button_code;
  bool happened, pressed;
} gfx_mouse_button_event;

typedef struct {
  int32_t x, y;
  bool happened;
} gfx_cursor_pos_event;

typedef struct {
  int32_t x_offset, y_offset;
  bool happened;
} gfx_scroll_event;

typedef struct {
  int32_t char_code;
  bool happened;
} gfx_char_event;

void gfx_add_key_callback(void *cb);
void gfx_add_mouse_button_callback(void *cb);
void gfx_add_scroll_callback(void *cb);
void gfx_add_cursor_pos_callback(void *cb);

bool gfx_key_went_down(uint32_t key);
bool gfx_key_is_down(uint32_t key);
bool gfx_key_is_released(uint32_t key);
bool gfx_key_changed(uint32_t key);

bool gfx_mouse_button_went_down(uint32_t button);
bool gfx_mouse_button_is_down(uint32_t button);
bool gfx_mouse_button_is_released(uint32_t button);
bool gfx_mouse_button_changed(uint32_t button);
bool gfx_mouse_button_went_down_on_div(uint32_t button);
bool gfx_mouse_button_is_released_on_div(uint32_t button);
bool gfx_mouse_button_changed_on_div(uint32_t button);

double gfx_get_mouse_x(void);
double gfx_get_mouse_y(void);
double gfx_get_mouse_x_delta(void);
double gfx_get_mouse_y_delta(void);
double gfx_get_mouse_scroll_x(void);
double gfx_get_mouse_scroll_y(void);

gfx_cursor_pos_event gfx_mouse_move_event_occur(void);
gfx_mouse_button_event gfx_mouse_button_event_occur(void);
gfx_scroll_event gfx_mouse_scroll_event_occur(void);
gfx_key_event gfx_key_event_occur(void);
gfx_char_event gfx_char_event_occur(void);

void gfx_set_cull_start_x(float x);
void gfx_set_cull_start_y(float y);
void gfx_set_cull_end_x(float x);
void gfx_set_cull_end_y(float y);  
void gfx_unset_cull_start_x(void);
void gfx_unset_cull_start_y(void);
void gfx_unset_cull_end_x(void);
void gfx_unset_cull_end_y(void);

