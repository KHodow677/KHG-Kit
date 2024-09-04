#include "entity/comp_animator.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_follower.h"
#include "entity/comp_mover.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_rotator.h"
#include "entity/comp_shooter.h"

void generate_physics_box(physics_info *info, bool collides, float width, float height, float mass, phy_vect pos, float ang, phy_vect cog);
void free_physics(physics_info *info);

void generate_renderer(renderer_info *info, physics_info *p_info, int tex_id);

void generate_follower(follower_info *info, physics_info *p_info, phy_body *target_body, bool move_enabled, bool rotate_enabled);
void free_follower(follower_info *info);

void generate_destroyer(destroyer_info *info);

void generate_animator(animator_info *info, int min_tex_id, int max_tex_id, float frame_duration, bool destroy_on_max);

void generate_mover(mover_info *info, physics_info *p_info);

void generate_rotator(rotator_info *info, physics_info *p_info);

void generate_shooter(shooter_info *info);

