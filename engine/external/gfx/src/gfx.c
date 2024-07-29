#include "gfx/gfx.h"
#include "gfx/elements.h"
#include "gfx/inputs.h"
#include "gfx/renderer.h"
#include "gfx/stb.h"
#include "gfx/strman.h"
#include "khg_utl/error_func.h"
#include <cglm/mat4.h>
#include <cglm/types-struct.h>
#include <time.h>
#include <GLFW/glfw3.h>

#include <locale.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <wchar.h>

LfState state;

void lf_init_glfw(uint32_t display_width, uint32_t display_height, void *glfw_window) {
  setlocale(LC_ALL, "");
  if(!glfwInit()) {
    error_func("Trying to initialize khg_gfx with GLFW without initializing GLFW first", user_defined_data);
    return;
  }
  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    error_func("Failed to initialize glad", user_defined_data);
    return;
  }
  memset(&state, 0, sizeof(state));
  state.init = true;
  state.dsp_w = display_width;
  state.dsp_h = display_height;
  state.window_handle = glfw_window;
  state.input.mouse.first_mouse_press = true;
  state.render.tex_count = 0;
  state.pos_ptr = (vec2s){0, 0};
  state.image_color_stack = LF_NO_COLOR;
  state.active_element_id = 0;
  state.text_wrap = false;
  state.line_overflow = true;
  state.theme = lf_default_theme();
  state.renderer_render = true;
  state.drag_state = (DragState){ false, {0, 0}, 0 };
  props_stack_create(&state.props_stack);
  memset(&state.grabbed_div, 0, sizeof(LfDiv));
  state.grabbed_div.id = -1;
  state.clipboard = clipboard_new(NULL);
  state.drawcalls = 0;
  glfwSetKeyCallback((GLFWwindow *)state.window_handle, glfw_key_callback);
  glfwSetMouseButtonCallback((GLFWwindow *)state.window_handle, glfw_mouse_button_callback);
  glfwSetScrollCallback((GLFWwindow *)state.window_handle, glfw_scroll_callback);
  glfwSetCursorPosCallback((GLFWwindow *)state.window_handle, glfw_cursor_callback);
  glfwSetCharCallback((GLFWwindow *)state.window_handle, glfw_char_callback);
  renderer_init();
  state.tex_arrow_down = lf_load_texture_asset("arrow-down", "png");
  state.tex_tick = lf_load_texture_asset("tick", "png");
}

void lf_terminate() {
  lf_free_font(&state.theme.font);
}

LfTheme lf_default_theme() {
  LfTheme theme = { 0 };
  theme.div_props = (LfUIElementProps){
    .color = (LfColor){45, 45, 45, 255},
    .border_color = (LfColor){0, 0, 0, 0}, 
    .border_width = 0.0f, 
    .corner_radius = 0.0f,
    .hover_color = LF_NO_COLOR,
  };
  float global_padding = 10;
  float global_margin = 5;
  theme.text_props = (LfUIElementProps){
    .text_color = LF_WHITE, 
    .border_color = LF_NO_COLOR,
    .padding = 0, 
    .margin_left = global_margin, 
    .margin_right = global_margin, 
    .margin_top = global_margin, 
    .margin_bottom = global_margin,
    .border_width = global_margin,
    .corner_radius = 0, 
    .hover_color = LF_NO_COLOR, 
    .hover_text_color = LF_NO_COLOR
  };
  theme.button_props = (LfUIElementProps){ 
    .color = LF_PRIMARY_ITEM_COLOR, 
    .text_color = LF_BLACK,
    .border_color = LF_SECONDARY_ITEM_COLOR,
    .padding = global_padding,
    .margin_left = global_margin, 
    .margin_right = global_margin, 
    .margin_top = global_margin, 
    .margin_bottom = global_margin, 
    .border_width = 4, 
    .corner_radius = 0, 
    .hover_color = LF_NO_COLOR, 
    .hover_text_color = LF_NO_COLOR
  };
  theme.image_props = (LfUIElementProps){ 
    .color = LF_WHITE,
    .margin_left = global_margin, 
    .margin_right = global_margin, 
    .margin_top = global_margin, 
    .margin_bottom = global_margin,
    .corner_radius = 0,
    .hover_color = LF_NO_COLOR, 
    .hover_text_color = LF_NO_COLOR
  };
  theme.inputfield_props = (LfUIElementProps){ 
    .color = LF_PRIMARY_ITEM_COLOR,
    .text_color = LF_BLACK,
    .border_color = LF_SECONDARY_ITEM_COLOR,
    .padding = global_padding, 
    .margin_left = global_margin, 
    .margin_right = global_margin, 
    .margin_top = global_margin, 
    .margin_bottom = global_margin, 
    .border_width = 4,
    .corner_radius = 0,
    .hover_color = LF_NO_COLOR, 
    .hover_text_color = LF_NO_COLOR
  };
  theme.checkbox_props = (LfUIElementProps){ 
    .color = LF_PRIMARY_ITEM_COLOR, 
    .text_color = LF_WHITE, 
    .border_color = LF_SECONDARY_ITEM_COLOR,
    .padding = global_padding, 
    .margin_left = global_margin, 
    .margin_right = global_margin, 
    .margin_top = global_margin, 
    .margin_bottom = global_margin, 
    .border_width = 4,
    .corner_radius = 0,
    .hover_color = LF_NO_COLOR, 
    .hover_text_color = LF_NO_COLOR
  };
  theme.slider_props = (LfUIElementProps){ 
    .color = LF_PRIMARY_ITEM_COLOR, 
    .text_color = LF_SECONDARY_ITEM_COLOR, 
    .border_color = LF_SECONDARY_ITEM_COLOR,
    .padding = global_padding,
    .margin_left = global_margin, 
    .margin_right = global_margin, 
    .margin_top = global_margin, 
    .margin_bottom = global_margin, 
    .border_width = 4,
    .corner_radius = 0,
    .hover_color = LF_NO_COLOR, 
    .hover_text_color = LF_NO_COLOR
  };
  theme.scrollbar_props = (LfUIElementProps){ 
    .color = LF_SECONDARY_ITEM_COLOR, 
    .border_color = LF_BLACK,
    .padding = 0,
    .margin_left = 0, 
    .margin_right = 5, 
    .margin_top = 5, 
    .margin_bottom = 0, 
    .border_width = 0,
    .corner_radius = 0,
    .hover_color = LF_NO_COLOR, 
    .hover_text_color = LF_NO_COLOR
  };
  theme.font = lf_load_font_asset("inter", "ttf", 24);
  theme.div_scroll_max_velocity = 100.0f; 
  theme.div_scroll_velocity_deceleration = 0.92;
  theme.div_scroll_acceleration = 2.5f;
  theme.div_scroll_amount_px = 20.0f;
  theme.div_smooth_scroll = true;
  theme.scrollbar_width = 8;
  return theme;
}

LfTheme lf_get_theme() {
  return state.theme;
}

void lf_set_theme(LfTheme theme) {
  state.theme = theme;
}

void lf_resize_display(uint32_t display_width, uint32_t display_height) {
  state.dsp_w = display_width;
  state.dsp_h = display_height;
  set_projection_matrix();
  state.current_div.aabb.size.x = state.dsp_w;
  state.current_div.aabb.size.y = state.dsp_h;
}

LfFont lf_load_font(const char *filepath, uint32_t size) {
  return load_font(filepath, size, 1024, 1024, 0);
}

LfFont lf_load_font_ex(const char *filepath, uint32_t size, uint32_t bitmap_w, uint32_t bitmap_h) {
  return load_font(filepath, size, bitmap_w, bitmap_h, 0);
}

LfTexture lf_load_texture_from_memory_resized(const void *data, size_t size, bool flip, LfTextureFiltering filter, uint32_t w, uint32_t h) {
  LfTexture tex;
  int32_t channels;
  unsigned char *resized = lf_load_texture_data_from_memory_resized(data, size, &channels, NULL, NULL, flip, w, h);
  lf_create_texture_from_image_data(LF_TEX_FILTER_LINEAR, &tex.id, w, h, channels, resized);

  tex.width = w;
  tex.height = h;

  return tex;
}

void lf_create_texture_from_image_data(LfTextureFiltering filter, uint32_t *id, int32_t width, int32_t height, int32_t channels, unsigned char *data) {
  GLenum internal_format = (channels == 4) ? GL_RGBA8 : GL_RGB8;
  GLenum data_format = (channels == 4) ? GL_RGBA : GL_RGB;
  glGenTextures(1, id);
  glBindTexture(GL_TEXTURE_2D, *id); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  switch(filter) {
    case LF_TEX_FILTER_LINEAR:
      glTextureParameteri(*id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTextureParameteri(*id, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      break;
    case LF_TEX_FILTER_NEAREST:
      glTextureParameteri(*id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTextureParameteri(*id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      break;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, data_format, width, height, 0, data_format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
}

void lf_free_texture(LfTexture *tex) {
  glDeleteTextures(1, &tex->id);
  memset(tex, 0, sizeof(LfTexture));
}

void lf_free_font(LfFont *font) {
  free(font->cdata);
  free(font->font_info);
}

LfFont lf_load_font_asset(const char *asset_name, const char *file_extension, uint32_t font_size) {
#if defined(_WIN32) || defined(_WIN64)
  char cwd[MAX_PATH];
  _getcwd(cwd, sizeof(cwd));
  size_t cwd_len = strlen(cwd);
  size_t asset_dir_len = cwd_len + strlen("\\res") + 1;
  size_t path_len = asset_dir_len + strlen("\\assets\\fonts\\") + strlen(asset_name) + strlen(".") + strlen(file_extension) + 1;
  char asset_dir[asset_dir_len];
  char path[path_len];
  snprintf(asset_dir, sizeof(asset_dir), "%s\\res", cwd);
  snprintf(path, sizeof(path), "%s\\assets\\fonts\\%s.%s", asset_dir, asset_name, file_extension);
  return lf_load_font(path, font_size);
#else 
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));
  char asset_dir[strlen(cwd) + strlen("/res")];
  memset(asset_dir, 0, sizeof(asset_dir));
  strcat(asset_dir, cwd);
  strcat(asset_dir, "/res");
  char path[strlen(asset_dir) + strlen("/assets/fonts/") + strlen(asset_name) + strlen(".") + strlen(file_extension)];
  memset(path, 0, sizeof(path));
  strcat(path, asset_dir);
  strcat(path, "/assets/fonts/");
  strcat(path, asset_name);
  strcat(path, ".");
  strcat(path, file_extension);
  return lf_load_font(path, font_size);
#endif
}

LfTexture lf_load_texture_asset(const char *asset_name, const char *file_extension) {
#if defined(_WIN32) || defined(_WIN64)
  char cwd[PATH_MAX];
  _getcwd(cwd, sizeof(cwd));
  size_t cwd_len = strlen(cwd);
  size_t asset_dir_len = cwd_len + strlen("\\res") + 1;
  size_t path_len = asset_dir_len + strlen("\\assets\\textures\\") + strlen(asset_name) + strlen(".") + strlen(file_extension) + 1;
  char asset_dir[asset_dir_len];
  char path[path_len];
  snprintf(asset_dir, sizeof(asset_dir), "%s\\res", cwd);
  snprintf(path, sizeof(path), "%s\\assets\\textures\\%s.%s", asset_dir, asset_name, file_extension);
  return lf_load_texture(path, false, LF_TEX_FILTER_LINEAR);
#else
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));
  char asset_dir[strlen(cwd) + strlen("/res")];
  memset(asset_dir, 0, sizeof(asset_dir));
  strcat(asset_dir, cwd);
  strcat(asset_dir, "/res");
  char path[strlen(asset_dir) + strlen("/assets/textures/") + strlen(asset_name) + strlen(".") + strlen(file_extension)];
  memset(path, 0, sizeof(path));
  strcat(path, asset_dir);
  strcat(path, "/assets/textures/");
  strcat(path, asset_name);
  strcat(path, ".");
  strcat(path, file_extension);
  return lf_load_texture(path, false, LF_TEX_FILTER_LINEAR);
#endif
}

void lf_add_key_callback(void *cb) {
  state.input.key_cbs[state.input.key_cb_count++] = (KEY_CALLBACK_t)cb;
}

void lf_add_mouse_button_callback(void *cb) {
  state.input.mouse_button_cbs[state.input.mouse_button_cb_count++] = (MOUSE_BUTTON_CALLBACK_t)cb;
}

void lf_add_scroll_callback(void *cb) {
  state.input.scroll_cbs[state.input.scroll_cb_count++] = (SCROLL_CALLBACK_t)cb;
}

void lf_add_cursor_pos_callback(void *cb) {
  state.input.cursor_pos_cbs[state.input.cursor_pos_cb_count++] = (CURSOR_CALLBACK_t)cb;
}

bool lf_key_went_down(uint32_t key) {
  return lf_key_changed(key) && state.input.keyboard.keys[key];
}

bool lf_key_is_down(uint32_t key) {
  return state.input.keyboard.keys[key];
}

bool lf_key_is_released(uint32_t key) {
  return lf_key_changed(key) && !state.input.keyboard.keys[key];
}

bool lf_key_changed(uint32_t key) {
  bool ret = state.input.keyboard.keys_changed[key];
  state.input.keyboard.keys_changed[key] = false;
  return ret;
}

bool lf_mouse_button_went_down(uint32_t button) {
  return lf_mouse_button_changed(button) && state.input.mouse.buttons_current[button];
}

bool lf_mouse_button_is_down(uint32_t button) {
  return state.input.mouse.buttons_current[button];
}

bool lf_mouse_button_is_released(uint32_t button) {
  return lf_mouse_button_changed(button) && !state.input.mouse.buttons_current[button];
}

bool lf_mouse_button_changed(uint32_t button) {
  return state.input.mouse.buttons_current[button] != state.input.mouse.buttons_last[button];
}

bool lf_mouse_button_went_down_on_div(uint32_t button) {
  return lf_mouse_button_went_down(button) && state.scrollbar_div.id == state.current_div.id;
}

bool lf_mouse_button_is_released_on_div(uint32_t button) {
  return lf_mouse_button_is_released(button) && state.scrollbar_div.id == state.current_div.id;
}

bool lf_mouse_button_changed_on_div(uint32_t button) {
  return lf_mouse_button_changed(button) && state.scrollbar_div.id == state.current_div.id;
}
double lf_get_mouse_x() {
  return state.input.mouse.xpos;
}

double lf_get_mouse_y() {
  return state.input.mouse.ypos;
}

double lf_get_mouse_x_delta() {
  return state.input.mouse.xpos_delta;
}

double lf_get_mouse_y_delta() {
  return state.input.mouse.ypos_delta;
}

double lf_get_mouse_scroll_x() {
  return state.input.mouse.xscroll_delta;
}

double lf_get_mouse_scroll_y() {
  return state.input.mouse.yscroll_delta;
}

LfDiv *_lf_div_begin_loc(vec2s pos, vec2s size, bool scrollable, float *scroll, float *scroll_velocity, const char *file, int32_t line) {
  bool hovered_div = lf_area_hovered(pos, size);
  if(hovered_div) {
    state.scroll_velocity_ptr = scroll_velocity;
    state.scroll_ptr = scroll;
  }
  uint64_t id = DJB2_INIT;
  id = djb2_hash(id, file, strlen(file));
  id = djb2_hash(id, &line, sizeof(line));
  if(state.element_id_stack != -1) {
    id = djb2_hash(id, &state.element_id_stack, sizeof(state.element_id_stack));
  }
  state.prev_pos_ptr = state.pos_ptr;
  state.prev_font_stack = state.font_stack;
  state.prev_line_height = state.current_line_height;
  state.prev_div = state.current_div;
  LfUIElementProps props = get_props_for(state.theme.div_props);
  state.div_props = props;
  LfDiv div;
  div.aabb = (LfAABB){.pos = pos, .size = size};
  div.scrollable = scrollable;
  div.id = id;
  if(div.scrollable) {
    if(*scroll > 0) {
      *scroll = 0;
    }
    if(state.theme.div_smooth_scroll) {
      *scroll += *scroll_velocity;
      *scroll_velocity *= state.theme.div_scroll_velocity_deceleration;
      if(*scroll_velocity > -0.1 && state.div_velocity_accelerating) {
        *scroll_velocity = 0.0f;
      }
    }
  }
  state.pos_ptr = pos; 
  state.current_div = div;
  div.interact_state = div_container((vec2s){ pos.x - props.padding, pos.y - props.padding }, (vec2s){ size.x + props.padding * 2.0f, size.y + props.padding * 2.0f }, props, props.color, props.border_width, false, state.div_hoverable);
  if(hovered_div) {
    state.selected_div_tmp = div;
  }
  if(div.scrollable) {
    lf_set_ptr_y(*scroll + props.border_width + props.corner_radius);
  } 
  else {
    lf_set_ptr_y(props.border_width + props.corner_radius);
  }
  state.cull_start = (vec2s){ pos.x, pos.y + props.border_width };
  state.cull_end = (vec2s){ pos.x + size.x - props.border_width, pos.y + size.y - props.border_width };
  state.current_div = div;
  state.current_line_height = 0;
  state.font_stack = NULL;
  return &state.current_div;
}

void lf_div_end() {
  if(state.current_div.scrollable) {
    draw_scrollbar_on(&state.selected_div_tmp);
  }
  state.pos_ptr = state.prev_pos_ptr;
  state.font_stack = state.prev_font_stack;
  state.current_line_height = state.prev_line_height;
  state.current_div = state.prev_div;
  state.cull_start = (vec2s){ -1, -1 };
  state.cull_end = (vec2s){ -1, -1 };
}

LfClickableItemState _lf_item_loc(vec2s size, const char *file, int32_t line) {
  LfUIElementProps props = get_props_for(state.theme.button_props);
  next_line_on_overflow( (vec2s){ size.x + props.padding * 2.0f + props.margin_right + props.margin_left, size.y + props.padding * 2.0f + props.margin_bottom + props.margin_top }, state.div_props.border_width);
  state.pos_ptr.x += props.margin_left;
  state.pos_ptr.y += props.margin_top;
  LfClickableItemState item = button(file, line, state.pos_ptr, size, props, props.color, props.border_width, false, true);
  state.pos_ptr.x += size.x + props.margin_left + props.padding * 2.0f;
  state.pos_ptr.y -= props.margin_top;
  return item;
}

LfClickableItemState _lf_button_loc(const char *text, const char *file, int32_t line) {
  return button_element_loc((void*)text, file, line, false);
}

LfClickableItemState _lf_button_wide_loc(const wchar_t *text, const char *file, int32_t line) {
  return button_element_loc((void*)text, file, line, true);
}

LfClickableItemState _lf_image_button_loc(LfTexture img, const char *file, int32_t line) {
  LfUIElementProps props = get_props_for(state.theme.button_props);
  float padding = props.padding;
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom = props.margin_bottom; 
  LfColor color = props.color;
  LfColor text_color = state.theme.button_props.text_color;
  LfFont font = get_current_font();
  next_line_on_overflow((vec2s){ img.width + padding * 2.0f, img.height + padding * 2.0f }, state.div_props.border_width);
  state.pos_ptr.x += margin_left;
  state.pos_ptr.y += margin_top;
  LfClickableItemState ret = button(file, line, state.pos_ptr, (vec2s){ img.width + padding * 2, img.height + padding * 2 }, props, color, props.border_width, true, true);
  LfColor imageColor = LF_WHITE;
  lf_image_render((vec2s){state.pos_ptr.x + padding, state.pos_ptr.y + padding}, imageColor, img, LF_NO_COLOR, 0, props.corner_radius);
  state.pos_ptr.x += img.width + margin_right + padding * 2.0f;
  state.pos_ptr.y -= margin_top;
  return ret; 
}

LfClickableItemState _lf_image_button_fixed_loc(LfTexture img, float width, float height, const char *file, int32_t line) {
  LfUIElementProps props = get_props_for(state.theme.button_props);
  float padding = props.padding;
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom = props.margin_bottom; 
  LfColor color = props.color;
  LfColor text_color = state.theme.button_props.text_color;
  LfFont font = get_current_font();
  int32_t render_width = ((width == -1) ? img.width : width);
  int32_t render_height = ((height == -1) ? img.height : height);
  next_line_on_overflow( (vec2s){ render_width + padding * 2.0f, render_height + padding * 2.0f }, state.div_props.border_width);
  state.pos_ptr.x += margin_left;
  state.pos_ptr.y += margin_top;
  LfClickableItemState ret = button(file, line, state.pos_ptr, (vec2s){ render_width + padding * 2, render_height + padding * 2 }, props, color, props.border_width, true, true);
  LfColor imageColor = LF_WHITE; 
  lf_image_render((vec2s){ state.pos_ptr.x + padding + (render_width - img.width) / 2.0f, state.pos_ptr.y + padding }, imageColor, img, LF_NO_COLOR, 0, props.corner_radius);
  state.pos_ptr.x += render_width + margin_right + padding * 2.0f;
  state.pos_ptr.y -= margin_top;
  return ret; 
}

LfClickableItemState _lf_button_fixed_loc(const char *text, float width, float height, const char *file, int32_t line) {
  return button_fixed_element_loc((void*)text, width, height, file, line, false);
}

LfClickableItemState _lf_button_fixed_loc_wide(const wchar_t *text, float width, float height, const char *file, int32_t line) {
  return button_fixed_element_loc((void*)text, width, height, file, line, true);
}

LfClickableItemState _lf_slider_int_loc(LfSlider *slider, const char *file, int32_t line) {
  LfUIElementProps props = get_props_for(state.theme.button_props);
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom =props.margin_bottom;
  float handle_size;
  if(slider->handle_size != 0.0f) {
    handle_size = slider->handle_size; 
  }
  else {
    handle_size = (slider->height != 0) ? slider->height * 4 : 20;
  }
  if(slider->held) {
    handle_size = (slider->height != 0) ? slider->height * 4.5 : 22.5;
  }
  float slider_width = (slider->width != 0) ? slider->width : 200;
  float slider_height = (slider->height != 0) ? slider->height : handle_size / 2.0f;
  LfColor color = props.color;
  next_line_on_overflow((vec2s){ slider_width + margin_right + margin_left, handle_size + margin_bottom + margin_top }, state.div_props.border_width);
  state.pos_ptr.x += margin_left;
  state.pos_ptr.y += margin_top;
  LfUIElementProps slider_props = props;
  slider_props.border_width /= 2.0f;
  LfClickableItemState slider_state = button_ex(file, line, state.pos_ptr, (vec2s){ (float)slider_width, (float)slider_height}, slider_props, color, 0, false, false, (vec2s){ -1, handle_size });
  slider->handle_pos = map_vals(*(int32_t *)slider->val, slider->min, slider->max, handle_size / 2.0f, slider->width - handle_size / 2.0f) - (handle_size) / 2.0f;
  lf_rect_render((vec2s){ state.pos_ptr.x + slider->handle_pos, state.pos_ptr.y - (handle_size) / 2.0f + slider_height / 2.0f }, (vec2s){ handle_size, handle_size }, props.text_color, props.border_color, props.border_width, slider->held ? props.corner_radius * 3.5f : props.corner_radius * 3.0f);
  if(slider_state == LF_HELD || slider_state == LF_CLICKED) {
    slider->held = true;
  }
  if(slider->held && lf_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)) {
    slider->held = false;
    slider_state = LF_CLICKED;
  }
  if(slider->held) {
    if(lf_get_mouse_x() >= state.pos_ptr.x && lf_get_mouse_x() <= state.pos_ptr.x + slider_width - handle_size) {
      slider->handle_pos = lf_get_mouse_x() - state.pos_ptr.x;
      *(int32_t *)slider->val = map_vals(state.pos_ptr.x + slider->handle_pos, state.pos_ptr.x,  state.pos_ptr.x + slider_width - handle_size, slider->min, slider->max);
    } 
    else if(lf_get_mouse_x() <= state.pos_ptr.x) {
      *(int32_t *)slider->val = slider->min;
      slider->handle_pos = 0;
    } 
    else if(lf_get_mouse_x() >= state.pos_ptr.x + slider_width - handle_size) {
      *(int32_t *)slider->val = slider->max;
      slider->handle_pos = slider_width - handle_size;
    }
    slider_state = LF_HELD;
  }
  state.pos_ptr.x += slider_width + margin_right;
  state.pos_ptr.y -= margin_top;
  return slider_state;
}

LfClickableItemState _lf_progress_bar_val_loc(float width, float height, int32_t min, int32_t max, int32_t val, const char *file, int32_t line) {
  LfUIElementProps props = get_props_for(state.theme.slider_props);
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom = props.margin_bottom; 
  const float handle_size = (height == -1) ? 10 : height * 2;
  const float slider_width = (width == -1) ? 200 : width;
  const float slider_height = (height == -1) ? handle_size / 2.0f : height;
  next_line_on_overflow((vec2s){ slider_width + margin_right + margin_left, handle_size + margin_bottom + margin_top }, state.div_props.border_width);
  state.pos_ptr.x += margin_left;
  state.pos_ptr.y += margin_top + handle_size / 4.0f;
  LfUIElementProps slider_props = props;
  slider_props.corner_radius = props. corner_radius / 2.0f;
  LfClickableItemState slider_state = button(file, line, state.pos_ptr, (vec2s){ (float)slider_width, (float)slider_height }, slider_props, props.color, 0, false, false);
  int32_t handle_pos = map_vals(val, min, max, handle_size / 2.0f, slider_width - handle_size / 2.0f) - (handle_size) / 2.0f;
  lf_push_element_id(1);
  LfClickableItemState handle = button(file, line, (vec2s){ state.pos_ptr.x + handle_pos, state.pos_ptr.y - (handle_size) / 2.0f + slider_height / 2.0f }, (vec2s){ handle_size, handle_size }, props, props.text_color, props.border_width, false, false);
  lf_pop_element_id();
  state.pos_ptr.x += slider_width + margin_right;
  state.pos_ptr.y -= margin_top + handle_size / 4.0f;
  return handle;
}

LfClickableItemState _lf_progress_bar_int_loc(float val, float min, float max, float width, float height, const char *file, int32_t line) {
  LfUIElementProps props = get_props_for(state.theme.slider_props);
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom = props.margin_bottom; 
  LfColor color = props.color;
  next_line_on_overflow((vec2s){ width + margin_right + margin_left, height + margin_bottom + margin_top }, state.div_props.border_width);
  state.pos_ptr.x += margin_left;
  state.pos_ptr.y += margin_top;
  LfClickableItemState bar = button(file, line, state.pos_ptr, (vec2s){ (float)width, (float)height },props, color, props.border_width, false, false);
  float pos_x = map_vals(val, min, max, 0, width);
  lf_push_element_id(1);
  LfClickableItemState handle = button(file, line, state.pos_ptr, (vec2s){ (float)pos_x, (float)height }, props, props.text_color, 0, false, false);
  lf_pop_element_id();
  state.pos_ptr.x += width + margin_right;
  state.pos_ptr.y -= margin_top;
  return bar;
}

LfClickableItemState _lf_progress_stripe_int_loc(LfSlider* slider, const char* file, int32_t line) {
  LfUIElementProps props = get_props_for(state.theme.slider_props);
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom =props.margin_bottom; 
  const float handle_size = 20;
  const float height = (slider->height != 0) ? slider->height : handle_size / 2.0f;
  LfColor color = props.color;
  next_line_on_overflow((vec2s){ slider->width + margin_right + margin_left, slider->height + margin_bottom + margin_top }, state.div_props.border_width);
  state.pos_ptr.x += margin_left;
  state.pos_ptr.y += margin_top;
  LfClickableItemState bar = button(file, line, state.pos_ptr, (vec2s){ (float)slider->width, (float)height },props, color, props.border_width, false, false);
  slider->handle_pos = map_vals(*(int32_t*)slider->val, slider->min, slider->max, 0, slider->width);
  lf_push_element_id(1);
  LfClickableItemState handle = button(file, line, state.pos_ptr, (vec2s){ (float)slider->handle_pos, (float)height }, props, props.text_color, 0, false, false);
  lf_pop_element_id();
  lf_rect_render((vec2s){ state.pos_ptr.x + slider->handle_pos, state.pos_ptr.y - (float)height / 2.0f }, (vec2s){ (float)slider->height * 2, (float)slider->height * 2 }, props.text_color, (LfColor){ 0.0f, 0.0f, 0.0f, 0.0f }, 0, props.corner_radius);
  state.pos_ptr.x += slider->width + margin_right;
  state.pos_ptr.y -= margin_top;
  return bar;
}

LfClickableItemState _lf_checkbox_loc(const char *text, bool *val, LfColor tick_color, LfColor tex_color, const char *file, int32_t line) { 
  return checkbox_element_loc((void *)text, val, tick_color, tex_color, file, line, false);
}

LfClickableItemState _lf_checkbox_wide_loc(const wchar_t *text, bool *val, LfColor tick_color, LfColor tex_color, const char *file, int32_t line) {
  return checkbox_element_loc((void *)text, val, tick_color, tex_color, file, line, true);
}

int32_t _lf_menu_item_list_loc(const char **items, uint32_t item_count, int32_t selected_index, LfMenuItemCallback per_cb, bool vertical, const char *file, int32_t line) {
  return menu_item_list_item_loc((void **)items, item_count, selected_index, per_cb, vertical, file, line, false);
}

int32_t _lf_menu_item_list_loc_wide(const wchar_t **items, uint32_t item_count, int32_t selected_index, LfMenuItemCallback per_cb, bool vertical, const char *file, int32_t line) {
  return menu_item_list_item_loc((void **)items, item_count, selected_index, per_cb, vertical, file, line, true);
}

void _lf_dropdown_menu_loc(const char **items, const char *placeholder, uint32_t item_count, float width, float height, int32_t *selected_index, bool *opened, const char *file, int32_t line) {
  return dropdown_menu_item_loc((void **)items, (void *)placeholder, item_count, width, height, selected_index, opened, file, line, false);
}

void _lf_dropdown_menu_loc_wide(const wchar_t **items, const wchar_t *placeholder, uint32_t item_count, float width, float height, int32_t *selected_index, bool *opened, const char *file, int32_t line) {
  return dropdown_menu_item_loc((void **)items, (void *)placeholder, item_count, width, height, selected_index, opened, file, line, true);
}

void _lf_input_text_loc(LfInputField *input, const char *file, int32_t line) {
  input_field(input, INPUT_TEXT, file, line);
}

void _lf_input_int_loc(LfInputField *input, const char *file, int32_t line) {
  input_field(input, INPUT_INT, file, line);
}
void _lf_input_float_loc(LfInputField *input, const char *file, int32_t line) {
  input_field(input, INPUT_FLOAT, file, line);
}
void lf_input_insert_char_idx(LfInputField *input, char c, uint32_t idx) {
  lf_input_field_unselect_all(input);
  insert_i_str(input->buf, c, idx); 
}

void lf_input_insert_str_idx(LfInputField *input, const char *insert, uint32_t len, uint32_t idx) {
  if(len > input->buf_size || strlen(input->buf) + len > input->buf_size) return;
  insert_str_str(input->buf, insert, idx);
  lf_input_field_unselect_all(input); 
}

void lf_input_field_unselect_all(LfInputField *input) {
  input->selection_start = -1;
  input->selection_end = -1;
  input->selection_dir = 0;
}

bool lf_input_grabbed() {
  return state.input_grabbed;
}

void lf_div_grab(LfDiv div) {
  state.grabbed_div = div;
}

void lf_div_ungrab() {
  memset(&state.grabbed_div, 0, sizeof(LfDiv));
  state.grabbed_div.id = -1;
}

bool lf_div_grabbed() {
  return state.grabbed_div.id != -1;
}

LfDiv lf_get_grabbed_div() {
  return state.grabbed_div;
}

void _lf_begin_loc(const char *file, int32_t line) {
  state.pos_ptr = (vec2s){ 0, 0 };
  renderer_begin();
  LfUIElementProps props = get_props_for(state.theme.div_props);
  props.color = (LfColor){ 0, 0, 0, 0 };
  lf_push_style_props(props);
  lf_div_begin(((vec2s){ 0, 0 }), ((vec2s){ (float)state.dsp_w, (float)state.dsp_h }), true);
  lf_pop_style_props();
}

void lf_end() {
  lf_div_end();
  state.selected_div = state.selected_div_tmp;
  update_input();
  clear_events();
  renderer_flush();
  state.drawcalls = 0;
}

void lf_next_line() {
  state.pos_ptr.x = state.current_div.aabb.pos.x + state.div_props.border_width;
  state.pos_ptr.y += state.current_line_height;
  state.current_line_height = 0;
}

vec2s lf_text_dimension(const char *str) {
  return lf_text_dimension_ex(str, -1);
}

vec2s lf_text_dimension_ex(const char *str, float wrap_point) {
  LfFont font = get_current_font();
  LfTextProps props = lf_text_render((vec2s){ 0.0f, 0.0f }, str, font, state.theme.text_props.text_color, wrap_point, (vec2s){ -1, -1 }, true, false, -1, -1);
  return (vec2s){ (float)props.width, (float)props.height };
}

vec2s lf_text_dimension_wide(const wchar_t *str) {
  return lf_text_dimension_wide_ex(str, -1);
}

vec2s lf_text_dimension_wide_ex(const wchar_t *str, float wrap_point) {
  LfFont font = get_current_font();
  LfTextProps props = lf_text_render_wchar((vec2s){ 0.0f, 0.0f }, str, font, LF_NO_COLOR, wrap_point, (vec2s){ -1, -1 }, true, false, -1, -1);
  return (vec2s){ (float)props.width, (float)props.height };
}

vec2s lf_button_dimension(const char *text) {
  LfUIElementProps props = get_props_for(state.theme.button_props);
  float padding = props.padding;
  vec2s text_dimension = lf_text_dimension(text);
  return (vec2s){ text_dimension.x + padding * 2.0f, text_dimension.y + padding };
}

float lf_get_text_end(const char *str, float start_x) {
  LfFont font = get_current_font();
  LfTextProps props = text_render_simple((vec2s){ start_x, 0.0f }, str, font, state.theme.text_props.text_color, true);
  return props.end_x;
}

void lf_text(const char *text) {
  LfUIElementProps props = get_props_for(state.theme.text_props);
  float padding = props.padding;
  float margin_left = props.margin_left, margin_right = props.margin_right, 
  margin_top = props.margin_top, margin_bottom = props.margin_bottom;
  LfColor text_color = props.text_color;
  LfColor color = props.color;
  LfFont font = get_current_font();
  LfTextProps text_props = lf_text_render(state.pos_ptr, text, font, text_color, state.text_wrap ?  (state.current_div.aabb.size.x + state.current_div.aabb.pos.x) - margin_right - margin_left : -1, (vec2s){ -1, -1 }, true, false, -1, -1);
  next_line_on_overflow((vec2s){ text_props.width + padding * 2.0f + margin_left + margin_right, text_props.height + padding * 2.0f + margin_top + margin_bottom }, state.div_props.border_width);
  state.pos_ptr.x += margin_left;
  state.pos_ptr.y += margin_top;
  lf_text_render((vec2s){ state.pos_ptr.x + padding, state.pos_ptr.y + padding }, text, font, text_color, state.text_wrap ?  (state.current_div.aabb.size.x + state.current_div.aabb.pos.x) - margin_right - margin_left : -1, (vec2s){ -1, -1 }, false, false, -1, -1);
  state.pos_ptr.x += text_props.width + margin_right + padding;
  state.pos_ptr.y -= margin_top;
}

void lf_text_wide(const wchar_t *text) {
  LfUIElementProps props = get_props_for(state.theme.text_props);
  float padding = props.padding;
  float margin_left = props.margin_left, margin_right = props.margin_right, 
  margin_top = props.margin_top, margin_bottom = props.margin_bottom;
  LfColor text_color = props.text_color;
  LfColor color = props.color;
  LfFont font = get_current_font();
  LfTextProps text_props = lf_text_render_wchar(state.pos_ptr, text, font, text_color, (state.text_wrap ? (state.current_div.aabb.size.x + state.current_div.aabb.pos.x) - margin_right - margin_left : -1), (vec2s){ -1, -1 }, true, false, -1, -1);
  next_line_on_overflow((vec2s){ text_props.width + padding * 2.0f + margin_left + margin_right, text_props.height + padding * 2.0f + margin_top + margin_bottom }, state.div_props.border_width);
  state.pos_ptr.x += margin_left;
  state.pos_ptr.y += margin_top;
  lf_rect_render(state.pos_ptr, (vec2s){ text_props.width + padding * 2.0f, text_props.height + padding * 2.0f }, props.color, props.border_color, props.border_width, props.corner_radius);
  lf_text_render_wchar((vec2s){ state.pos_ptr.x + padding, state.pos_ptr.y + padding }, text, font, text_color, (state.text_wrap ? (state.current_div.aabb.size.x + state.current_div.aabb.pos.x) - margin_right - margin_left : -1), (vec2s){ -1, -1 }, false, false, -1, -1);
  state.pos_ptr.x += text_props.width + padding * 2.0f + margin_right + padding;
  state.pos_ptr.y -= margin_top;
}

void lf_set_text_wrap(bool wrap) {
  state.text_wrap = wrap;
}

LfDiv lf_get_current_div() {
  return state.current_div;
}

LfDiv lf_get_selected_div() {
  return state.selected_div;
}

LfDiv *lf_get_current_div_ptr() {
  return &state.current_div;
}

LfDiv *lf_get_selected_div_ptr() {
  return &state.selected_div;
}

void lf_set_ptr_x(float x) {
  state.pos_ptr.x = x + state.current_div.aabb.pos.x;
}

void lf_set_ptr_y(float y) {
  state.pos_ptr.y = y + state.current_div.aabb.pos.y;
}

void lf_set_ptr_x_absolute(float x) {
  state.pos_ptr.x = x;
}

void lf_set_ptr_y_absolute(float y) {
  state.pos_ptr.y = y;
}

float lf_get_ptr_x() {
  return state.pos_ptr.x;
}

float lf_get_ptr_y() {
  return state.pos_ptr.y;
}

uint32_t lf_get_display_width() {
  return state.dsp_w;
}

uint32_t lf_get_display_height() {
  return state.dsp_h;
}

void lf_push_font(LfFont *font) {
  state.font_stack = font;
}

void lf_pop_font() {
  state.font_stack = NULL;
}

uint32_t decode_utf8(const char *s, int *bytes_read) {
  uint8_t c = s[0];
  if (c < 0x80) {
    *bytes_read = 1;
    return c;
  } 
  else if (c < 0xE0) {
    *bytes_read = 2;
    return ((s[0] & 0x1F) << 6) | (s[1] & 0x3F);
  } 
  else if (c < 0xF0) {
    *bytes_read = 3;
    return ((s[0] & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
  } 
  else {
    *bytes_read = 4;
    return ((s[0] & 0x07) << 18) | ((s[1] & 0x3F) << 12) | ((s[2] & 0x3F) << 6) | (s[3] & 0x3F);
  }
}

static wchar_t *str_to_wstr(const char *str) {
  size_t len = strlen(str) + 1;
  wchar_t *wstr = (wchar_t *)malloc(len * sizeof(wchar_t));
  if (wstr == NULL) {
    error_func("Memory allocation failed", user_defined_data);
    return NULL;
  }
  if (mbstowcs(wstr, str, len) == (size_t)-1) {
    error_func("Conversion failed", user_defined_data);
    free(wstr);
    return NULL;
  }
  return wstr;
}

LfTextProps lf_text_render(vec2s pos, const char *str, LfFont font, LfColor color, int32_t wrap_point, vec2s stop_point, bool no_render, bool render_solid, int32_t start_index, int32_t end_index) {
  wchar_t *wstr = str_to_wstr(str);
  LfTextProps textprops =  lf_text_render_wchar(pos, (const wchar_t *)wstr, font, color, wrap_point, stop_point, no_render, render_solid, start_index, end_index);
  free(wstr);
  return textprops;
}

void lf_rect_render(vec2s pos, vec2s size, LfColor color, LfColor border_color, float border_width, float corner_radius) {
  if(!state.renderer_render) return;
  if(item_should_cull((LfAABB){.pos = pos, .size = size})) {
    return;
  }
  vec2s pos_initial = pos;
  pos = (vec2s){pos.x + size.x / 2.0f, pos.y + size.y / 2.0f};
  vec2s texcoords[4] = {
    (vec2s){ 1.0f, 1.0f },
    (vec2s){ 1.0f, 0.0f },
    (vec2s){ 0.0f, 0.0f },
    (vec2s){ 0.0f, 1.0f },
  };
  mat4 translate; 
  mat4 scale;
  mat4 transform;
  vec3 pos_xyz = { (corner_radius != 0.0f ? (float)state.dsp_w / 2.0f : pos.x), (corner_radius != 0.0f ? (float)state.dsp_h / 2.0f : pos.y), 0.0f };
  vec3 size_xyz = { corner_radius != 0.0f ? state.dsp_w : size.x, corner_radius != 0.0f ? state.dsp_h : size.y, 0.0f };
  glm_translate_make(translate, pos_xyz);
  glm_scale_make(scale, size_xyz);
  glm_mat4_mul(translate,scale,transform);
  for(uint32_t i = 0; i < 4; i++) {
    if(state.render.vert_count >= MAX_RENDER_BATCH) {
      renderer_flush();
      renderer_begin();
    }
    vec4 result;
    glm_mat4_mulv(transform, state.render.vert_pos[i].raw, result);
    state.render.verts[state.render.vert_count].pos[0] = result[0];
    state.render.verts[state.render.vert_count].pos[1] = result[1];
    vec4s border_color_zto = lf_color_to_zto(border_color);
    const vec4 border_color_arr = {border_color_zto.r, border_color_zto.g, border_color_zto.b, border_color_zto.a};
    memcpy(state.render.verts[state.render.vert_count].border_color, border_color_arr, sizeof(vec4));
    state.render.verts[state.render.vert_count].border_width = border_width; 
    vec4s color_zto = lf_color_to_zto(color);
    const vec4 color_arr = { color_zto.r, color_zto.g, color_zto.b, color_zto.a };
    memcpy(state.render.verts[state.render.vert_count].color, color_arr, sizeof(vec4));
    const vec2 texcoord_arr = { texcoords[i].x, texcoords[i].y };
    memcpy(state.render.verts[state.render.vert_count].texcoord, texcoord_arr, sizeof(vec2));
    state.render.verts[state.render.vert_count].tex_index = -1;
    const vec2 scale_arr = { size.x, size.y };
    memcpy(state.render.verts[state.render.vert_count].scale, scale_arr, sizeof(vec2));
    const vec2 pos_px_arr = { (float)pos_initial.x, (float)pos_initial.y };
    memcpy(state.render.verts[state.render.vert_count].pos_px, pos_px_arr, sizeof(vec2));
    state.render.verts[state.render.vert_count].corner_radius = corner_radius;
    const vec2 cull_start_arr = { state.cull_start.x, state.cull_start.y };
    memcpy(state.render.verts[state.render.vert_count].min_coord, cull_start_arr, sizeof(vec2));
    const vec2 cull_end_arr = { state.cull_end.x, state.cull_end.y };
    memcpy(state.render.verts[state.render.vert_count].max_coord, cull_end_arr, sizeof(vec2));
    state.render.vert_count++;
  }
  state.render.index_count += 6;
}

void lf_image_render(vec2s pos, LfColor color, LfTexture tex, LfColor border_color, float border_width, float corner_radius) {
  if(!state.renderer_render) return;
  if(item_should_cull((LfAABB){ .pos = pos, .size = (vec2s){ tex.width, tex.height } })) {
    return;
  }
  if(state.render.tex_count - 1 >= MAX_TEX_COUNT_BATCH - 1) {
    renderer_flush();
    renderer_begin();
  }
  vec2s pos_initial = pos;
  pos = (vec2s){pos.x + tex.width / 2.0f, pos.y + tex.height / 2.0f};
  if(state.image_color_stack.a != 0.0) {
    color = state.image_color_stack;
  }
  vec2s texcoords[4] = {
    (vec2s){ 0.0f, 0.0f },
    (vec2s){ 1.0f, 0.0f },
    (vec2s){ 1.0f, 1.0f },
    (vec2s){ 0.0f, 1.0f },
  };
  float tex_index = -1.0f;
  for(uint32_t i = 0; i < state.render.tex_count; i++) {
    if(tex.id == state.render.textures[i].id) {
      tex_index = i;
      break;
    }
  }
  if(tex_index == -1.0f) {
    tex_index = (float)state.render.tex_index;
    state.render.textures[state.render.tex_count++] = tex;
    state.render.tex_index++;
  }
  mat4 translate = GLM_MAT4_IDENTITY_INIT; 
  mat4 scale = GLM_MAT4_IDENTITY_INIT;
  mat4 transform = GLM_MAT4_IDENTITY_INIT;
  vec3s pos_xyz = (vec3s){ pos.x, pos.y, 0.0f };
  vec3 tex_size;
  tex_size[0] = tex.width;
  tex_size[1] = tex.height;
  tex_size[2] = 0;
  glm_translate_make(translate, pos_xyz.raw);
  glm_scale_make(scale, tex_size);
  glm_mat4_mul(translate,scale,transform);
  for(uint32_t i = 0; i < 4; i++) {
    if(state.render.vert_count >= MAX_RENDER_BATCH) {
      renderer_flush();
      renderer_begin();
    }
    vec4 result;
    glm_mat4_mulv(transform, state.render.vert_pos[i].raw, result);
    memcpy(state.render.verts[state.render.vert_count].pos, result, sizeof(vec2));
    vec4s border_color_zto = lf_color_to_zto(border_color);
    const vec4 border_color_arr = { border_color_zto.r, border_color_zto.g, border_color_zto.b, border_color_zto.a };
    memcpy(state.render.verts[state.render.vert_count].border_color, border_color_arr, sizeof(vec4));
    state.render.verts[state.render.vert_count].border_width = border_width; 
    vec4s color_zto = lf_color_to_zto(color);
    const vec4 color_arr = { color_zto.r, color_zto.g, color_zto.b, color_zto.a };
    memcpy(state.render.verts[state.render.vert_count].color, color_arr, sizeof(vec4));
    const vec2 texcoord_arr = { texcoords[i].x, texcoords[i].y };
    memcpy(state.render.verts[state.render.vert_count].texcoord, texcoord_arr, sizeof(vec2));
    state.render.verts[state.render.vert_count].tex_index = tex_index;
    const vec2 scale_arr = { (float)tex.width, (float)tex.height };
    memcpy(state.render.verts[state.render.vert_count].scale, scale_arr, sizeof(vec2));
    vec2 pos_px_arr = { (float)pos_initial.x, (float)pos_initial.y };
    memcpy(state.render.verts[state.render.vert_count].pos_px, pos_px_arr, sizeof(vec2));
    state.render.verts[state.render.vert_count].corner_radius = corner_radius;
    const vec2 cull_start_arr = { state.cull_start.x, state.cull_start.y };
    memcpy(state.render.verts[state.render.vert_count].min_coord, cull_start_arr, sizeof(vec2));
    const vec2 cull_end_arr = { state.cull_end.x, state.cull_end.y };
    memcpy(state.render.verts[state.render.vert_count].max_coord, cull_end_arr, sizeof(vec2));
    state.render.vert_count++;
  } 
  state.render.index_count += 6;
}

bool lf_point_intersects_aabb(vec2s p, LfAABB aabb) {
  vec2s min = { aabb.pos.x, aabb.pos.y };
  vec2s max = { aabb.pos.x + aabb.size.x, aabb.pos.y + aabb.size.y };
  if (p.x >= min.x && p.x <= max.x && p.y >= min.y && p.y <= max.y) {
    return true;
  }
  return false;
}

bool lf_aabb_intersects_aabb(LfAABB a, LfAABB b) {
  vec2s minA = a.pos;
  vec2s maxA = (vec2s){ a.pos.x + a.size.x, a.pos.y + a.size.y };
  vec2s minB = b.pos;
  vec2s maxB = (vec2s){ b.pos.x + b.size.x, b.pos.y + b.size.y };
  return (minA.x >= minB.x && minA.x <= maxB.x && minA.y >= minB.y && minA.y <= maxB.y);
}

void lf_push_style_props(LfUIElementProps props) {
  props_stack_push(&state.props_stack, props);
}

void lf_pop_style_props() {
  props_stack_pop(&state.props_stack);
}

bool lf_hovered(vec2s pos, vec2s size) {
  bool hovered = lf_get_mouse_x() <= (pos.x + size.x) && lf_get_mouse_x() >= (pos.x) && lf_get_mouse_y() <= (pos.y + size.y) && lf_get_mouse_y() >= (pos.y) && ((state.selected_div.id == state.current_div.id && state.grabbed_div.id == -1) || (state.grabbed_div.id == state.current_div.id && state.grabbed_div.id != -1));
  return hovered;
}

bool lf_area_hovered(vec2s pos, vec2s size) {
  bool hovered = lf_get_mouse_x() <= (pos.x + size.x) && lf_get_mouse_x() >= (pos.x) && lf_get_mouse_y() <= (pos.y + size.y) && lf_get_mouse_y() >= (pos.y);
  return hovered;
}

LfCursorPosEvent lf_mouse_move_event() {
  return state.cp_ev;
}

LfMouseButtonEvent lf_mouse_button_event() {
  return state.mb_ev;
}

LfScrollEvent lf_mouse_scroll_event() {
  return state.scr_ev;
}

LfKeyEvent lf_key_event() {
  return state.key_ev;
}

LfCharEvent lf_char_event() { 
  return state.ch_ev;
}

void lf_set_cull_end_x(float x) {
  state.cull_end.x = x; 
}

void lf_set_cull_end_y(float y) {
  state.cull_end.y = y; 
}
void lf_set_cull_start_x(float x) {
  state.cull_start.x = x;
}

void lf_set_cull_start_y(float y) {
  state.cull_start.y = y;
}

void lf_unset_cull_start_x() {
  state.cull_start.x = -1;
}

void lf_unset_cull_start_y() {
  state.cull_start.y = -1;
}

void lf_unset_cull_end_x() {
  state.cull_end.x = -1;
}

void lf_unset_cull_end_y() {
  state.cull_end.y = -1;
}

void lf_set_image_color(LfColor color) {
  state.image_color_stack = color;
}

void lf_unset_image_color() {
  state.image_color_stack = LF_NO_COLOR;
}

void lf_set_current_div_scroll(float scroll) {
  *state.scroll_ptr = scroll;
}
float lf_get_current_div_scroll() {
  return *state.scroll_ptr;
}

void lf_set_current_div_scroll_velocity(float scroll_velocity) {
  *state.scroll_velocity_ptr = scroll_velocity;
}

float lf_get_current_div_scroll_velocity() {
  return *state.scroll_ptr;
}

void lf_set_line_height(uint32_t line_height) {
  state.current_line_height = line_height;
}

uint32_t lf_get_line_height() {
  return state.current_line_height;
}

void lf_set_line_should_overflow(bool overflow) {
  state.line_overflow = overflow;
}

void lf_set_div_hoverable(bool clickable) {
  state.div_hoverable = clickable;
}
void lf_push_element_id(int64_t id) {
  state.element_id_stack = id;
}

void lf_pop_element_id() {
  state.element_id_stack = -1;
}

LfColor lf_color_brightness(LfColor color, float brightness) {
  uint32_t adjustedR = (int)(color.r * brightness);
  uint32_t adjustedG = (int)(color.g * brightness);
  uint32_t adjustedB = (int)(color.b * brightness);
  color.r = (unsigned char)(adjustedR > 255 ? 255 : adjustedR);
  color.g = (unsigned char)(adjustedG > 255 ? 255 : adjustedG);
  color.b = (unsigned char)(adjustedB > 255 ? 255 : adjustedB);
  return color; 
}

LfColor lf_color_alpha(LfColor color, uint8_t a) {
  return (LfColor){color.r, color.g, color.b, a};
}

vec4s lf_color_to_zto(LfColor color) {
  return (vec4s){color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f};
}

LfColor lf_color_from_hex(uint32_t hex) {
  LfColor color;
  color.r = (hex>> 16) & 0xFF;
  color.g = (hex >> 8) & 0xFF; 
  color.b = hex& 0xFF; 
  color.a = 255; 
  return color;
}

LfColor lf_color_from_zto(vec4s zto) {
  return (LfColor){(uint8_t)(zto.r * 255.0f), (uint8_t)(zto.g * 255.0f), (uint8_t)(zto.b * 255.0f), (uint8_t)(zto.a * 255.0f)};
}

void lf_image(LfTexture tex) {
  LfUIElementProps props = get_props_for(state.theme.image_props);
  float margin_left = props.margin_left, margin_right = props.margin_right, 
  margin_top = props.margin_top, margin_bottom = props.margin_bottom;
  LfColor color = props.color;
  next_line_on_overflow((vec2s){ tex.width + margin_left + margin_right, tex.height + margin_top + margin_bottom }, state.div_props.border_width);
  state.pos_ptr.x += margin_left; 
  state.pos_ptr.y += margin_top;
  lf_image_render(state.pos_ptr, color, tex, props.border_color, props.border_width, props.corner_radius);
  state.pos_ptr.x += tex.width + margin_right;
  state.pos_ptr.y -= margin_top;
}

void lf_rect(float width, float height, LfColor color, float corner_radius) {
  next_line_on_overflow((vec2s){ (float)width, (float)height }, state.div_props.border_width);
  lf_rect_render(state.pos_ptr, (vec2s){ (float)width, (float)height }, color, (LfColor){ 0.0f, 0.0f, 0.0f, 0.0f }, 0, corner_radius);
  state.pos_ptr.x += width;
}

void lf_seperator() {
  lf_next_line();
  LfUIElementProps props = get_props_for(state.theme.button_props);
  state.pos_ptr.x += props.margin_left;
  state.pos_ptr.y += props.margin_top;
  const uint32_t seperator_height = 1;
  lf_set_line_height(props.margin_top + seperator_height + props.margin_bottom);
  lf_rect_render(state.pos_ptr, (vec2s){ state.current_div.aabb.size.x - props.margin_left * 2.0f, seperator_height }, props.color, LF_NO_COLOR, 0, props.corner_radius);
  state.pos_ptr.y -= props.margin_top;
  lf_next_line();
}

void lf_set_clipboard_text(const char *text) {
  clipboard_set_text(state.clipboard, text);
}
char *lf_get_clipboard_text() {
  return clipboard_text(state.clipboard);
}

void lf_set_no_render(bool no_render) {
  state.renderer_render = !no_render;
}
