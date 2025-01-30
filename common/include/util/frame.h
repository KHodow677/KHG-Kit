#pragma once

#include <stdbool.h>

typedef struct frame_info {
  float timer;
  float duration;
} frame_info;

void frame_tick(frame_info *frame, float dt);
bool frame_time_up(frame_info *frame);
bool frame_time_close(frame_info *frame, float tolerance);
void frame_reset(frame_info *frame);
float frame_perc(frame_info *frame);

