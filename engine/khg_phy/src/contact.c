/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#include "khg_phy/contact.h"
#include "khg_phy/space.h"
#include <stdbool.h>
#include <stdint.h>


/**
 * @file contact.c
 * 
 * @brief Collision and contact information.
 */


bool phy_persistent_contact_pair_penetrating(phy_persistent_contact_pair *pcp) {
    bool penetrating = false;

    for (size_t c = 0; c < pcp->contact_count; c++) {
        phy_contact contact = pcp->contacts[c];
       
        if (contact.separation < 0.0) {
            penetrating = true;
            break;
       }
    }

    return penetrating;
}

uint64_t phy_persistent_contact_pair_hash(void *item) {
    phy_persistent_contact_pair *pcp = (phy_persistent_contact_pair *)item;
    return phy_persistent_contact_pair_key(pcp->shape_a, pcp->shape_b);
}

void phy_persistent_contact_pair_remove(
    phy_space *space,
    phy_persistent_contact_pair *pcp
) {
    for (size_t c = 0; c < pcp->contact_count; c++) {
        phy_contact *contact = &pcp->contacts[c];

        phy_contact_event event = {
            .body_a = pcp->body_a,
            .body_b = pcp->body_b,
            .shape_a = pcp->shape_a,
            .shape_b = pcp->shape_b,
            .normal = pcp->normal,
            .penetration = contact->separation,
            .position = phy_vector2_add(pcp->body_a->position, contact->anchor_a),
            .normal_impulse = {contact->solver_info.normal_impulse},
            .friction_impulse = {contact->solver_info.tangent_impulse},
            .id = contact->id
        };

        if (space->listener && !contact->remove_invoked) {
            if (space->listener->on_contact_removed)
                space->listener->on_contact_removed(space, event, space->listener_arg);
            contact->remove_invoked = true;
        };
    }

    phy_hashmap_remove(space->contacts, pcp);
}
