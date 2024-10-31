#include "utility/math_utl.h"
#include <math.h>

const float round_to_tenths(const float val) {
  return roundf(val * 10.0f) / 10.0f;
}

const float normalize_angle(const float angle) {
  float temp_ang = angle;
  while (angle < 0) {
    temp_ang += TWO_PI;
  }
  while (angle >= TWO_PI) {
    temp_ang -= TWO_PI;
  }
  return angle;
}

