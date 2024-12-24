#include "physics/physics.h"
#include "khg_phy/contact.h"
#include "khg_phy/space.h"
#include "khg_phy/core/phy_vector.h"
#include "physics/collisions.h"

phy_space *SPACE = NULL;
physics_collision_info COLLISION_INFO;

void physics_setup(const phy_vector2 grav) {
  SPACE = phy_space_new();
  phy_space_set_gravity(SPACE, grav);
  phy_space_settings *settings = phy_space_get_settings(SPACE);
  settings->penetration_slop = 0.0f;
  phy_space_set_contact_listener(SPACE, (phy_contact_listener){ physics_on_contact_added, physics_on_contact_persisted, physics_on_contact_removed }, &COLLISION_INFO);
}

void physics_cleanup() {
  phy_space_free(SPACE);
}

