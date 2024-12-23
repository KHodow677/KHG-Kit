#pragma once

#include "khg_phy/body.h"
#include "khg_phy/space.h"
#include "khg_phy/core/phy_vector.h"

typedef struct physics_collision_info {
  phy_rigid_body *player_body;
  bool *player_on_ground;
} physics_collision_info;

extern phy_space *SPACE;
extern physics_collision_info COLLISION_INFO;

void physics_setup(const phy_vector2 grav);
void physics_cleanup(void);

