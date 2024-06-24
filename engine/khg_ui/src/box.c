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

box *x_distance_pixels(box *b, int dist) {
  b->dimensions.x = dist;
  b->x_center_state = 0;
  return b;
}

box *y_distance_pixels(box *b, int dist) {
  b->dimensions.y = dist;
  b->y_center_state = 0;
  return b;
}

box *x_center(box *b, int dist) {
  b->dimensions.x = dist;
  b->x_center_state = 1;
  return b;
}

box *y_center(box *b, int dist) {
  b->dimensions.y = dist;
  b->y_center_state = 1;
  return b;
}

box *x_left(box *b, int dist) {
  b->dimensions.x = dist;
  b->x_center_state = -1;
  return b;
}

box *x_left_perc(box *b, float perc) {
  x_left(b, perc * width);
  return b;
}

box *y_top(box *b, int dist) {
  b->dimensions.y = dist;
  b->y_center_state = -1;
  return b;
}

box *y_top_perc(box *b, float perc) {
  y_top(b, perc * height);
  return b;
}

box *x_right(box *b, int dist) {
  b->dimensions.x = dist;
  b->x_center_state = 2;
  return b;
}

box *y_bottom(box *b, int dist) {
  b->dimensions.y = dist;
  b->y_center_state = 2;
  return b;
}

box *x_dimension_pixels(box *b, int dim) {
  b->dimensions_state = 0;
  b->dimensions.z = dim;
  return b;
}

box *y_dimension_pixels(box *b, int dim) {
  b->dimensions_state = 0;
  b->dimensions.w = dim;
  return b;
}

box *x_dimension_percentage(box *b, float p) {
  b->dimensions_state = 0;
  b->dimensions.z = p * width;
  return b;
}

box *y_dimension_percentage(box *b, float p) {
  b->dimensions_state = 0;
  b->dimensions.w = p * height;
  return b;
}

box *x_aspect_ratio(box *b, float r) {
  b->dimensions_state = 2;
  b->aspect = r;
  return b;
}

box *y_aspect_ratio(box *b, float r) {
  b->dimensions_state = 1;
  b->aspect = r;
  return b;
}

bool is_in_button(const vec2 *p, const vec4 *box_vec) {
  return (p->x >= box_vec->x && p->x <= box_vec->x + box_vec->z && p->y >= box_vec->y && p->y <= box_vec->y + box_vec->w);
}

