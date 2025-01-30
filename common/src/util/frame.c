#include "util/frame.h"
#include <stdbool.h>

void frame_tick(frame_info *frame, float dt) {
  frame->timer -= dt;
}

bool frame_time_up(frame_info *frame) {
  return frame->timer <= 0;
}

bool frame_time_close(frame_info *frame, float tolerance) {
  return frame->timer <= tolerance;
}

void frame_reset(frame_info *frame) {
  frame->timer = frame->duration;
}

float frame_perc(frame_info *frame) {
  return 1.0f - frame->timer / frame->duration;
}

