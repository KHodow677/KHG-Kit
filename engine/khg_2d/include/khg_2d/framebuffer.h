#pragma once

#include "khg_2d/texture.h"

typedef struct {
  unsigned int fbo;
  texture texture;
} framebuffer;

void create_framebuffer(framebuffer *fb, unsigned int w, unsigned int h);
void resize_framebuffer(framebuffer *fb, unsigned int w, unsigned int h);
void cleanup_framebuffer(framebuffer *fb);
void clear_framebuffer(framebuffer *fb);
