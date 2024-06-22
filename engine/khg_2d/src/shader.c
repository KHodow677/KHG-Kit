#include "khg_2d/shader.h"
#include <string.h>

void bind_shader(shader *s) {
  glUseProgram(s->id);
}

void clear_shader(shader *s) {
  glDeleteProgram(s->id);
  memset(s, 0, sizeof(shader));
}
