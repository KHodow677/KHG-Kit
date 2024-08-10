#include "entity/comp_follower.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"

void generate_physics_box(physics_info *info, cpSpace *sp, bool collides, float width, float height, float mass, cpVect pos, cpFloat ang, cpVect cog);
void free_physics(physics_info *info);

void generate_renderer(renderer_info *info, physics_info *p_info, char *file_name, char *file_type);
void free_renderer(renderer_info *info);

void generate_follower(follower_info *info, physics_info *p_info, physics_info *target_p_info, bool follow_pos, bool follow_ang);

