#include "ecs/comp_mover.h"
#include "khg_phy/contact.h"
#include "khg_phy/space.h"
#include "physics/collisions.h"
#include "physics/physics.h"

void physics_on_contact_added(phy_space *space, phy_contact_event event, void *user_arg) {
  physics_collision_info *collision_info = user_arg;
  mover_on_collision_added(event, collision_info);
}

void physics_on_contact_persisted(phy_space *space, phy_contact_event event, void *user_arg) {

}

void physics_on_contact_removed(phy_space *space, phy_contact_event event, void *user_arg) {

}

