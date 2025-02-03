#pragma once

#include "khg_gfx/texture.h"

void populate_texture_data(const char *filename);
void load_texture_data(void *arg);

gfx_texture get_texture_data(const unsigned int tex_id);
const unsigned int get_texture_id(const char *tex_name);

void clear_texture_data(void);
