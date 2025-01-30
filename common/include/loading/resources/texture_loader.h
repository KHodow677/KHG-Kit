#pragma once

#include "khg_gfx/texture.h"

void generate_tex_defs(const char *filename);
void emplace_tex_defs_tick(void *arg);
int emplace_tex_defs(void *arg);
gfx_texture get_tex_def(char *tex_str);
void free_tex_defs(void);

