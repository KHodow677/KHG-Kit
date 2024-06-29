#include "khg_math/math.h"

float radians(float degrees) {
  return degrees * (PI / 180);
}

float degrees(float radians) {
  return radians * (180 / PI);
}

int clamp(int value, int min, int max) {
    if (value < min) {
        return min;
    } else if (value > max) {
        return max;
    } else {
        return value;
    }
}

float clampf(float value, float min, float max) {
    if (value < min) {
        return min;
    } else if (value > max) {
        return max;
    } else {
        return value;
    }
}
