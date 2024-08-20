#include "physics/physics_setup.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/space.h"

phy_space *physics_setup(phy_vect grav) {
  phy_space *sp = cpSpaceNew();
  cpSpaceSetGravity(sp, grav);
  return sp;
}

void physics_free(phy_space *sp) {
  cpSpaceFree(sp);
}

