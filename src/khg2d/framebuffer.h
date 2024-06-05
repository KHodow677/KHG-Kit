#include "texture.h"

typedef struct {
  unsigned int fbo;
  texture texture;
} framebuffer;

void createFramebuffer(framebuffer *fb, unsigned int w, unsigned int h);
void resizeFramebuffer(framebuffer *fb, unsigned int w, unsigned int h);
void cleanupFramebuffer(framebuffer *fb);
void clearFrameBuffer(framebuffer *fb);
