#pragma once

#include "khg_gfx/texture.h"

void generate_tex_defs(const char *filename);
void emplace_tex_defs_tick(void *arg);
int emplace_tex_defs(void *arg);
void free_tex_defs(void);

gfx_texture get_tex_def_by_location(unsigned int loc);
gfx_texture get_tex_def(char *tex_str);
unsigned int get_location_tex_str(const char *tex_str);

