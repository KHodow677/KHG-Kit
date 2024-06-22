#include "khg_ui/box.h"
#include "khg_ui/renderer_ui.h"

vec4 box_operate(box *b) {
  if (b->dimensions_state == 1) {
    b->dimensions.w = b->dimensions.z * b->aspect;
  }
  else if (b->dimensions_state == 2) {
    b->dimensions.z = b->dimensions.w * b->aspect;
  }
  if (b->x_center_state == -1) {
    b->dimensions.x += x_padd;
  }
  if (b->y_center_state == -1)
  {
    b->dimensions.y += y_padd;
  }
  if (b->x_center_state == 1) {
    b->dimensions.x += x_padd + (width / 2.0f) - (b->dimensions.z / 2);
  }
  if (b->y_center_state == 1) {
    b->dimensions.y += y_padd + (height / 2.0f) - (b->dimensions.w / 2.0f);
  }
  if (b->x_center_state == 2) {
    b->dimensions.x += x_padd + width - b->dimensions.z;
  }
  if (b->y_center_state == 2) {
    b->dimensions.y += y_padd + height - b->dimensions.w;
  }
  return b->dimensions;
}
