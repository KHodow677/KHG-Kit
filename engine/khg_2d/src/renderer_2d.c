#include "khg_2d/renderer_2d.h"
#include "khg_2d/utils.h"
#include "khg_math/minmax.h"
#include "khg_math/math.h"
#include "khg_math/mat3.h"
#include "khg_utils/error_func.h"
#include "glad/glad.h"
#include "khg_utils/string.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

void internalFlush(renderer_2d *r2d, bool shouldClear) {
  const int size = vector_size(r2d->sprite_textures);
  unsigned int id = r2d->sprite_textures[0].id;
  enable_GL_necessary_features();
  if (!has_initialized) {
    error_func("Library not initialized. Have you forgotten to call gl2d::init() ?", user_defined_data);
    clear_draw_data(r2d);
    return;
  }
  if (!r2d->vao) {
    clear_draw_data(r2d);
    error_func("Renderer not initialized. Have you forgotten to call gl2d::Renderer2D::create() ?", user_defined_data);
    return;
  }
  if (r2d->window_h <= 0 || r2d->window_w <= 0) {
    if (shouldClear) {
      error_func("Negative windowW or windowH, have you forgotten to call updateWindowMetrics(w, h)?", user_defined_data);
      clear_draw_data(r2d);
    }
    return;
  }
  if (vector_empty(r2d->sprite_textures)) {
    return; 
  }
  glViewport(0, 0, r2d->window_w, r2d->window_h);
  glBindVertexArray(r2d->vao);
  glUseProgram(r2d->current_shader.id);
  glUniform1i(r2d->current_shader.u_sampler, 0);
  glBindBuffer(GL_ARRAY_BUFFER, r2d->buffers[quad_positions]);
  glBufferData(GL_ARRAY_BUFFER, vector_size(r2d->sprite_positions) * sizeof(vec2), r2d->sprite_positions, GL_STREAM_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, r2d->buffers[quad_colors]);
  glBufferData(GL_ARRAY_BUFFER, vector_size(r2d->sprite_colors) * sizeof(vec4), r2d->sprite_colors, GL_STREAM_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, r2d->buffers[texture_positions]);
  glBufferData(GL_ARRAY_BUFFER, vector_size(r2d->texture_positions) * sizeof(vec2), r2d->texture_positions, GL_STREAM_DRAW);
  bind_texture(&r2d->sprite_textures[0], 0);
  int pos = 0;
  for (int i = 1; i < size; i++) {
    if (r2d->sprite_textures[i].id != id) {
      glDrawArrays(GL_TRIANGLES, pos * 6, 6 * (i - pos));
      pos = i;
      id = r2d->sprite_textures[i].id;
      bind_texture(&r2d->sprite_textures[i], 0);
    }
  }
  glDrawArrays(GL_TRIANGLES, pos * 6, 6 * (size - pos));
  glBindVertexArray(0);
  if (shouldClear) {
    clear_draw_data(r2d);
  }
}

void renderQuadToScreenInternal(renderer_2d *r2d) {
  static float positions[12] = { -1, 1, -1, -1, 1, 1, 1, 1, -1, -1, 1, -1 };
  static float colors[6 * 4] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
  static float tex_coords[12] = { 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0 };
  glBindVertexArray(r2d->vao);
  glBindBuffer(GL_ARRAY_BUFFER, r2d->buffers[quad_positions]);
  glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(vec2), positions, GL_STREAM_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, r2d->buffers[quad_colors]);
  glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(vec4), colors, GL_STREAM_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, r2d->buffers[texture_positions]);
  glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(vec2), tex_coords, GL_STREAM_DRAW);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void renderPoint(renderer_2d *r2d, vec2 pos) {
  vec4 colors[4] = { color_black, color_black, color_black, color_black };
  render_rectangle(r2d, (vec4){ pos.x - 1.0f, pos.y - 1.0f, 2.0f, 2.0f }, colors, (vec2){ 0.0f, 0.0f }, 0);
}

vec2 calcPos(int p, float size, int segments, vec2 position) {
  vec2 circle = { size, 0.0f };
  float a = 3.1415926 * 2 * ((float)p / segments);
  float c = cos(a);
  float s = sin(a);
  circle = (vec2){ c * circle.x - s * circle.y, s * circle.x - c * circle.y };
  return vec2_add(&circle, &position);
}

void create_renderer_2d(renderer_2d *r2d, GLuint fbo, size_t quad_count) {
  if (!has_initialized) {
		error_func("Library not initialized. Have you forgotten to call gl2d::init() ?", user_defined_data);
  }
  r2d->default_fbo = fbo;
  r2d->sprite_positions = NULL;
  r2d->sprite_colors = NULL;
  r2d->texture_positions = NULL;
  r2d->sprite_textures = NULL;
  clear_draw_data(r2d);
  vector_reserve(r2d->sprite_positions, quad_count * 6);
  vector_reserve(r2d->sprite_colors, quad_count * 6);
  vector_reserve(r2d->texture_positions, quad_count * 6);
  vector_reserve(r2d->sprite_textures, quad_count);
  reset_shader_and_camera(r2d);
  glGenVertexArrays(1, &r2d->vao);
  glBindVertexArray(r2d->vao);
  glGenBuffers(buffer_size, r2d->buffers);
  glBindBuffer(GL_ARRAY_BUFFER, r2d->buffers[quad_positions]);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glBindBuffer(GL_ARRAY_BUFFER, r2d->buffers[quad_colors]);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glBindBuffer(GL_ARRAY_BUFFER, r2d->buffers[texture_positions]);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glBindVertexArray(0);
}

void cleanup_renderer_2d(renderer_2d *r2d) {
  glDeleteVertexArrays(1, &r2d->vao);
  glDeleteBuffers(buffer_size, r2d->buffers);
  cleanup_framebuffer(&r2d->post_process_fbo_1);
  cleanup_framebuffer(&r2d->post_process_fbo_2);
  r2d->internal_post_process_flip = 0;
  vector_free(r2d->sprite_positions);
  vector_free(r2d->sprite_colors);
  vector_free(r2d->texture_positions);
  vector_free(r2d->sprite_textures);
}

void push_shader(renderer_2d *r2d, shader s) {
  vector_push_back(r2d->shader_push_pop, s);
  r2d->current_shader = s;
}

void pop_shader(renderer_2d *r2d) {
  if (vector_empty(r2d->shader_push_pop)) {
    error_func("Pop on an empty stack on popShader", user_defined_data);
  }
  else {
    r2d->current_shader = *vector_back(r2d->shader_push_pop);
    vector_pop_back(r2d->shader_push_pop);
  }
}

void push_camera(renderer_2d *r2d, camera c) {
  vector_push_back(r2d->camera_push_pop, c);
  r2d->current_camera = c;
}

void pop_camera(renderer_2d *r2d) {
  if (vector_empty(r2d->camera_push_pop)) {
    error_func("Pop on an empty stack on popCamera", user_defined_data);
  }
  else {
    r2d->current_camera = *vector_back(r2d->camera_push_pop);
    vector_pop_back(r2d->camera_push_pop);
  }
}

vec4 get_view_rect(renderer_2d *r2d) {
  vec4 rect = { 0.0f, 0.0f, r2d->window_w, r2d->window_h };
  mat3 mat;
  mat.values[0] = 1.0f;
  mat.values[1] = 0.0f;
  mat.values[2] = r2d->current_camera.position.x;
  mat.values[3] = 0.0f;
  mat.values[4] = 1.0f;
  mat.values[5] = r2d->current_camera.position.y;
  mat.values[6] = 0.0f;
  mat.values[7] = 0.0f;
  mat.values[8] = 1.0f;
  mat = mat3_transpose(&mat);
  vec3 pos_1 = { rect.x, rect.y, 1.0f };
  vec3 pos_2 = { rect.z + rect.x, rect.w + rect.y, 1.0f };
  pos_1 = mat3_transform_vec3(&mat, &pos_1);
  pos_2 = mat3_transform_vec3(&mat, &pos_2);
  vec2 point = { (pos_1.x + pos_2.y) / 2.0f, (pos_1.y + pos_2.y) / 2.0f };
  vec2 scale_point_1 = scale_around_point((vec2){ pos_1.x, pos_1.y }, point, 1.0f / r2d->current_camera.zoom);
  vec2 scale_point_2 = scale_around_point((vec2){ pos_2.x, pos_2.y }, point, 1.0f / r2d->current_camera.zoom);
  pos_1 = (vec3){ scale_point_1.x, scale_point_1.y, 1.0f };
  pos_2 = (vec3){ scale_point_2.x, scale_point_2.y, 1.0f };
  rect = (vec4){ pos_1.x, pos_1.y, pos_2.x - pos_1.x, pos_2.y - pos_1.y };
  return rect;
}

void update_window_metrics(renderer_2d *r2d, int w, int h) {
  r2d->window_w = w;
  r2d->window_h = h;
}

vec4 pix_to_screen(renderer_2d *r2d, const vec4 *transform) {
  const float transform_y = transform->y * -1.0f;
  vec2 v1 = { transform->x, transform_y };
  vec2 v3 = { transform->x + transform->z, transform_y - transform->w };
  v1.x -= r2d->current_camera.position.x;
  v1.y += r2d->current_camera.position.y;
  v3.x -= r2d->current_camera.position.x;
  v3.y += r2d->current_camera.position.y;
  vec2 camera_center = { r2d->window_w / 2.0f, r2d->window_h / 2.0f };
  v1 = scale_around_point(v1, camera_center, r2d->current_camera.zoom);
  v3 = scale_around_point(v3, camera_center, r2d->current_camera.zoom);
  v1.x = position_to_screen_coords_x(v1.x, r2d->window_w);
  v3.x = position_to_screen_coords_x(v3.x, r2d->window_w);
  v1.y = position_to_screen_coords_y(v1.y, r2d->window_h);
  v3.y = position_to_screen_coords_y(v3.y, r2d->window_h);
  vec4 result = { v1.x, v1.y, v3.x, v3.y };
  return result;
}

void clear_draw_data(renderer_2d *r2d) {
  vector_clear(r2d->sprite_positions);
  vector_clear(r2d->sprite_colors);
  vector_clear(r2d->texture_positions);
  vector_clear(r2d->sprite_textures);
}

vec2 get_text_size(renderer_2d *r2d, const char *text, const font font, const float size, const float spacing, const float line_space) {
  if (font.texture.id == 0) {
    error_func("Missing font", user_defined_data);
    return (vec2){ 0.0f, 0.0f };
  }
  vec2 position = { 0.0f, 0.0f };
  const int text_length = (int)strlen(text);
  vec4 rectangle;
  rectangle.x = position.x;
  float line_position_y = position.y, maxPos = 0, maxPosY = 0, bonusY = 0;
  for (int i = 0; i< text_length; i++) {
    if (text[i] == '\n') {
      rectangle.x = position.x;
      line_position_y += (font.max_height + line_space) * size;
      bonusY += (font.max_height + line_space) * size;
      maxPosY = 0;
    }
    else if (text[i] == '\t') {
      const stbtt_aligned_quad quad = font_get_glyph_quad(font, '_');
      float x = quad.x1 - quad.x0;
      rectangle.x += x * size * 3 + spacing * size;
    }
    else if (text[i] == ' ') {
      const stbtt_aligned_quad quad = font_get_glyph_quad(font, ' ');
      float x = quad.x1 - quad.x0;
      rectangle.x += x * size + spacing * size;
    }
    else if (text[i] >= ' ' && text[i] <= '~') {
      const stbtt_aligned_quad quad = font_get_glyph_quad(font, text[i]);
      rectangle.z = quad.x1 - quad.x0;
      rectangle.w = quad.y1 - quad.y0;
      rectangle.z *= size;
      rectangle.w *= size;
      rectangle.y = line_position_y + quad.y0 * size;
      rectangle.x += rectangle.z + spacing * size;
      maxPosY = max(maxPosY, rectangle.y);
      maxPos = max(maxPos, rectangle.x);
    }
  }
  maxPos = max(maxPos, rectangle.x);
  maxPos = max(maxPosY, rectangle.y);
  float padd_x = maxPos;
  float padd_y = maxPosY;
  padd_y += font.max_height * size + bonusY;
  vec2 result = { padd_x, padd_y };
  return result;
}

void render_text(renderer_2d *r2d, vec2 position, const char *text, const font f, const vec4 color, const float size, const float spacing, const float line_space, bool show_in_center, const vec4 shadow_color, const vec4 light_color) {
  if (f.texture.id == 0) {
    error_func("Missing font", user_defined_data);
    return;
  } 
  const int text_length = (int)strlen(text);
  vec4 rectangle = { 0.0f, 0.0f, 0.0f, 0.0f };
  if (show_in_center) {
    vec2 text_size = get_text_size(r2d, text, f, size, spacing, line_space);
    position.x -= text_size.x / 2.0f;
    position.y -= text_size.y / 2.0f;
  }
  rectangle.x = position.x;
  float line_position_y = position.y;
  for (int i = 0; i < text_length; i++) {
    if (text[i] == '\n') {
      rectangle.x = position.x;
      line_position_y += (f.max_height + line_space) * size;
    }
    else if (text[i] == '\t') {
      const stbtt_aligned_quad quad = font_get_glyph_quad(f, '_');
      float x = quad.x1 - quad.x0;
      rectangle.x += x * size * 3 + spacing * size;
    }
    else if (text[i] == ' ') {
      const stbtt_aligned_quad quad = font_get_glyph_quad(f, '_');
      float x = quad.x1 - quad.x0;
      rectangle.x += x * size + spacing * size;
    }
    else if (text[i] >= ' ' && text[i] <= '~') {
      const stbtt_aligned_quad quad = font_get_glyph_quad(f, text[i]);
      vec4 color_data[4] = { color, color, color, color };
      vec4 texture_coords = { quad.s0, quad.t0, quad.s1, quad.t1 };
      vec2 origin = { 0.0f, 0.0f };
      rectangle.z = quad.x1 - quad.x0;
      rectangle.w = quad.y1 - quad.y0;
      rectangle.z *= size;
      rectangle.y = line_position_y + quad.y0 * size;
      if (shadow_color.w) {
        vec2 pos = { -5.0f, 3.0f };
        vec4 shadow_color_data[4] = { shadow_color, shadow_color, shadow_color, shadow_color };
        pos = vec2_multiply_num_on_vec2(size, &pos);
        vec4 transform = { rectangle.x + pos.x, rectangle.y + pos.y, rectangle.z, rectangle.w };
        render_rectangle_texture(r2d, transform, f.texture, shadow_color_data, origin, 0, texture_coords);
      }
      render_rectangle_texture(r2d, rectangle, f.texture, color_data, origin, 0, texture_coords);
      if (light_color.w) {
        vec2 pos = { -2.0f, 1.0f };
        vec4 light_color_data[4] = { light_color, light_color, light_color, light_color };
        pos = vec2_multiply_num_on_vec2(size, &pos);
        vec4 transform = { rectangle.x + pos.x, rectangle.y + pos.y, rectangle.z, rectangle.w };
        render_rectangle_texture(r2d, transform, f.texture, light_color_data, origin, 0, texture_coords);
      }
      rectangle.x += rectangle.z + spacing * size;
    }
  }
}

float determine_text_rescale_fit_smaller(renderer_2d *r2d, const string *str, font *f, vec4 transform, float max_size) {
  vec2 s = get_text_size(r2d, *str, *f, max_size, 4, 3);
  float ratio_x = transform.z / s.x;
  float ratio_y = transform.w / s.y;
  if (ratio_x > 1 && ratio_y > 1) {
    return max_size;
  }
  else {
    if (ratio_x < ratio_y) {
      return max_size * ratio_x;
    }
    else {
      return max_size * ratio_y;
    }
  }
}

float determine_text_rescale_fit_bigger(renderer_2d *r2d, const string *str, font *f, vec4 transform, float min_size) {
  vec2 s = get_text_size(r2d, *str, *f, min_size, 4, 3);
  float ratio_x = transform.z / s.x;
  float ratio_y = transform.w / s.y;
  if (ratio_x > 1 && ratio_y > 1) {
    if (ratio_x > ratio_y) {
      return min_size * ratio_y;
    }
    else {
      return min_size * ratio_x;
    }
  }
  return min_size; 
}

float determine_text_rescale_fit(renderer_2d *r2d, const string *str, font *f, vec4 transform) {
  float ret = 1.0f;
  vec2 s = get_text_size(r2d, *str, *f, ret, 4, 3);
  float ratio_x = transform.z / s.x;
  float ratio_y = transform.w / s.y;
  if (ratio_x > 1 && ratio_y > 1) {
    if (ratio_x > ratio_y) {
      return ret * ratio_y;
    }
    else {
      return ret * ratio_x;
    }
  }
  else {
    if (ratio_x < ratio_y) {
      return ret * ratio_x;
    }
    else {
      return ret * ratio_y;
    }
  }
  return ret;
}

int wrap(renderer_2d *r2d, const string *in, font *f, float base_size, float max_dimension, string *out_res) {
  if (out_res) {
    *out_res = "";
    str_reserve(*out_res, str_size(*in) + 10);
  }
  string word = str_create();
  string current_line = str_create();
  bool wrap = 0;
  bool new_line = 1;
  int new_line_counter = 0;
  str_reserve(current_line, str_size(*in) + 10);
  for (int i = 0; i < str_size(*in); i++) {
    str_add(word, in[i]);
    str_add(current_line, in[i]);
    if (*in[i] == ' ') {
      if (wrap) {
        if (out_res) {
          str_add(*out_res, "\n");
          str_clear(current_line);
        }
        new_line_counter++;
      }
      if (out_res) {
        str_add(*out_res, word);
      }
      str_clear(word);
      wrap = 0;
      new_line = true;
    }
    else if (*in[i] == '\n') {
      if (wrap) {
        if (out_res) {
          str_add(*out_res, "\n");
        }
      }
      str_clear(current_line);
      if (out_res) {
        str_add(*out_res, word);
      }
      str_clear(word);
      wrap = 0;
      new_line = true;
    }
    else {
      if (!wrap && !new_line) {
        float size = base_size;
        vec2 text_size = get_text_size(r2d, current_line, *f, size, 4, 3);
        if (text_size.x >= max_dimension && !new_line) {
          wrap = 1;
        }
      }
    }
  }
  if (wrap) {
    if (out_res) {
      str_add(*out_res, "\n");
      new_line_counter++;
    }
  }
  if (out_res) {
    str_add(*out_res, word);
  }
  str_free(word);
  str_free(current_line);
  return new_line_counter + 1;
}

void render_text_wrapped(renderer_2d *r2d, const string *text, font f, vec4 text_pos, vec4 color, float base_size, float spacing, float line_spacing, bool show_in_center, vec4 shadow_color, vec4 light_color) {
  string new_text = str_create();
  vec2 text_position = { text_pos.x, text_pos.y };
  wrap(r2d, text, &f, base_size, text_pos.z, &new_text);
  render_text(r2d, text_position, new_text, f, color, base_size, spacing, line_spacing, show_in_center, shadow_color, light_color);
  str_free(new_text);
}

vec2 get_text_size_wrapped(renderer_2d *r2d, const string *text, font f, float max_text_length, float base_size, float spacing, float line_spacing) {
  string new_text = str_create();
  wrap(r2d, text, &f, base_size, max_text_length, &new_text);
  vec2 rez = get_text_size(r2d, new_text, f, base_size, spacing, line_spacing);
  str_free(new_text);
  return rez;
}

void render_rectangle_texture(renderer_2d *r2d, const vec4 transform, const texture t, const vec4 colors[4], const vec2 origin, const float rotation_degrees, const vec4 texture_coords) {
  vec2 new_origin = { origin.x + transform.x + (transform.z / 2), origin.y + transform.y + (transform.w / 2) };
  render_rectangle_texture_abs_rotation(r2d, transform, t, colors, new_origin, rotation_degrees, texture_coords);
}

void render_rectangle_texture_abs_rotation(renderer_2d *r2d, const vec4 transform, const texture t, const vec4 colors[4], const vec2 origin, const float rotation_degrees, const vec4 texture_coords) {
  texture texture_copy = t;
  if (texture_copy.id == 0) {
    error_func("Invalid texture", user_defined_data);
    texture_copy = white_1_px_square_texture;
  }
  const float transform_y = transform.y * -1.0f;
  vec2 v1 = { transform.x, transform_y };
  vec2 v2 = { transform.x, transform_y - transform.w };
  vec2 v3 = { transform.x + transform.z, transform_y - transform.w };
  vec2 v4 = { transform.x + transform.z, transform_y };
  if (rotation_degrees != 0) {
    v1 = rotate_around_point(v1, origin, rotation_degrees);
    v2 = rotate_around_point(v2, origin, rotation_degrees);
    v3 = rotate_around_point(v3, origin, rotation_degrees);
    v4 = rotate_around_point(v4, origin, rotation_degrees);
  }
  v1.x -= r2d->current_camera.position.x;
  v1.y += r2d->current_camera.position.y;
  v2.x -= r2d->current_camera.position.x;
  v2.y += r2d->current_camera.position.y;
  v3.x -= r2d->current_camera.position.x;
  v3.y += r2d->current_camera.position.y;
  v4.x -= r2d->current_camera.position.x;
  v4.y += r2d->current_camera.position.y;
  if (r2d->current_camera.rotation != 0) {
    vec2 camera_center;
    camera_center.x = r2d->window_w / 2.0f;
    camera_center.y = r2d->window_h / 2.0f;
    v1 = rotate_around_point(v1, camera_center, r2d->current_camera.rotation);
    v2 = rotate_around_point(v2, camera_center, r2d->current_camera.rotation);
    v3 = rotate_around_point(v3, camera_center, r2d->current_camera.rotation);
    v4 = rotate_around_point(v4, camera_center, r2d->current_camera.rotation);
  }
  vec2 camera_center = { r2d->window_w / 2.0f, -r2d->window_h / 2.0f };
  v1 = scale_around_point(v1, camera_center, r2d->current_camera.zoom);
  v2 = scale_around_point(v2, camera_center, r2d->current_camera.zoom);
  v3 = scale_around_point(v3, camera_center, r2d->current_camera.zoom);
  v4 = scale_around_point(v4, camera_center, r2d->current_camera.zoom);
  v1.x = position_to_screen_coords_x(v1.x, (float)r2d->window_w);
  v2.x = position_to_screen_coords_x(v2.x, (float)r2d->window_w);
  v3.x = position_to_screen_coords_x(v3.x, (float)r2d->window_w);
  v4.x = position_to_screen_coords_x(v4.x, (float)r2d->window_w);
  v1.y = position_to_screen_coords_y(v1.y, (float)r2d->window_h);
  v2.y = position_to_screen_coords_y(v2.y, (float)r2d->window_h);
  v3.y = position_to_screen_coords_y(v3.y, (float)r2d->window_h);
  v4.y = position_to_screen_coords_y(v4.y, (float)r2d->window_h);
  vector_push_back(r2d->sprite_positions, v1);
  vector_push_back(r2d->sprite_positions, v2);
  vector_push_back(r2d->sprite_positions, v4);
  vector_push_back(r2d->sprite_positions, v2);
  vector_push_back(r2d->sprite_positions, v3);
  vector_push_back(r2d->sprite_positions, v4);
  vector_push_back(r2d->sprite_colors, colors[0]);
  vector_push_back(r2d->sprite_colors, colors[1]);
  vector_push_back(r2d->sprite_colors, colors[3]);
  vector_push_back(r2d->sprite_colors, colors[1]);
  vector_push_back(r2d->sprite_colors, colors[2]);
  vector_push_back(r2d->sprite_colors, colors[3]);
  vec2 t_pos = { texture_coords.x, texture_coords.y };
  vector_push_back(r2d->texture_positions, t_pos);
  t_pos = (vec2){ texture_coords.x, texture_coords.w };
  vector_push_back(r2d->texture_positions, t_pos);
  t_pos = (vec2){ texture_coords.z, texture_coords.y };
  vector_push_back(r2d->texture_positions, t_pos);
  t_pos = (vec2){ texture_coords.x, texture_coords.w };
  vector_push_back(r2d->texture_positions, t_pos);
  t_pos = (vec2){ texture_coords.z, texture_coords.w };
  vector_push_back(r2d->texture_positions, t_pos);
  t_pos = (vec2){ texture_coords.z, texture_coords.y };
  vector_push_back(r2d->texture_positions, t_pos);
  vector_push_back(r2d->sprite_textures, texture_copy);
}

void render_rectangle(renderer_2d *r2d, const vec4 transform, const vec4 colors[4], const vec2 origin, const float rotation_degrees) {
  render_rectangle_texture(r2d, transform, white_1_px_square_texture, colors, origin, rotation_degrees, default_texture_coords);
}

void render_rectangle_abs_rotation(renderer_2d *r2d, const vec4 transform, const vec4 colors[4], const vec2 origin, const float rotation_degrees) {
  render_rectangle_texture_abs_rotation(r2d, transform, white_1_px_square_texture, colors, origin, rotation_degrees, default_texture_coords);
}

void render_line_angle(renderer_2d *r2d, vec2 position, const float angle_degrees, const float length, const vec4 color, const float width) {
  vec4 colors[4] = { color, color, color, color };
  vec2 half_width = { 0.0f, width / 2.0f };
  vec2 difference = vec2_subtract(&position, &half_width);
  render_rectangle(r2d, (vec4){ difference.x, difference.y, length, width }, colors, (vec2){ -length / 2.0f, 0.0f }, angle_degrees);
}

void render_line_start_end(renderer_2d *r2d, vec2 start, vec2 end, const vec4 color, const float width) {
  vec2 distance = vec2_subtract(&end, &start);
  float length = sqrt(pow(distance.x, 2) + pow(distance.y, 2));
  float angle = atan2(distance.y, distance.x);
  render_line_angle(r2d, start, -degrees(angle), length, color, width);
}

void render_rectangle_outline(renderer_2d *r2d, const vec4 position, const vec4 color, const float width, const vec2 origin, const float rotation_degrees) {
  vec2 top_left = { position.x, position.y };
  vec2 top_right = { position.x + position.z, position.y };
  vec2 bottom_right = { position.x, position.y + position.w };
  vec2 bottom_left = { position.x + position.z, position.y + position.w };
  vec2 p1 = { top_left.x - (width / 2.0f), top_left.y };
  vec2 p2 = { top_right.x + (width / 2.0f), top_right.y };
  vec2 p3 = { top_right.x, top_right.y + (width / 2.0f) };
  vec2 p4 = { bottom_right.x, bottom_right.y - (width / 2.0f) };
  vec2 p5 = { bottom_right.x + (width / 2.0f), bottom_right.y };
  vec2 p6 = { bottom_left.x - (width / 2.0f), bottom_left.y };
  vec2 p7 = { bottom_left.x, bottom_left.y - (width / 2.0f) };
  vec2 p8 = { top_left.x, top_left.y + (width / 2.0f) };
  if (rotation_degrees != 0) {
    vec2 o = { origin.x + position.x + position.z / 2.0f, origin.y - position.y - position.w / 2.0f };
    p1 = rotate_around_point(p1, o, -rotation_degrees);
    p2 = rotate_around_point(p2, o, -rotation_degrees);
    p3 = rotate_around_point(p3, o, -rotation_degrees);
    p4 = rotate_around_point(p4, o, -rotation_degrees);
    p5 = rotate_around_point(p5, o, -rotation_degrees);
    p6 = rotate_around_point(p6, o, -rotation_degrees);
    p7 = rotate_around_point(p7, o, -rotation_degrees);
    p8 = rotate_around_point(p8, o, -rotation_degrees);
  }
  renderPoint(r2d, p1);
  renderPoint(r2d, p2);
  renderPoint(r2d, p3);
  renderPoint(r2d, p4);
  renderPoint(r2d, p5);
  renderPoint(r2d, p6);
  renderPoint(r2d, p7);
  renderPoint(r2d, p8);
  render_line_start_end(r2d, p1, p2, color, width);
  render_line_start_end(r2d, p3, p4, color, width);
  render_line_start_end(r2d, p5, p6, color, width);
  render_line_start_end(r2d, p7, p8, color, width);
}

void render_circle_outline(renderer_2d *r2d, const vec2 position, const vec4 color, const float size, const float width, const unsigned int segments) {
  vec2 last_pos = calcPos(1, size, segments, position);
  render_line_start_end(r2d, calcPos(0, size, segments, position), last_pos, color, width);
  for (int i = 1; i < segments; i++) {
    vec2 pos_1 = last_pos;
    vec2 pos_2 = calcPos(i + 1, size, segments, position);
    render_line_start_end(r2d, pos_1, pos_2, color, width);
    last_pos = pos_2;
  }
}

void render_nine_patch(renderer_2d *r2d, const vec4 position, const vec4 color, const vec2 origin, const float rotation_degrees, const texture texture, const vec4 texture_coords, const vec4 inner_texture_coords) {
  vec4 color_data[4] = { color, color, color, color };
  int w = 0;
  int h = 0;
  float texture_space_w = texture_coords.z - texture_coords.x;
  float texture_space_h = texture_coords.y - texture_coords.w;
  float top_border = (texture_coords.y - inner_texture_coords.y) / texture_space_h * position.w;
  float bottom_border = (inner_texture_coords.w - texture_coords.w) / texture_space_h * position.w;
  float left_border = (inner_texture_coords.x - texture_coords.x) / texture_space_w * position.z;
  float right_border = (texture_coords.z - inner_texture_coords.z) / texture_space_w * position.z;
  float new_aspect_ratio = position.z / position.w;
  vec2 pos_2d = { 0.0f, 0.0f };
  vec4 inner_pos = position, top_pos = position;
  vec4 bottom = position, left = position, right = position;
  vec4 topleft = position, topright = position, bottomleft = position, bottomright = position;
  glBindTexture(GL_TEXTURE_2D, texture.id);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
  if (new_aspect_ratio < 1.f) {
    top_border *= new_aspect_ratio;
    bottom_border *= new_aspect_ratio;
  }
  else {
    left_border /= new_aspect_ratio;
  }
  inner_pos.x += left_border;
  inner_pos.y += top_border;
  inner_pos.z -= left_border + right_border;
  inner_pos.w -= top_border + bottom_border;
  render_rectangle_texture(r2d, inner_pos, texture, color_data, pos_2d, 0, inner_texture_coords);
  top_pos.x += left_border;
  top_pos.z -= left_border + right_border;
  top_pos.w = top_border;
  vec4 upper_tex_pos = { inner_texture_coords.x, texture_coords.y, inner_texture_coords.z, inner_texture_coords.y };
  render_rectangle_texture(r2d, top_pos, texture, color_data, pos_2d, 0, upper_tex_pos);
  bottom.x += left_border;
  bottom.y += (float)position.w - bottom_border;
  bottom.z -= left_border + right_border;
  bottom.w = bottom_border;
  vec4 bottom_tex_pos = { inner_texture_coords.x, inner_texture_coords.w, inner_texture_coords.z, texture_coords.w };
  render_rectangle_texture(r2d, bottom, texture, color_data, pos_2d, 0, bottom_tex_pos);
  left.y += top_border;
  left.z = left_border;
  left.w -= top_border + bottom_border;
  vec4 left_tex_pos = { texture_coords.x, inner_texture_coords.y, inner_texture_coords.x, inner_texture_coords.w };
  render_rectangle_texture(r2d, left, texture, color_data, pos_2d, 0, left_tex_pos);
  right.x += position.z - right_border;
  right.y += top_border;
  right.z = right_border;
  right.w -= top_border + bottom_border;
  vec4 right_tex_pos = { inner_texture_coords.z, inner_texture_coords.y, texture_coords.z, inner_texture_coords.w };
  render_rectangle_texture(r2d, right, texture, color_data, pos_2d, 0, right_tex_pos);
  topleft.z = left_border;
  topleft.w = top_border;
  vec4 topleft_tex_pos = { texture_coords.x, texture_coords.y, inner_texture_coords.x, inner_texture_coords.y };
  render_rectangle_texture(r2d, topleft, texture, color_data, pos_2d, 0, topleft_tex_pos);
  topright.x += position.z - right_border;
  topright.z = right_border;
  topright.w = top_border;
  vec4 topright_tex_pos = { inner_texture_coords.z, texture_coords.y, texture_coords.z, inner_texture_coords.y };
  render_rectangle_texture(r2d, topright, texture, color_data, pos_2d, 0, topright_tex_pos);
  bottomleft.y += position.w - bottom_border;
  bottomleft.z = left_border;
  bottomleft.w = bottom_border;
  vec4 bottomleft_tex_pos = { texture_coords.x, inner_texture_coords.w, inner_texture_coords.x, texture_coords.w };
  render_rectangle_texture(r2d, bottomleft, texture, color_data, pos_2d, 0, bottomleft_tex_pos);
  bottomright.y += position.w - bottom_border;
  bottomright.x += position.z - right_border;
  bottomright.z = right_border;
  bottomright.w = bottom_border;
  vec4 bottomright_tex_pos = { inner_texture_coords.z, inner_texture_coords.w, texture_coords.z, texture_coords.w };
  render_rectangle_texture(r2d, bottomright, texture, color_data, pos_2d, 0, bottomright_tex_pos);
}

void clear_screen(renderer_2d *r2d, const vec4 color) {
  glBindFramebuffer(GL_FRAMEBUFFER, r2d->default_fbo);
  glClearBufferfv(GL_COLOR, 0, &color.x);
}

void set_shader(renderer_2d *r2d, const shader s) {
  r2d->current_shader = s;
}

void set_camera(renderer_2d *r2d, const camera c) {
  r2d->current_camera = c;
}

void reset_shader_and_camera(renderer_2d *r2d) {
  r2d->current_camera = default_camera;
  r2d->current_shader = default_shader;
}

void render_post_process(renderer_2d *r2d, shader shader, texture input, framebuffer result) {
  glBindFramebuffer(GL_FRAMEBUFFER, result.fbo);
  enable_GL_necessary_features();
  if (!has_initialized) {
    error_func("Library not initialized. Have you forgotten to call gl2d::init() ?", user_defined_data);
    return;
  }
  if (!r2d->vao) {
    error_func("Renderer not initialized. Have you forgotten to call gl2d::Renderer2D::create() ?", user_defined_data);
    return;
  }
  if (!shader.id) {
    error_func("Post Process Shader not created.", user_defined_data);
    return;
  }
  vec2 size = get_texture_size(&input);
  if (size.x == 0 || size.y == 0) {
    return;
  }
  glViewport(0, 0, size.x, size.y);
  glUseProgram(shader.id);
  glUniform1i(shader.u_sampler, 0);
  bind_texture(&input, 0);
  renderQuadToScreenInternal(r2d);
  glBindVertexArray(0);
}

void flush(renderer_2d *r2d, bool should_clear) {
  glBindFramebuffer(GL_FRAMEBUFFER, r2d->default_fbo);
  internalFlush(r2d, should_clear);
}

void flush_fbo(renderer_2d *r2d, framebuffer fb, bool should_clear) {
  if (fb.fbo == 0) {
    error_func("Framebuffer not initialized", user_defined_data);
    return;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, fb.fbo);
  glBindTexture(GL_TEXTURE_2D, 0);
  internalFlush(r2d, should_clear);
  glBindFramebuffer(GL_FRAMEBUFFER, r2d->default_fbo);
}

void render_framebuffer_to_entire_screen(renderer_2d *r2d, framebuffer fb, framebuffer screen) {
  render_texture_to_entire_screen(r2d, fb.texture, screen);
}

void render_texture_to_entire_screen(renderer_2d *r2d, texture t, framebuffer screen) {
  vec2 size = { r2d->window_w, r2d->window_h };
  glBindFramebuffer(GL_FRAMEBUFFER, screen.fbo);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  if (!has_initialized) {
    error_func("Library not initialized. Have you forgotten to call gl2d::init() ?", user_defined_data);
    return;
  }
  if (!r2d->vao) {
    error_func("Renderer not initialized. Have you forgotten to call gl2d::Renderer2D::create() ?", user_defined_data);
    return;
  }
  if (!r2d->current_shader.id) {
    error_func("Post Process Shader not created.", user_defined_data);
    return;
  }
  if (screen.fbo) {
    size = get_texture_size(&screen.texture);
  }
  if (size.x == 0 || size.y == 0) {
    return;
  }
  glViewport(0, 0, size.x, size.y);
  glUseProgram(r2d->current_shader.id);
  glUniform1i(r2d->current_shader.u_sampler, 0);
  bind_texture(&t, 0);
  renderQuadToScreenInternal(r2d);
  glBindVertexArray(0);
}

void flush_post_process(renderer_2d *r2d, const vector(shader) *post_process, framebuffer fb, bool should_clear) {
  if (vector_empty(*post_process)) {
    if (should_clear) {
      clear_draw_data(r2d);
      return;
    }
  }
  if (!r2d->post_process_fbo_1.fbo) {
    create_framebuffer(&r2d->post_process_fbo_1, 0, 0);
  }
  resize_framebuffer(&r2d->post_process_fbo_1, r2d->window_w, r2d->window_h);
  clear_framebuffer(&r2d->post_process_fbo_1);
  flush_fbo(r2d, r2d->post_process_fbo_1, should_clear);
  r2d->internal_post_process_flip = 1;
  post_process_over_texture(r2d, post_process, r2d->post_process_fbo_1.texture, fb);
}

void post_process_over_texture(renderer_2d *r2d, const vector(shader) *post_process, texture in, framebuffer fb) {
  if (vector_empty(*post_process)) {
    return;
  }
  if (!r2d->post_process_fbo_1.fbo) { 
    create_framebuffer(&r2d->post_process_fbo_1, 0, 0);
  }
  if (!r2d->post_process_fbo_2.fbo && vector_size(*post_process) > 1) {
    create_framebuffer(&r2d->post_process_fbo_2, 0, 0);
  }
  if (r2d->internal_post_process_flip == 0) {
    resize_framebuffer(&r2d->post_process_fbo_1, r2d->window_w, r2d->window_h);
    clear_framebuffer(&r2d->post_process_fbo_1);
    resize_framebuffer(&r2d->post_process_fbo_2, r2d->window_w, r2d->window_h);
    clear_framebuffer(&r2d->post_process_fbo_2); 
  }
  else if(r2d->post_process_fbo_2.fbo) {
    resize_framebuffer(&r2d->post_process_fbo_2, r2d->window_w, r2d->window_h);
    clear_framebuffer(&r2d->post_process_fbo_2);
  }
  for (int i = 0; i < vector_size(*post_process); i++) {
    framebuffer output;
    texture input;
    if (r2d->internal_post_process_flip == 0) {
      input = r2d->post_process_fbo_2.texture;
      output = r2d->post_process_fbo_1;
    }
    else {
      input = r2d->post_process_fbo_1.texture;
      output = r2d->post_process_fbo_2;
    }
    if (i == 0) {
      input = in;
    }
    if (i == vector_size(*post_process) - 1) {
      output = fb;
    }
    clear_framebuffer(&output);
    render_post_process(r2d, *post_process[i], input, output);
    r2d->internal_post_process_flip = !r2d->internal_post_process_flip;
  }
  r2d->internal_post_process_flip = 0;
}

void enable_GL_necessary_features(void) {
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}
