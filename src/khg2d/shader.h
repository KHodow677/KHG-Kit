#pragma once

#include "glad/glad.h"

typedef struct shader {
  GLuint id;
  int u_sampler;
} shader;

void bindShader(shader *s);
void clearShader(shader *s);
