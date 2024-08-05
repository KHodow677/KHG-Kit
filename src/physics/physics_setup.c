#include "physics_setup.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/space.h"

cpSpace *physics_setup(cpVect *grav) {
  cpSpace *sp = cpSpaceNew();
  cpSpaceSetGravity(sp, *grav);
  return sp;
}

void physics_free(cpSpace *sp) {
  cpSpaceFree(sp);
}

