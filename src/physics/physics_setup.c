#include "physics/physics_setup.h"
#include "game_manager.h"
#include "khg_phy/phy_types.h"
#include "khg_phy/space.h"
#include "khg_phy/threaded_space.h"

phy_space *physics_setup(phy_vect grav) {
  phy_space *sp = phy_threaded_space_new();
  phy_threaded_space_set_threads(sp, THREAD_COUNT);
  phy_space_set_gravity(sp, grav);
  return sp;
}

void physics_free(phy_space *sp) {
  phy_threaded_space_free(sp);
}

