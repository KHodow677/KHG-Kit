#pragma once

#include "khg_phy/body.h"
#include <stdint.h>

typedef struct phy_broadphase_pair {
  phy_rigid_body *a;
  phy_rigid_body *b;
} phy_broadphase_pair;

static inline uint64_t phy_broadphase_pair_hash(void *item) {
  phy_broadphase_pair *pair = (phy_broadphase_pair *)item;
  return phy_u32_pair(pair->a->id, pair->b->id);
}

typedef enum {
  PHY_BROADPHASE_ALGORITHM_BRUTE_FORCE,
  PHY_BROADPHASE_ALGORITHM_BVH
} phy_broadphase_algorithm;

void phy_broadphase_brute_force(struct phy_space *space);
void phy_broadphase_bvh(struct phy_space *space);

void phy_broadphase_finalize(struct phy_space *space);

