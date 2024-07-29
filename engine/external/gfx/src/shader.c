#include "gfx/shader.h"
#include "khg_utl/error_func.h"

uint32_t shader_create(GLenum type, const char *src) {
  uint32_t shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
  int32_t compiled; 
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if(!compiled) {
    error_func("Failed to compile shader", user_defined_data);
    glDeleteShader(shader);
  }
  return shader;
}

LfShader shader_prg_create(const char *vert_src, const char *frag_src) {
  uint32_t vertex_shader = shader_create(GL_VERTEX_SHADER, vert_src);
  uint32_t fragment_shader = shader_create(GL_FRAGMENT_SHADER, frag_src);
  LfShader prg;
  prg.id = glCreateProgram();
  glAttachShader(prg.id, vertex_shader);
  glAttachShader(prg.id, fragment_shader);
  glLinkProgram(prg.id);
  int32_t linked;
  glGetProgramiv(prg.id, GL_LINK_STATUS, &linked);
  if(!linked) {
    error_func("Failed to link shader program", user_defined_data);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteProgram(prg.id);
    return prg;
  }
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  return prg;
}

void shader_set_mat(LfShader prg, const char *name, mat4 mat) {
  glUniformMatrix4fv(glGetUniformLocation(prg.id, name), 1, GL_FALSE, mat[0]);
}

void set_projection_matrix() { 
  float p_left = 0.0f;
  float p_right = state.dsp_w;
  float p_bottom = state.dsp_h;
  float p_top = 0.0f;
  float p_near = 0.1f;
  float p_far = 100.0f;
  mat4 orthoMatrix = GLM_MAT4_IDENTITY_INIT;
  orthoMatrix[0][0] = 2.0f / (p_right - p_left);
  orthoMatrix[1][1] = 2.0f / (p_top - p_bottom);
  orthoMatrix[2][2] = -1;
  orthoMatrix[3][0] = -(p_right + p_left) / (p_right - p_left);
  orthoMatrix[3][1] = -(p_top + p_bottom) / (p_top - p_bottom);
  shader_set_mat(state.render.shader, "u_proj", orthoMatrix);
}
