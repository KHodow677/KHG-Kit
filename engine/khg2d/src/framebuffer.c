#include "khg2d/framebuffer.h"
#include <string.h>

void createFramebuffer(framebuffer *fb, unsigned int w, unsigned int h) {
  glGenFramebuffers(1, &fb->fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);
  glGenTextures(1, &fb->texture.id);
  glBindTexture(GL_TEXTURE_2D, fb->texture.id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->texture.id, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void resizeFramebuffer(framebuffer *fb, unsigned int w, unsigned int h) {
  glBindTexture(GL_TEXTURE_2D, fb->texture.id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
}

void cleanupFramebuffer(framebuffer *fb) {
  if (fb->fbo) {
    glDeleteFramebuffers(1, &fb->fbo);
    fb->fbo = 0;
  }
  if (fb->texture.id) {
    glDeleteTextures(1, &fb->texture.id);
    memset(&fb->texture, 0, sizeof(texture));
  }
}

void clearFramebuffer(framebuffer *fb) {
  glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
