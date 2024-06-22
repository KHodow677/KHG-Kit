#include "khg_ui/renderer_ui.h"
#include "khg_ui/frame.h"
#include <stdlib.h>

void create_frame(frame *f, vec4 size) {
  f->loaded = 1;
  f->lastW = width;
  f->lastH = height;
  f->lastX = x_padd;
  f->lastY = y_padd;
  width = size.z;
  height = size.w;
  x_padd = size.x;
  y_padd = size.y;
}

void create_frame_from_other(frame *f, frame *other) {
  f->loaded = 1;
  f->lastW = other->lastW;
  f->lastH = other->lastH;
  f->lastX = other->lastX;
  f->lastY = other->lastY;
  delete_frame(other);
}

void create_frame_from_other_ptr(frame *f, frame **other_ptr) {
  f->loaded = 1;
  f->lastW = (*other_ptr)->lastW;
  f->lastH = (*other_ptr)->lastH;
  f->lastX = (*other_ptr)->lastX;
  f->lastY = (*other_ptr)->lastY;
  delete_frame(*other_ptr);
}

void delete_frame(frame *f) {
  if (f->loaded) {
    width = f->lastW;
    height = f->lastH;
    x_padd = f->lastX;
    y_padd = f->lastY;
  }
  free(f);
}

