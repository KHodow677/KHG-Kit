#include "khg2d.h"
#include <string.h>

void bindShader(shader *s) {
  glUseProgram(s->id);
}

void clearShader(shader *s) {
  glDeleteProgram(s->id);
  memset(s, 0, sizeof(shader));
}

