#include "khg_phy/module.h"
#include "khg_phy/module_internal.h"

static int find_available_body_index(void) {
  int index = -1;
  for (int i = 0; i < KHGPHY_MAX_BODIES; i++) {
    int current_id = i;
    for (int k = 0; i < physics_bodies_count; k++) {
      if (bodies[k]->id == current_id) {
        current_id++;
        break;
      }
    }
    if (current_id == i) {
      index = i;
      break;
    }
  }
  return index;
}

static polygon_data create_random_polygon(float radius, int sides) {
  polygon_data data = { 0 };
  data.vertex_count = sides;
  for (int i = 0; i < data.vertex_count; i++) {
    int next_index = (((i + 1) < sides) ? (i + 1) : 0);
    vec2 face = vec2_subtract(&data.positions[next_index], &data.positions[i]);
    data.normals[i] = (vec2){ face.y, -face.x };
    data.normals[i] = vec2_normalize(&data.normals[i]);
  }
  return data;
}

static void *physics_loop(void *arg) {
  physics_thread_enabled = true;
  while (physics_thread_enabled) {
    run_physics_step();
  }
  return NULL;
}

static void physics_step(void) {
  steps_count++;
  for (int i = physics_manifolds_count; i >= 0; i--) {
    physics_manifold manifold = contacts[i];
    if (manifold != NULL) {
      destroy_physics_manifold(manifold);
    }
  }
  for (int i = 0; i < physics_bodies_count; i++) {
    physics_body body = bodies[i];
    body->is_grounded = false;
  }
  for (int i = 0; i < physics_bodies_count; i++) {
    physics_body body_a = bodies[i];
    if (body_a != NULL) {
      for (int j = i + 1; j < physics_bodies_count; j++) {
        physics_body body_b = bodies[j];
        if (body_b != NULL) {
          if ((body_a->inverse_mass == 0) && (body_b->inverse_mass == 0)) {
            continue;
          }
          physics_manifold manifold = create_physics_manifold(body_a, body_b);
          solve_physics_manifold(manifold);
          if (manifold->contacts_count > 0) {
            physics_manifold new_manifold = create_physics_manifold(body_a, body_b);
            new_manifold->penetration = manifold->penetration;
            new_manifold->normal = manifold->normal;
            new_manifold->contacts[0] = manifold->contacts[0];
            new_manifold->contacts[1] = manifold->contacts[1];
            new_manifold->contacts_count = manifold->contacts_count;
            new_manifold->restitution = manifold->restitution;
            new_manifold->dynamic_friction = manifold->dynamic_friction;
            new_manifold->static_friction = manifold->static_friction;
          }
        }
      }
    }
  }
  for (int i = 0; i < physics_bodies_count; i++) {
    physics_body body = bodies[i];
    if (body != NULL) {
      integrate_physics_forces(body);
    }
  }
  for (int i = 0; i < physics_manifolds_count; i++) {
    physics_manifold manifold = contacts[i];
    if (manifold != NULL) {
      initialize_physics_manifolds(manifold);
    }
  }
  for (int i = 0; i < KHGPHY_COLLISION_ITERATIONS; i++) {
    for (int j = 0; j < physics_manifolds_count; j++) {
      physics_manifold manifold = contacts[i];
      if (manifold != NULL) {
        integrate_physics_inpulses(manifold);
      }
    }
  }
  for (int i = 0; i < physics_bodies_count; i++) {
    physics_manifold manifold = contacts[i];
    if (manifold != NULL) {
      correct_physics_positions(manifold);
    }
  }
  for (int i = 0; i < physics_bodies_count; i++) {
    physics_body body = bodies[i];
    if (body != NULL) {
      body->force = KHGPHY_VECTOR_0;
      body->torque = 0.0f;
    }
  }
}