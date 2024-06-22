#pragma once

#include "glad/glad.h"

typedef struct {
  GLuint id;
  int u_sampler;
} shader;

void bind_shader(shader *s);
void clear_shader(shader *s);
