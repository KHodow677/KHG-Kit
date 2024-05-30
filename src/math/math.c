#include "math.h"

float radians(float degrees) {
  return degrees * (PI / 180);
}

float degrees(float radians) {
  return radians * (180 / PI);
}
