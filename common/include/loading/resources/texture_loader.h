#pragma once

#include "khg_gfx/texture.h"

const unsigned int get_tex_id_from_string(const char *tex_key);
const gfx_texture get_texture(const unsigned int tex_id);
const gfx_texture get_texture_from_string(const char *tex_key);

void generate_textures(void);
int load_texture_raw_tick(void *arg);
int load_texture_tick(void *arg);

