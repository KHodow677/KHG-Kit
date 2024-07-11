#include "khg_phy/module.h"
#include "khg_phy/module_internal.h"
#include <math.h>
#include <stdlib.h>

static vec2 get_support(physics_shape shape, vec2 dir) {
  float bestProjection = -KHGPHY_FLT_MAX;
  vec2 bestVertex = { 0.0f, 0.0f };
  polygon_data data = shape.vertex_data;
  for (int i = 0; i < data.vertex_count; i++) {
    vec2 vertex = data.positions[i];
    float projection = MathDot(vertex, dir);
    if (projection > bestProjection) {
      bestVertex = vertex;
      bestProjection = projection;
    }
  }
  return bestVertex;
}

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

static void initialize_physics_manifolds(physics_manifold manifold) {
  physics_body body_a = manifold->body_a;
  physics_body body_b = manifold->body_b;
  if ((body_a == NULL) || (body_b == NULL)) {
    return;
  }
  manifold->restitution = sqrtf(body_a->restitution * body_b->restitution);
  manifold->static_friction = sqrtf(body_a->static_friction * body_b->static_friction);
  manifold->dynamic_friction = sqrtf(body_a->dynamic_friction * body_b->dynamic_friction);
  for (int i = 0; i < manifold->contacts_count; i++) {
    vec2 radius_a = vec2_subtract(&manifold->contacts[i], &body_a->position);
    vec2 radius_b = vec2_subtract(&manifold->contacts[i], &body_b->position);
    vec2 cross_a = vec2_cross(body_a->angular_velocity, &radius_a);
    vec2 cross_b = vec2_cross(body_b->angular_velocity, &radius_b);
    vec2 radius_v = { 0.0f, 0.0f };
    radius_v.x = body_b->velocity.x + cross_b.x - body_a->velocity.x - cross_a.x;
    radius_v.y = body_b->velocity.y + cross_b.y - body_a->velocity.y - cross_a.y;
    vec2 gravity_val = (vec2){ gravity_force.x * delta_time / 1000, gravity_force.y * delta_time / 1000 };
    if (vec2_len_sqr(&radius_v) < (vec2_len_sqr(&gravity_val) + KHGPHY_EPSILON)) {
      manifold->restitution = 0;
    }
  }
}

static void integrate_physics_inpulses(physics_manifold manifold) {
  physics_body body_a = manifold->body_a;
  physics_body body_b = manifold->body_b;
  if ((body_a == NULL) || (body_b == NULL)) {
    return;
  }
  if (fabs(body_a->inverse_mass + body_b->inverse_mass) <= KHGPHY_EPSILON) {
    body_a->velocity = KHGPHY_VECTOR_0;
    body_b->velocity = KHGPHY_VECTOR_0;
    return;
  }
  for (int i = 0; i < manifold->contacts_count; i++) {
    vec2 radius_a = vec2_subtract(&manifold->contacts[i], &body_a->position);
    vec2 radius_b = vec2_subtract(&manifold->contacts[i], &body_b->position);
    vec2 radius_v = { 0.0f, 0.0f };
    radius_v.x = body_b->velocity.x + vec2_cross(body_b->angular_velocity, &radius_b).x - body_a->velocity.x - vec2_cross(body_a->angular_velocity, &radius_a).x;
    radius_v.y = body_b->velocity.y + vec2_cross(body_b->angular_velocity, &radius_b).y - body_a->velocity.y - vec2_cross(body_a->angular_velocity, &radius_a).y;
    float contact_velocity = vec2_dot(&radius_v, &manifold->normal);
    if (contact_velocity > 0.0f) {
      return;
    }
    float ra_cross_n = vec2_cross_vec2(&radius_a, &manifold->normal);
    float rb_cross_n = vec2_cross_vec2(&radius_b, &manifold->normal);
    float inverse_mass_sum = body_a->inverse_mass + body_b->inverse_mass + (ra_cross_n * ra_cross_n) * body_a->inverse_inertia + (rb_cross_n * rb_cross_n) * body_b->inverse_inertia;
    float impulse = -(1.0f + manifold->restitution) * contact_velocity;
    impulse /= inverse_mass_sum;
    impulse /= (float)manifold->contacts_count;
    vec2 impulse_v = { manifold->normal.x * impulse, manifold->normal.y * impulse };
    if (body_a->enabled) {
      body_a->velocity.x += body_a->inverse_mass * (-impulse_v.x);
      body_a->velocity.y += body_a->inverse_mass * (-impulse_v.y);
      if (!body_a->freeze_orient) {
        vec2 impulse_vec2 = { -impulse_v.x, -impulse_v.y };
        body_a->angular_velocity += body_a->inverse_inertia * vec2_cross_vec2(&radius_a, &impulse_vec2);
      }
    }
    if (body_b->enabled) {
      body_b->velocity.x += body_b->inverse_mass * (impulse_v.x);
      body_b->velocity.y += body_b->inverse_mass * (impulse_v.y);
      if (!body_b->freeze_orient) {
        body_b->angular_velocity += body_b->inverse_inertia * vec2_cross_vec2(&radius_b, &impulse_v);
      }
    }
    radius_v.x = body_b->velocity.x + vec2_cross(body_b->angular_velocity, &radius_b).x - body_a->velocity.x - vec2_cross(body_a->angular_velocity, &radius_a).x;
    radius_v.y = body_b->velocity.y + vec2_cross(body_b->angular_velocity, &radius_b).y - body_a->velocity.y - vec2_cross(body_a->angular_velocity, &radius_a).y;
    vec2 tangent = { radius_v.x + (manifold->normal.x * vec2_dot(&radius_v, &manifold->normal)), radius_v.y - (manifold->normal.y * vec2_dot(&radius_v, &manifold->normal)) };
    tangent = vec2_normalize(&tangent);
    float impulse_tangent = -vec2_dot(&radius_v, &tangent);
    impulse_tangent /= inverse_mass_sum;
    impulse_tangent /= (float)manifold->contacts_count;
    float abs_impulse_tangent = fabs(impulse_tangent);
    if (abs_impulse_tangent <= KHGPHY_EPSILON) {
      return;
    }
    vec2 tangent_impulse = { 0.0f, 0.0f };
    if (abs_impulse_tangent < impulse * manifold->static_friction) {
      tangent_impulse = (vec2){ tangent.x * impulse_tangent, tangent.y * impulse_tangent };
    }
    else {
      tangent_impulse = (vec2){ tangent.x * impulse * manifold->dynamic_friction, tangent.y * -impulse * manifold->dynamic_friction };
    }
    if (body_a->enabled) {
      body_a->velocity.x += body_a->inverse_mass * (-tangent_impulse.x);
      body_a->velocity.y += body_a->inverse_mass * (-tangent_impulse.y);
      if (!body_a->freeze_orient) {
        vec2 tangent_impulse_vec2 = { -tangent_impulse.x, -tangent_impulse.y };
        body_a->angular_velocity += body_a->inverse_inertia * vec2_cross_vec2(&radius_a, &tangent_impulse_vec2);
      }
    }
    if (body_b->enabled) {
      body_b->velocity.x += body_b->inverse_mass * (tangent_impulse.x);
      body_b->velocity.y += body_b->inverse_mass * (tangent_impulse.y);
      if (!body_b->freeze_orient) {
        body_b->angular_velocity += body_b->inverse_inertia * vec2_cross_vec2(&radius_b, &tangent_impulse);
      }
    }
  }
}

static void integrate_physics_velocity(physics_body body) {
  if ((body == NULL) || !body->enabled) {
    return;
  }
  body->position.x += body->velocity.x * delta_time;
  body->position.y += body->velocity.y * delta_time;
  if (!body->freeze_orient) {
    body->orient += body->angular_velocity * delta_time;
  }
  mat2_set(&body->shape.transform, body->orient);
  integrate_physics_forces(body);
}

static void correct_physics_positions(physics_manifold manifold) {
  physics_body body_a = manifold->body_a;
  physics_body body_b = manifold->body_b;
  if ((body_a == NULL) || (body_b == NULL)) {
    return;
  }
  vec2 correction = { 0.f, 0.f };
  correction.x = (max(manifold->penetration - KHGPHY_PENETRATION_ALLOWANCE, 0.0f) / (body_a->inverse_mass + body_b->inverse_mass)) * manifold->normal.x * KHGPHY_PENETRATION_CORRECTION;
  correction.y = (max(manifold->penetration - KHGPHY_PENETRATION_ALLOWANCE, 0.0f) / (body_a->inverse_mass + body_b->inverse_mass)) * manifold->normal.y * KHGPHY_PENETRATION_CORRECTION;
  if (body_a->enabled) {
    body_a->position.x -= correction.x * body_a->inverse_mass;
    body_a->position.y -= correction.y * body_a->inverse_mass;
  }
  if (body_b->enabled) {
    body_b->position.x += correction.x * body_b->inverse_mass;
    body_b->position.y += correction.y * body_b->inverse_mass;
  }
}

static float find_axis_least_penetration(int *face_index, physics_shape shape_a, physics_shape shape_b) {
  float best_distance = -KHGPHY_FLT_MAX;
  int best_index = 0;
  polygon_data data_a = shape_a.vertex_data;
  for (int i = 0; i < data_a.vertex_count; i++) {
    vec2 normal = data_a.normals[i];
    vec2 trans_normal = mat2_multiply_vec2(&shape_a.transform, &normal);
    mat2 bu_t = mat2_transpose(&shape_b.transform);
    vec2 support = get_support(shape_b, (vec2){ -normal.x, -normal.y });
    vec2 vertex = data_a.positions[i];
    vertex = mat2_multiply_vec2(&shape_a.transform, &vertex);
    vertex = vec2_add(&vertex, &shape_a.body->position);
    vertex = vec2_subtract(&vertex, &shape_b.body->position);
    vertex = mat2_multiply_vec2(&bu_t, &vertex);
    vec2 support_diff = vec2_subtract(&support, &vertex);
    float distance = vec2_dot(&normal, &support_diff);
    if (distance > best_distance) {
      best_distance = distance;
      best_index = i;
    }
  }
  *face_index = best_index;
  return best_distance;
}

static void find_incident_face(vec2 *v0, vec2 *v1, physics_shape ref, physics_shape inc, int index) {
  polygon_data ref_data = ref.vertex_data;
  polygon_data inc_data = ref.vertex_data;
  vec2 reference_normal = ref_data.normals[index];
  reference_normal = mat2_multiply_vec2(&ref.transform, &reference_normal);
  mat2 inc_transform_t = mat2_transpose(&inc.transform);
  reference_normal = mat2_multiply_vec2(&inc_transform_t, &reference_normal);
  int incident_face = 0;
  float min_dot = KHGPHY_FLT_MAX;
  for (int i = 0; i < inc_data.vertex_count; i++) {
    float dot = vec2_dot(&reference_normal, &inc_data.normals[i]);
    if (dot < min_dot) {
      min_dot = dot;
      incident_face = i;
    }
  }
  *v0 = mat2_multiply_vec2(&inc.transform, &inc_data.positions[incident_face]);
  *v0 = vec2_add(v0, &inc.body->position);
  incident_face = (((incident_face + 1) < inc_data.vertex_count) ? (incident_face + 1) : 0);
  *v1 = mat2_multiply_vec2(&inc.transform, &inc_data.positions[incident_face]);
  *v1 = vec2_add(v1, &inc.body->position);
}

static int clip(vec2 normal, float clip, vec2 *face_a, vec2 *face_b) {
  int sp = 0;
  vec2 out[2] = { *face_a, *face_b };
  float distance_a = vec2_dot(&normal, face_a) - clip;
  float distance_b = vec2_dot(&normal, face_b) - clip;
  if (distance_a <= 0.0f) {
    out[sp++] = *face_a;
  }
  if (distance_b <= 0.0f) {
    out[sp++] = *face_b;
  }
  if ((distance_a * distance_b) < 0.0f) {
    float alpha = distance_a / (distance_a - distance_b);
    out[sp] = *face_a;
    vec2 delta = vec2_subtract(face_b, face_a);
    delta.x *= alpha;
    delta.y *= alpha;
    out[sp] = vec2_add(&out[sp], &delta);
    sp++;
  }
  *face_a = out[0];
  *face_b = out[1];
  return sp;
}

static bool bias_greater_than(float value_a, float value_b) {
  return (value_a >= (value_b * 0.95f + value_a * 0.01f));
}

static vec2 triangle_barycenter(vec2 v1, vec2 v2, vec2 v3) {
  vec2 result = { 0.0f, 0.f };
  result.x = (v1.x + v2.x + v3.x) / 3;
  result.y = (v1.y + v2.y + v3.y) / 3;
  return result;
}

static void init_timer(void) {
  srand(time(NULL));
  #if defined(_WIN32)
    QueryPerformanceFrequency((unsigned long long int *) &frequency);
  #elif defined(__linux__)
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) == 0) {
      frequency = 1000000000;
    }
  #elif defined(__APPLE__)
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    frequency = (timebase.denom * 1e9) / timebase.numer;
  #elif defined(EMSCRIPTEN)
    frequency = 1000;
  #endif
  base_time = get_time_count();
  start_time = get_current_time();
}

static uint64_t get_time_count(void) {
  uint64_t value = 0;
  #if defined(_WIN32)
    QueryPerformanceCounter((unsigned long long int *) &value);
  #elif defined(__linux__)
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    value = (uint64_t)now.tv_sec * (uint64_t) 1000000000 + (uint64_t)now.tv_nsec;
  #elif defined(__APPLE__)
    value = mach_absolute_time();
  #elif defined(EMSCRIPTEN)
    value = emscripten_get_now();
  #endif
  return value;
}

static double get_current_time(void) {
  return (double)(get_time_count() - base_time) / frequency * 1000;
}