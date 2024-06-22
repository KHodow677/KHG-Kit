#pragma once

#include "khg_math/vec4.h"
#include <stdbool.h>

typedef struct {
  int lastW;
  int lastH;
  int lastX;
  int lastY;
  bool loaded;
} frame;

void create_frame(frame *f, vec4 size);
void create_frame_from_other(frame *f, frame *other);
void create_frame_from_other_ptr(frame *f, frame **other_ptr);
void delete_frame(frame *f);
