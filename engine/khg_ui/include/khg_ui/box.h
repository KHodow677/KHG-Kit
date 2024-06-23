#include "khg_math/vec2.h"
#include "khg_math/vec4.h"
#include <stdbool.h>

typedef struct {
  vec4 dimensions;
  float aspect;
  char x_center_state;
  char y_center_state;
  char dimensions_state;
} box;

vec4 box_operate(box *b);
box *x_distance_pixels(box *b, int dist);
box *y_distance_pixels(box *b, int dist);
box *x_center(box *b, int dist);
box *y_center(box *b, int dist);
box *x_left(box *b, int dist);
box *x_left_perc(box *b, float perc);
box *y_top(box *b, int dist);
box *y_top_perc(box *b, float perc);
box *x_right(box *b, int dist);
box *y_bottom(box *b, int dist);
box *x_dimension_pixels(box *b, int dim);
box *y_dimension_pixels(box *b, int dim);
box *x_dimension_percentage(box *b, float p);
box *y_dimension_percentage(box *b, float p);
box *x_aspect_ratio(box *b, float r);
box *y_aspect_ratio(box *b, float r);
bool is_in_button(const vec2 *p, const vec4 *box_vec);

