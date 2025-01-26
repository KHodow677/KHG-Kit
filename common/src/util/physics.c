#include "ecs/comp_mover.h"
#include "khg_phy/contact.h"
#include "khg_phy/space.h"
#include "khg_phy/core/phy_vector.h"
#include "util/physics.h"

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

void physics_on_contact_added(phy_space *space, phy_contact_event event, void *user_arg) {
  physics_collision_info *collision_info = user_arg;
  mover_on_collision_added(event, collision_info);
}

void physics_on_contact_persisted(phy_space *space, phy_contact_event event, void *user_arg) {
}

void physics_on_contact_removed(phy_space *space, phy_contact_event event, void *user_arg) {
}

