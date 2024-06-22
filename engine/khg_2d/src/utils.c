#include "khg_2d/renderer_2d.h"
#include "khg_2d/shader.h"
#include "khg_2d/utils.h"
#include "khg_math/math.h"
#include "khg_math/vec4.h"
#include "khg_utils/error_func.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

bool has_initialized = false;
shader default_shader = { 0 };
camera default_camera = { 0 };
texture white_1_px_square_texture = { 0 };

char *default_vertex_shader = "";
char *default_fragment_shader = "";
char *default_vertex_post_process_shader = "";

float position_to_screen_coords_x(const float position, float w) {
  return (position / w) * 2 - 1;
}

float position_to_screen_coords_y(const float position, float h) {
  return -((-position / h) * 2 - 1);
}

vec2 convert_point(const camera *c, const vec2 *p, float window_w, float window_h) {
  vec2 r = *p;
  vec2 camera_center_1 = { c->position.x + window_w / 2, c->position.y - window_h / 2 };
  vec2 camera_center_2 = { c->position.x + window_w / 2, c->position.y + window_h / 2 };
  r = (vec2){ c->position.x, c->position.y };
  r = rotate_around_point(r, camera_center_1, c->rotation);
  r = scale_around_point(r, camera_center_2, 1 / c->zoom);
  return r;
}

GLuint load_shader(const char *source, GLenum shader_type) {
  GLuint id = glCreateShader(shader_type);
  int result = 0;
  glShaderSource(id, 1, &source, 0);
  glCompileShader(id);
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if (!result) {
    int l = 0;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &l);
    char *message = malloc(l);
    glGetShaderInfoLog(id, l, &l, message);
    message[l - 1] = 0;
    error_func(message, user_defined_data);
    free(message);
  }
  return id;
}

void init(void) {
  if (has_initialized) { 
    return;
  }
  has_initialized = true;
  default_vertex_shader = load_file_contents("./res/shaders/defaultVertexShader.vert");
  default_fragment_shader = load_file_contents("./res/shaders/defaultFragmentShader.frag");
  default_vertex_post_process_shader = load_file_contents("./res/shaders/defaultVertexPostProcessShader.vert");
  default_shader = create_shader(default_vertex_shader, default_fragment_shader);
  default_camera = create_camera();
  create_1_px_square(&white_1_px_square_texture, 0);
  enable_GL_necessary_features();
}

void cleanup(void) {
  cleanup_texture(&white_1_px_square_texture);
  clear_shader(&default_shader);
  has_initialized = false;
}

vec2 rotate_around_point(vec2 vector, vec2 point, const float degrees) {
  float new_x, new_y;
  float a = radians(degrees);
  float s = sinf(a);
  float c = cosf(a);
  point.y = -point.y;
  vector.x -= point.x;
  new_x = vector.x * c - vector.y * s;
  new_y = vector.x * s + vector.y * c; vector.y -= point.y;
  vector = (vec2){ new_x + point.x, new_y + point.y };
  return vector;
}

vec2 scale_around_point(vec2 vector, vec2 point, float scale) {
  vec2 point_dif = vec2_subtract(&vector, &point);
  vec2 scaled_dif = vec2_multiply_num_on_vec2(scale, &point_dif);
  return vec2_add(&scaled_dif, &point);
}

void validate_program(GLuint id) {
  int info = 0;
  glGetProgramiv(id, GL_LINK_STATUS, &info);
  if (info != GL_TRUE) {
    int l = 0;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &l);
    char *message = malloc(l);
    glGetProgramInfoLog(id, l, &l, message);
    error_func(message, user_defined_data);
    free(message);
  }
  glValidateProgram(id);
}

camera create_camera(void) {
  return (camera){ .zoom = 1, .position = (vec2){ 0.0f, 0.0f }, .rotation = 0.0f };
}

shader create_shader(const char *vertex, const char *fragment) {
  shader shader = { 0 };
  const GLuint vertex_id = load_shader(vertex, GL_VERTEX_SHADER);
  const GLuint fragment_id = load_shader(fragment, GL_FRAGMENT_SHADER);
  shader.id = glCreateProgram();
  glAttachShader(shader.id, vertex_id);
  glAttachShader(shader.id, fragment_id);
  glBindAttribLocation(shader.id, 0, "quad_positions");
  glBindAttribLocation(shader.id, 1, "quad_colors");
  glBindAttribLocation(shader.id, 2, "texturePositions");
  glLinkProgram(shader.id);
  glDeleteShader(vertex_id);
  glDeleteShader(fragment_id);
  validate_program(shader.id);
  shader.u_sampler = glGetUniformLocation(shader.id, "u_sampler");
  return shader;
}

shader create_shader_from_file(const char *filePath) {
  FILE *file = fopen(filePath, "rb");
  shader empty_shader;
  if (!file) {
      char e[256];
      error_func(e, user_defined_data);
      empty_shader.id = 0;
      empty_shader.u_sampler = 0;
      return empty_shader;
  }
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *file_data = (char *)malloc(file_size + 1);
  if (file_data == NULL) {
      char e[256];
      error_func(e, user_defined_data);
      fclose(file);
      empty_shader.id = 0;
      empty_shader.u_sampler = 0;
      return empty_shader;
  }
  fread(file_data, 1, file_size, file);
  fclose(file);
  file_data[file_size] = '\0';
  shader res = create_shader_default_vertex(file_data);
  free(file_data);
  return res;
}

shader create_shader_default_vertex(const char *fragment) {
  return create_shader(default_vertex_shader, fragment);
}

shader create_post_process_shader_from_file(const char *file_path) {
  FILE *file = fopen(file_path, "rb");
  shader empty_shader;
  if (!file) {
    char e[256];
    error_func(e, user_defined_data);
    empty_shader.id = 0;
    empty_shader.u_sampler = 0;
    return empty_shader;
  }
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *file_data = (char *)malloc(file_size + 1);
  if (file_data == NULL) {
    char e[256];
    error_func(e, user_defined_data);
    fclose(file);
    empty_shader.id = 0;
    empty_shader.u_sampler = 0;
    return empty_shader;
  }
  fread(file_data, 1, file_size, file);
  fclose(file);
  file_data[file_size] = '\0';
  shader res = create_post_process_shader(file_data);
  free(file_data);
  return res;
}

shader create_post_process_shader(const char *fragment) {
  return create_shader(default_vertex_post_process_shader, fragment);
}

void clean_texture_coordinates(int t_size_x, int t_size_y, int x, int y, int size_x, int size_y, int s1, int s2, int s3, int s4, vec4 *outer, vec4 *inner) {
  float newX = (float)t_size_x / (float)x;
  float newY = (float)t_size_y / (float)y;
  float newSizeX = (float)t_size_x / (float)size_x;
  float newSizeY = (float)t_size_y / (float)size_y;
  newY = 1 - newY;
  if (outer) {
    outer->x = newX;
    outer->y = newY;
    outer->z = newX + newSizeX;
    outer->w = newY - newSizeY;
  }
  if (inner) {
    inner->x = newX + ((float)s1 / t_size_x);
    inner->y = newY - ((float)s2 / t_size_y);
    inner->z = newX + newSizeX - ((float)s3 / t_size_x);
    inner->w = newY - newSizeY + ((float)s4 / t_size_y);
  }
}

char *load_file_contents(char const *path) {
    FILE *f = fopen (path, "rb");
    fseek (f, 0, SEEK_END);
    long length = ftell (f);
    fseek (f, 0, SEEK_SET);
    char *buffer = (char*)malloc ((length+1)*sizeof(char));
    if (buffer) {
      fread (buffer, sizeof(char), length, f);
    }
    fclose (f);
    buffer[length] = '\0';
    return buffer;
}
