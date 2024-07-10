#include "khg_phy/module.h"
#include "khg_phy/module_internal.h"
#include <math.h>
#include <stdlib.h>

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

static int find_available_manifold_index(void) {
  int index = -1;
  for (int i = 0; i < KHGPHY_MAX_MANIFOLDS; i++) {
    int current_id = i;
    for (int k = 0; k < physics_manifolds_count; k++) {
      if (contacts[k]->id == current_id) {
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

static physics_manifold create_physics_manifold(physics_body a, physics_body b) {
  physics_manifold new_manifold = (physics_manifold)malloc(sizeof(physics_manifold_data));
  used_memory += sizeof(physics_manifold_data);
  int new_id = find_available_manifold_index();
  if (new_id != -1) {
    new_manifold->id = new_id;
    new_manifold->body_a = a;
    new_manifold->body_b = b;
    new_manifold->penetration = 0;
    new_manifold->normal = KHGPHY_VECTOR_0;
    new_manifold->contacts[0] = KHGPHY_VECTOR_0;
    new_manifold->contacts[1] = KHGPHY_VECTOR_0;
    new_manifold->contacts_count = 0;
    new_manifold->restitution = 0.0f;
    new_manifold->dynamic_friction = 0.0f;
    new_manifold->static_friction = 0.0f;
    contacts[physics_manifolds_count] = new_manifold;
    physics_manifolds_count++;
  }
  return new_manifold;
}

static void destroy_physics_manifold(physics_manifold manifold) {
  if (manifold != NULL) {
    int id = manifold->id;
    int index = -1;
    for (int i = 0; i < physics_manifolds_count; i++) {
      if (contacts[i]->id == id) {
        index = i;
        break;
      }
    }
    if (index == -1) {
      return;
    }
    free(manifold);
    used_memory -= sizeof(physics_manifold_data);
    contacts[index] = NULL;
    for (int i = index; i < physics_manifolds_count; i++) {
      if ((i + 1) < physics_manifolds_count) {
        contacts[i] = contacts[i + 1];
      }
    }
    physics_manifolds_count--;
  }
}

static void solve_physics_manifold(physics_manifold manifold) {
  switch (manifold->body_a->shape.type) {
    case PHYSICS_CIRCLE:
      switch (manifold->body_b->shape.type) {
        case PHYSICS_CIRCLE:
          solve_circle_to_circle(manifold);
          break;
        case PHYSICS_POLYGON:
          solve_circle_to_polygon(manifold);
          break;
        default:
          break;
      }
      break;
    case PHYSICS_POLYGON:
      switch (manifold->body_b->shape.type) {
        case PHYSICS_CIRCLE:
          solve_polygon_to_circle(manifold);
          break;
        case PHYSICS_POLYGON:
          solve_polygon_to_polygon(manifold);
        default:
          break;
      }
    default:
      break;
  }
  if (!manifold->body_b->is_grounded) {
    manifold->body_b->is_grounded = (manifold->normal.y < 0);
  }
}

static void solve_circle_to_circle(physics_manifold manifold) {
  physics_body body_a = manifold->body_a;
  physics_body body_b = manifold->body_b;
  if ((body_a == NULL) || (body_b == NULL)) {
    return;
  }
  vec2 normal = vec2_subtract(&body_b->position, &body_a->position);
  float dist_sqr = vec2_len_sqr(&normal);
  float radius = body_a->shape.radius + body_b->shape.radius;
  if (dist_sqr >= radius * radius) {
    manifold->contacts_count = 0;
    return;
  }
  float distance = sqrtf(dist_sqr);
  manifold->contacts_count = 1;
  if (distance == 0.0f) {
    manifold->penetration = body_a->shape.radius;
    manifold->normal = (vec2){ 1.0f, 0.0f };
    manifold->contacts[0] = body_a->position;
  }
  else {
    manifold->penetration = radius - distance;
    manifold->normal = (vec2){ normal.x/distance, normal.y/distance };
    manifold->contacts[0] = (vec2){ manifold->normal.x * body_a->shape.radius + body_a->position.x, manifold->normal.y * body_a->shape.radius + body_a->position.y };
  }
  if (!body_a->is_grounded) {
    body_a->is_grounded = (manifold->normal.y < 0);
  }
}

static void solve_circle_to_polygon(physics_manifold manifold) {
  physics_body body_a = manifold->body_a;
  physics_body body_b = manifold->body_b;
  if ((body_a == NULL) || (body_b == NULL)) {
    return;
  }
  solve_different_shapes(manifold, body_a, body_b);
}

static void solve_polygon_to_circle(physics_manifold manifold) {
  physics_body body_a = manifold->body_a;
  physics_body body_b = manifold->body_b;
  if ((body_a == NULL) || (body_b == NULL)) {
    return;
  }
  solve_different_shapes(manifold, body_b, body_a);
  manifold->normal.x *= -1.0f;
  manifold->normal.y *= -1.0f;
}

static void solve_different_shapes(physics_manifold manifold, physics_body body_a, physics_body body_b) {
  manifold->contacts_count = 0;
  vec2 center = body_a->position;
  mat2 transposed_mat = mat2_transpose(&body_b->shape.transform);
  vec2 scale_vec2 = vec2_subtract(&center, &body_b->position);
  center = mat2_multiply_vec2(&transposed_mat, &scale_vec2);
  float separation = -KHGPHY_FLT_MAX;
  int faceNormal = 0;
  polygon_data vertex_data = body_b->shape.vertex_data;
  for (int i = 0; i < vertex_data.vertex_count; i++) {
    vec2 offsets = vec2_subtract(&center, &vertex_data.positions[i]);
    float current_separation = vec2_dot(&vertex_data.normals[i], &offsets);
    if (current_separation > body_a->shape.radius) {
      return;
    }
    if (current_separation > separation) {
      separation = current_separation;
      faceNormal = i;
    }
  }
  vec2 v1 = vertex_data.positions[faceNormal];
  int nextIndex = (((faceNormal + 1) < vertex_data.vertex_count) ? (faceNormal + 1) : 0);
  vec2 v2 = vertex_data.positions[nextIndex];
  if (separation < KHGPHY_EPSILON) {
    manifold->contacts_count = 1;
    vec2 normal = mat2_multiply_vec2(&body_b->shape.transform, &vertex_data.normals[faceNormal]);
    manifold->normal = (vec2){ -normal.x, -normal.y };
    manifold->contacts[0] = (vec2){ manifold->normal.x * body_a->shape.radius + body_a->position.x, manifold->normal.y * body_a->shape.radius + body_a->position.y };
    manifold->penetration = body_a->shape.radius;
    return;
  }
  vec2 diff1 = vec2_subtract(&center, &v1);
  vec2 diff2 = vec2_subtract(&center, &v2);
  vec2 diff3 = vec2_subtract(&v2, &v1);
  float dot1 = vec2_dot(&diff1, &diff3);
  float dot2 = vec2_dot(&diff2, &diff3);
  manifold->penetration = body_a->shape.radius - separation;
  if (dot1 <= 0.0f) {
    if (vec2_dist_sqr(&center, &v1) > body_a->shape.radius * body_a->shape.radius) {
      return;
    }
    manifold->contacts_count = 1;
    vec2 normal = vec2_subtract(&v1, &center);
    normal = mat2_multiply_vec2(&body_b->shape.transform, &normal);
    normal = vec2_normalize(&normal);
    manifold->normal = normal;
    v1 = mat2_multiply_vec2(&body_b->shape.transform, &v1);
    v1 = vec2_add(&v1, &body_b->position);
    manifold->contacts[0] = v1;
  }
  else if (dot2 <= 0.0f) {
    if (vec2_dist_sqr(&center, &v2) > body_a->shape.radius * body_a->shape.radius) {
      return;
    }
    manifold->contacts_count = 1;
    vec2 normal = vec2_subtract(&v2, &center);
    v2 = mat2_multiply_vec2(&body_b->shape.transform, &v2);
    v2 = vec2_add(&v2, &body_b->position);
    manifold->contacts[0] = v2;
    normal = mat2_multiply_vec2(&body_b->shape.transform, &normal);
    normal = vec2_normalize(&normal);
    manifold->normal = normal;
  }
  else {
    vec2 normal = vertex_data.normals[faceNormal];
    vec2 dist = vec2_subtract(&center, &v1);
    if (vec2_dot(&dist, &normal) > body_a->shape.radius) {
      return;
    }
    normal = mat2_multiply_vec2(&body_b->shape.transform, &normal);
    manifold->normal = (vec2){ -normal.x, -normal.y };
    manifold->contacts[0] = (vec2){ manifold->normal.x * body_a->shape.radius + body_a->position.x, manifold->normal.y * body_a->shape.radius + body_a->position.y };
    manifold->contacts_count = 1;
  }
}

static void solve_polygon_to_polygon(physics_manifold manifold) {
  if ((manifold->body_a == NULL) || (manifold->body_b == NULL)) {
    return;
  }
  physics_shape body_a = manifold->body_a->shape;
  physics_shape body_b = manifold->body_b->shape;
  manifold->contacts_count = 0;
  int face_a = 0;
  float penetration_a = find_axis_least_penetration(&face_a, body_a, body_b);
  if (penetration_a >= 0.0f) {
    return;
  }
  int face_b = 0;
  float penetration_b = find_axis_least_penetration(&face_b, body_b, body_a);
  if (penetration_b >= 0.0f) {
    return;
  }
  int reference_index = 0;
  bool flip = false;
  physics_shape ref_poly;
  physics_shape inc_poly;
  if (bias_greater_than(penetration_a, penetration_b)) {
    ref_poly = body_a;
    inc_poly = body_b;
    reference_index = face_a;
  }
  else {
    ref_poly = body_b;
    inc_poly = body_a;
    reference_index = face_b;
    flip = true;
  }
  vec2 incident_face[2];
  find_incident_face(&incident_face[0], &incident_face[1], ref_poly, inc_poly, reference_index);
  polygon_data ref_data = ref_poly.vertex_data;
  vec2 v1 = ref_data.positions[reference_index];
  reference_index = (((reference_index + 1) < ref_data.vertex_count) ? (reference_index + 1) : 0);
  vec2 v2 = ref_data.positions[reference_index];
  v1 = mat2_multiply_vec2(&ref_poly.transform, &v1);
  v1 = vec2_add(&v1, &ref_poly.body->position);
  v2 = mat2_multiply_vec2(&ref_poly.transform, &v2);
  v2 = vec2_add(&v2, &ref_poly.body->position);
  vec2 side_plane_normal = vec2_subtract(&v2, &v1);
  side_plane_normal = vec2_normalize(&side_plane_normal);
  vec2 ref_face_normal = { side_plane_normal.y, -side_plane_normal.x };
  float ref_c = vec2_dot(&ref_face_normal, &v1);
  float neg_side = vec2_dot(&side_plane_normal, &v1) * -1;
  float pos_side = vec2_dot(&side_plane_normal, &v2);
  if (clip((vec2){ -side_plane_normal.x, -side_plane_normal.y }, neg_side, &incident_face[0], &incident_face[1]) < 2) {
    return;
  }
  if (clip(side_plane_normal, pos_side, &incident_face[0], &incident_face[1]) < 2) {
    return;
  }
  manifold->normal = (flip ? (vec2){ -ref_face_normal.x , ref_face_normal.y } : ref_face_normal);
  int current_point = 0;
  float separation = vec2_dot(&ref_face_normal, &incident_face[0]) - ref_c;
  if (separation <= 0.0f) {
    manifold->contacts[current_point] = incident_face[0];
    manifold->penetration = -separation;
    current_point++;
  }
  else {
    manifold->penetration = 0.0f;
  }
  separation = vec2_dot(&ref_face_normal, &incident_face[1]) - ref_c;
  if (separation <= 0.0f) {
    manifold->contacts[current_point] = incident_face[1];
    manifold->penetration += -separation;
    current_point++;
    manifold->penetration /= current_point;
  }
  manifold->contacts_count = current_point;
}

static void integrate_physics_forces(physics_body body) {
  if ((body == NULL) || (body->inverse_mass == 0.0f) || !body->enabled) {
    return;
  }
  body->velocity.x += (body->force.x * body->inverse_mass) * (delta_time / 2.0);
  body->velocity.y += (body->force.y * body->inverse_mass) * (delta_time / 2.0);
  if (body->use_gravity) {
    body->velocity.x += gravity_force.x * (delta_time / 1000 / 2.0);
    body->velocity.y += gravity_force.y * (delta_time / 1000 / 2.0);
  }
  if (!body->freeze_orient) {
    body->angular_velocity += body->torque * body->inverse_inertia * (delta_time / 2.0);
  }
}
