#pragma once
#include "gfx/gfx.h"

uint32_t shader_create(GLenum type, const char *src);
LfShader shader_prg_create(const char* vert_src, const char *frag_src);
void shader_set_mat(LfShader prg, const char* name, mat4 mat); 
void set_projection_matrix(void);
