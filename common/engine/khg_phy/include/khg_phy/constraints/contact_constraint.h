#pragma once

#include "khg_phy/contact.h"

void phy_contact_presolve(struct phy_space *space, phy_persistent_contact_pair *pcp, float inv_dt);
void phy_contact_warmstart(struct phy_space *space, phy_persistent_contact_pair *pcp);
void phy_contact_solve_velocity(phy_persistent_contact_pair *pcp);
void phy_contact_solve_position(phy_persistent_contact_pair *pcp);

