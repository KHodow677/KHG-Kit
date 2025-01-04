#include "khg_phy/narrowphase.h"
#include "khg_phy/space.h"
#include "khg_phy/math.h"
#include "khg_phy/contact.h"
#include "khg_phy/collision.h"

static void generate_contact_pair(phy_persistent_contact_pair *pcp, phy_rigid_body *body_a, phy_rigid_body *body_b, phy_shape *shape_a, phy_shape *shape_b) {
  phy_transform xform_a = {body_a->origin, body_a->angle};
  phy_transform xform_b = {body_b->origin, body_b->angle};
  pcp->contact_count = 0;
  if (shape_a->type == PHY_SHAPE_TYPE_POLYGON && shape_b->type == PHY_SHAPE_TYPE_POLYGON) {
    *pcp = phy_collide_polygon_x_polygon(shape_a, xform_a, shape_b, xform_b);
  }
  else if (shape_a->type == PHY_SHAPE_TYPE_CIRCLE && shape_b->type == PHY_SHAPE_TYPE_CIRCLE) {
    *pcp = phy_collide_circle_x_circle(shape_a, xform_a, shape_b, xform_b);
  }
  else if (shape_a->type == PHY_SHAPE_TYPE_CIRCLE && shape_b->type == PHY_SHAPE_TYPE_POLYGON) {
    *pcp = phy_collide_polygon_x_circle(shape_b, xform_b, shape_a, xform_a, true);
  }
  else if (shape_a->type == PHY_SHAPE_TYPE_POLYGON && shape_b->type == PHY_SHAPE_TYPE_CIRCLE) {
    *pcp = phy_collide_polygon_x_circle(shape_a, xform_a, shape_b, xform_b, false);
  }
  pcp->body_a = body_a;
  pcp->body_b = body_b;
  pcp->shape_a = shape_a;
  pcp->shape_b = shape_b;
}

void phy_narrow_phase(phy_space *space) {
  for (unsigned int i = 0; i < space->broadphase_pairs->current_size; i++) {
    void *pool_i = (char *)space->broadphase_pairs->pool + i * space->broadphase_pairs->chunk_size;
    phy_rigid_body *body_a = ((phy_broadphase_pair *)pool_i)->a;
    phy_rigid_body *body_b = ((phy_broadphase_pair *)pool_i)->b;
    if (!body_a || !body_b) {
      continue;
    }
    phy_vector2 com_a = phy_vector2_rotate(body_a->com, body_a->angle);
    phy_vector2 com_b = phy_vector2_rotate(body_b->com, body_b->angle);
    for (unsigned int j = 0; j < body_a->shapes->size; j++) {
      phy_shape *shape_a = body_a->shapes->data[j];
      for (unsigned int k = 0; k < body_b->shapes->size; k++) {
        phy_shape *shape_b = body_b->shapes->data[k];
        phy_persistent_contact_pair *old_pcp = phy_hashmap_get(space->contacts, &(phy_persistent_contact_pair){.shape_a=shape_a, .shape_b=shape_b});
        if (old_pcp) {
          phy_persistent_contact_pair pcp;
          generate_contact_pair(&pcp, body_a, body_b, shape_a, shape_b);
          phy_contact_event persisted_queue[6];
          phy_contact_event removed_queue[6];
          unsigned int persisted_queue_size = 0;
          unsigned int removed_queue_size = 0;
          for (unsigned int c = 0; c < pcp.contact_count; c++) {
            phy_contact *contact = &pcp.contacts[c];
            contact->anchor_a = phy_vector2_sub(contact->anchor_a, com_a);
            contact->anchor_b = phy_vector2_sub(contact->anchor_b, com_b);
            for (unsigned int old_c = 0; old_c < old_pcp->contact_count; old_c++) {
              phy_contact old_contact = old_pcp->contacts[old_c];
              if (old_contact.id == contact->id) {
                contact->is_persisted = true;
                contact->remove_invoked = old_contact.remove_invoked;
                if (space->settings.warmstarting) {
                  contact->solver_info = old_contact.solver_info;
                }
                if (space->listener) {
                  phy_contact_event event = { .body_a = body_a, .body_b = body_b, .shape_a = shape_a, .shape_b = shape_b, .normal = pcp.normal, .penetration = contact->separation, .position = phy_vector2_add(body_a->position, contact->anchor_a), .normal_impulse = {contact->solver_info.normal_impulse}, .friction_impulse = {contact->solver_info.tangent_impulse}, .id = contact->id };
                  if (contact->separation < 0.0) {
                    if (space->listener->on_contact_persisted) {
                      persisted_queue[persisted_queue_size++] = event;
                    }
                    contact->remove_invoked = false;
                  }
                  else if (!contact->remove_invoked) {
                    if (space->listener->on_contact_removed) {
                      removed_queue[removed_queue_size++] = event;
                    }
                    contact->remove_invoked = true;
                  }
                }
              }
            }
          }
          if (pcp.contact_count == 0 && old_pcp->contact_count > 0) {
            for (unsigned int old_c = 0; old_c < old_pcp->contact_count; old_c++) {
              phy_contact *contact = &old_pcp->contacts[old_c];
              phy_contact_event event = { .body_a = body_a, .body_b = body_b, .shape_a = shape_a, .shape_b = shape_b, .normal = pcp.normal, .penetration = contact->separation, .position = phy_vector2_add(body_a->position, contact->anchor_a), .normal_impulse = {contact->solver_info.normal_impulse}, .friction_impulse = {contact->solver_info.tangent_impulse}, .id = contact->id };
              if (space->listener && !contact->remove_invoked) {
                if (space->listener->on_contact_removed) {
                  removed_queue[removed_queue_size++] = event;
                }
                contact->remove_invoked = true;
              }
            }
          }
          phy_hashmap_set(space->contacts, &pcp);
          for (unsigned int qi = 0; qi < persisted_queue_size; qi++) {
            space->listener->on_contact_persisted(space, persisted_queue[qi], space->listener_arg);
          }
          for (unsigned int qi = 0; qi < removed_queue_size; qi++) {
            space->listener->on_contact_removed(space, removed_queue[qi], space->listener_arg);
          }
        }
        else {
          phy_persistent_contact_pair pcp;
          generate_contact_pair(&pcp, body_a, body_b, shape_a, shape_b);
          for (unsigned int c = 0; c < pcp.contact_count; c++) {
            phy_contact *contact = &pcp.contacts[c];
            contact->anchor_a = phy_vector2_sub(contact->anchor_a, com_a);
            contact->anchor_b = phy_vector2_sub(contact->anchor_b, com_b);
          }
          phy_hashmap_set(space->contacts, &pcp);
          for (unsigned int c = 0; c < pcp.contact_count; c++) {
            phy_contact *contact = &pcp.contacts[c];
            if (space->listener && space->listener->on_contact_added && contact->separation < 0.0) {
              phy_contact_event event = { .body_a = body_a, .body_b = body_b, .shape_a = shape_a, .shape_b = shape_b, .normal = pcp.normal, .penetration = contact->separation, .position = phy_vector2_add(body_a->position, contact->anchor_a), .normal_impulse = {contact->solver_info.normal_impulse}, .friction_impulse = {contact->solver_info.tangent_impulse}, .id = contact->id }; space->listener->on_contact_added(space, event, space->listener_arg);
            }
          }
        }
      }
    }
  }
}

