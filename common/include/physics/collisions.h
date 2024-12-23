#pragma once

#include "khg_phy/contact.h"
#include "khg_phy/space.h"

void physics_on_contact_added(phy_space *space, phy_contact_event event, void *user_arg);
void physics_on_contact_persisted(phy_space *space, phy_contact_event event, void *user_arg);
void physics_on_contact_removed(phy_space *space, phy_contact_event event, void *user_arg);

