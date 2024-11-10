#pragma once

#include "khg_phy/aabb.h"
#include "khg_phy/core/phy_array.h"
#include <stdbool.h>

typedef struct phy_bvh_node {
  bool is_leaf;
  struct phy_bvh_node *left;
  struct phy_bvh_node *right;
  phy_aabb aabb;
  phy_array *bodies;
  size_t depth;
} phy_bvh_node;

phy_bvh_node *phy_bvh_node_new(bool is_leaf, phy_array *bodies);
void phy_bvh_node_free(phy_bvh_node *node);

void phy_bvh_node_build_aabb(phy_bvh_node *node);

void phy_bvh_node_subdivide(phy_bvh_node *node);
void phy_bvh_node_collide(phy_bvh_node *node, phy_aabb aabb, phy_array *collided);

size_t phy_bvh_node_size(phy_bvh_node *node);

phy_bvh_node *phy_bvh_tree_new(phy_array *bodies);
void phy_bvh_tree_free(phy_bvh_node *root);

