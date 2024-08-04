#include "physics_setup.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/space.h"

void physics_setup() {
  cpVect gravity = cpv(0, 0);
  cpSpace *space = cpSpaceNew();
  cpSpaceSetGravity(space, gravity);
}

