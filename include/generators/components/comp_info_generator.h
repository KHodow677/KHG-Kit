#include "entity/comp_animator.h"
#include "entity/comp_commander.h"
#include "entity/comp_damage.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_health.h"
#include "entity/comp_mover.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_rotator.h"
#include "entity/comp_selector.h"
#include "entity/comp_shooter.h"
#include "entity/comp_stream_spawner.h"
#include "entity/comp_targeter.h"

void generate_physics_box(ecs_id eid, physics_info *info, bool collides, float width, float height, float mass, phy_vect pos, float ang, phy_vect cog, uint32_t category);
void generate_physics_circle(ecs_id eid, physics_info *info, bool collides, float radius, float mass, phy_vect pos, float ang, phy_vect cog, uint32_t category);
void generate_physics_pivot(ecs_id eid, physics_info *info, physics_info *p_info, bool collides, float width, float height, float mass, phy_vect pos, float ang, phy_vect cog, uint32_t category);
void generate_static_physics_circle(ecs_id eid, physics_info *info, bool collides, float radius, phy_vect pos, float ang, phy_vect cog, uint32_t category);
void generate_static_physics_box(ecs_id eid, physics_info *info, bool collides, float width, float height, phy_vect pos, float ang, phy_vect cog, uint32_t category);
void free_physics(physics_info *info, bool has_constraint);

void generate_renderer(renderer_info *info, physics_info *p_info, int tex_id, int render_layer, ecs_id linked_ent);
void generate_static_renderer_segments(renderer_info *info, physics_info *p_info, phy_vect pos, int tex_id, int render_layer, ecs_id linked_ent, float angle);
void free_renderer(renderer_info *info);

void generate_destroyer(destroyer_info *info);

void generate_animator(animator_info *info, int min_tex_id, int max_tex_id, float frame_duration, bool destroy_on_max);

void generate_mover(mover_info *info, ecs_id entity, float max_vel, float max_ang_vel, phy_vect *init_path, int init_path_length);
void free_mover(mover_info *info);

void generate_rotator(rotator_info *info, physics_info *p_info, float init_ang);

void generate_shooter(shooter_info *info, float barrel_length, float cooldown);

void generate_selector(selector_info *info, int tex_id, int linked_tex_id, int selected_tex_id, int selected_linked_tex_id);

void generate_stream_spawner(stream_spawner_info *info, float spawn_cooldown, phy_vect spawn_offset);
void free_stream_spawner(stream_spawner_info *info);

void generate_commander(commander_info *info, mover_info *m_info);

void generate_targeter(targeter_info *info, physics_info *body_p_info, physics_info *targeting_p_info, float range);
void free_targeter(targeter_info *info);

void generate_health(health_info *info, float max_health, float starting_health);

void generate_damage(damage_info *info, float damage);

