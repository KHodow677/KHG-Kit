#pragma once

#include "khgmath/vec2.h"
#include "khgmath/vec4.h"
#include "camera.h"
#include "shader.h"
#include "texture.h"

#define KHG2D_DEFAULT_TEXTURE_LOAD_MODE_PIXELATED false
#define KHG2D_DEFAULT_TEXTURE_LOAD_MODE_USE_MIPMAPS true

static vec4 color_red = { 1, 0, 0, 1 };
static vec4 color_green = { 0, 1, 0, 1 };
static vec4 color_blue = { 0, 0, 1, 1 };
static vec4 color_black = { 0, 0, 0, 1 };
static vec4 color_white = { 1, 1, 1, 1 };
static vec4 color_yellow = { 1, 1, 0, 1 };
static vec4 color_magenta = { 1, 0, 1, 1 };
static vec4 color_turquoise = { 0, 1, 1, 1 };
static vec4 color_orange = { 1, (float)0x7F / 255.0f, 0, 1 };
static vec4 color_purple = { 101.0f / 255.0f, 29.0f / 255.0f, 173.0f / 255.0f, 1 };
static vec4 color_gray = { (float)0x7F / 255.0f, (float)0x7F / 255.0f, (float)0x7F / 255.0f, 1 };
static vec4 color_transparent = { 0, 0, 0, 0 };

extern bool has_initialized;
extern shader default_shader;
extern camera default_camera;
extern texture white_1_px_square_texture;

static vec4 default_texture_coords = { 0, 1, 1, 0 };

extern char *default_vertex_shader;
extern char *default_fragment_shader;
extern char *default_vertex_post_process_shader;

float position_to_screen_coords_x(const float position, float w);
float position_to_screen_coords_y(const float position, float h);
vec2 convert_point(const camera *c, const vec2 *p, float window_w, float window_h);
GLuint load_shader(const char *source, GLenum shader_type);
void init(void);
void cleanup(void);
vec2 rotate_around_point(vec2 vec, vec2 point, const float degrees);
vec2 scale_around_point(vec2 vec, vec2 point, float scale);
camera create_camera(void);
shader create_shader(const char *vertex, const char *fragment);
shader create_shader_from_file(const char *file_path);
shader create_shader_default_vertex(const char *fragment);
shader create_post_process_shader_from_file(const char *file_path);
shader create_post_process_shader(const char *fragment);
void clean_texture_coordinates(int t_size_x, int t_size_y, int x, int y, int size_x, int size_y, int s1, int s2, int s3, int s4, vec4 *outer, vec4 *inner);
char *load_file_contents(char const *path);
