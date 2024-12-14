#include "tile/collider.h"
#include <stdio.h>

const area_collider create_collider(phy_vector2 pos, phy_vector2 size) {
  printf("POS: %f, %f\n", pos.x, pos.y);
  printf("SIZE: %f, %f\n", size.x, size.y);
  return (area_collider){ true };
}

void free_collider(area_collider *collider) {

}

void free_colliders(utl_array *colliders) {

}

void render_colliders(utl_array *tiles, int parallax_value) {

}

