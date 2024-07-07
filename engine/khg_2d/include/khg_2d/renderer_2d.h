#pragma once

#include "khg_2d/camera.h"
#include "khg_2d/font.h"
#include "khg_2d/framebuffer.h"
#include "khg_2d/renderer_2d_buffer_type.h"
#include "khg_2d/shader.h"
#include "khg_2d/texture.h"
#include "khg_math/vec2.h"
#include "khg_math/vec4.h"
#include "khg_utils/string.h"
#include "khg_utils/vector.h"

typedef struct {
  GLuint default_fbo;
  GLuint buffers[buffer_size];
  GLuint vao;
  vector(vec2) sprite_positions;
  vector(vec4) sprite_colors;
  vector(vec2) texture_positions;
  vector(texture) sprite_textures;
  shader current_shader;
  vector(shader) shader_push_pop;
  camera current_camera;
  vector(camera) camera_push_pop;
  int window_w;
  int window_h;
  framebuffer post_process_fbo_1;
  framebuffer post_process_fbo_2;
  bool internal_post_process_flip;
} renderer_2d;

void create_renderer_2d(renderer_2d *r2d, GLuint fbo, size_t quad_count);
void cleanup_renderer_2d(renderer_2d *r2d);
void push_shader(renderer_2d *r2d, shader s);
void pop_shader(renderer_2d *r2d);
void push_camera(renderer_2d *r2d, camera c);
void pop_camera(renderer_2d *r2d);
vec4 get_view_rect(renderer_2d *r2d);
void update_window_metrics(renderer_2d *r2d, int w, int h);
vec4 pix_to_screen(renderer_2d *r2d, const vec4 *transform);
void clear_draw_data(renderer_2d *r2d);
vec2 get_text_size(renderer_2d *r2d, const char *text, const font font, const float size, const float spacing, const float line_space);
void render_text(renderer_2d *r2d, vec2 position, const char *text, const font f, const vec4 color, const float size, const float spacing, const float line_space, bool show_in_center, const vec4 shadow_color, const vec4 light_color);
float determine_text_rescale_fit_smaller(renderer_2d *r2d, const string *str, font *f, vec4 transform, float max_size);
float determine_text_rescale_fit_bigger(renderer_2d *r2d, const string *str, font *f, vec4 transform, float min_size);
float determine_text_rescale_fit(renderer_2d *r2d, const string *str, font *f, vec4 transform);
int wrap(renderer_2d *r2d, const string *in, font *f, float base_size, float max_dimension, string *out_res);
void render_text_wrapped(renderer_2d *r2d, const string *text, font f, vec4 text_pos, vec4 color, float base_size, float spacing, float line_spacing, bool show_in_center, vec4 shadow_color, vec4 light_color);
vec2 get_text_size_wrapped(renderer_2d *r2d, const string *text, font f, float max_text_length, float base_size, float spacing, float line_spacing);
void render_rectangle_texture(renderer_2d *r2d, const vec4 transforms, const texture texture, const vec4 colors[4], const vec2 origin, const float rotation_degrees, const vec4 texture_coords);
void render_rectangle_texture_abs_rotation(renderer_2d *r2d, const vec4 transforms, const texture texture, const vec4 colors[4], const vec2 origin, const float rotation_degrees, const vec4 texture_coords);
void render_rectangle(renderer_2d *r2d, const vec4 transforms, const vec4 colors[4], const vec2 origin, const float rotation_degrees);
void render_rectangle_abs_rotation(renderer_2d *r2d, const vec4 transforms, const vec4 colors[4], const vec2 origin, const float rotation_degrees);
void render_line_angle(renderer_2d *r2d, vec2 position, const float angle_degrees, const float length, const vec4 color, const float width);
void render_line_start_end(renderer_2d *r2d, const vec2 start, const vec2 end, const vec4 color, const float width);
void render_rectangle_outline(renderer_2d *r2d, const vec4 position, const vec4 color, const float width, const vec2 origin, const float rotation_degrees);
void render_circle_outline(renderer_2d *r2d, const vec2 position, const vec4 color, const float size, const float width, const unsigned int segments);
void render_nine_patch(renderer_2d *r2d, const vec4 position, const vec4 color, const vec2 origin, const float rotation_degrees, const texture texture, const vec4 texture_coords, const vec4 inner_texture_coords);
void clear_screen(renderer_2d *r2d, const vec4 color);
void set_shader(renderer_2d *r2d, const shader s);
void set_camera(renderer_2d *r2d, const camera c);
void reset_shader_and_camera(renderer_2d *r2d);
void render_post_process(renderer_2d *r2d, shader shader, texture input, framebuffer result);
void flush(renderer_2d *r2d, bool should_clear);
void flush_fbo(renderer_2d *r2d, framebuffer fb, bool should_clear);
void render_framebuffer_to_entire_screen(renderer_2d *r2d, framebuffer fb, framebuffer screen);
void render_texture_to_entire_screen(renderer_2d *r2d, texture t, framebuffer screen);
void flush_post_process(renderer_2d *r2d, const vector(shader) *post_process, framebuffer fb, bool should_clear);
void post_process_over_texture(renderer_2d *r2d, const vector(shader) *post_process, texture in, framebuffer fb);
void enable_GL_necessary_features(void);

