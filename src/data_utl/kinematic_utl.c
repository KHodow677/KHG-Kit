#include "data_utl/kinematic_utl.h"
#include <math.h>

float round_to_tenths(float val) {
  return roundf(val * 10.0f) / 10.0f;
}

float normalize_angle(float angle) {
  while (angle < 0) {
    angle += TWO_PI;
  }
  while (angle >= TWO_PI) {
    angle -= TWO_PI;
  }
  return angle;
}

