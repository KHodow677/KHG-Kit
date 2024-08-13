#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_follower.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"

void generate_physics_box(physics_info *info, cpSpace *sp, bool collides, float width, float height, float mass, cpVect pos, cpFloat ang, cpVect cog);
void free_physics(physics_info *info, cpSpace *sp);

void generate_renderer(renderer_info *info, physics_info *p_info, int tex_id);

void generate_follower(follower_info *info, physics_info *p_info, physics_info *target_p_info, bool follow_pos, bool follow_ang);

void generate_destroyer(destroyer_info *info);

void generate_animator(animator_info *info, utl_vector *vec, int frame_count);
void free_animator(animator_info *info);
