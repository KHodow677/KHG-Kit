#include "entity/comp_animator.h"
#include "entity/comp_commander.h"
#include "entity/comp_copier.h"
#include "entity/comp_damage.h"
#include "entity/comp_destroyer.h"
#include "entity/comp_health.h"
#include "entity/comp_life_taker.h"
#include "entity/comp_mover.h"
#include "entity/comp_physics.h"
#include "entity/comp_renderer.h"
#include "entity/comp_rotator.h"
#include "entity/comp_selector.h"
#include "entity/comp_shooter.h"
#include "entity/comp_spawn.h"
#include "entity/comp_status.h"
#include "entity/comp_stream_spawner.h"
#include "entity/comp_targeter.h"

void generate_physics_box(ecs_id eid, comp_physics *info, bool collides, float width, float height, float mass, phy_vect pos, float ang, phy_vect cog, uint32_t category);
void generate_physics_circle(ecs_id eid, comp_physics *info, bool collides, float radius, float mass, phy_vect pos, float ang, phy_vect cog, uint32_t category);
void generate_physics_pivot(ecs_id eid, comp_physics *info, comp_physics *p_info, bool collides, float width, float height, float mass, phy_vect pos, float ang, phy_vect cog, uint32_t category);
void generate_static_physics_circle(ecs_id eid, comp_physics *info, bool collides, float radius, phy_vect pos, float ang, phy_vect cog, uint32_t category);
void generate_static_physics_box(ecs_id eid, comp_physics *info, bool collides, float width, float height, phy_vect pos, float ang, phy_vect cog, uint32_t category);
void free_physics(comp_physics *info, bool has_constraint);

void generate_renderer(comp_renderer *info, comp_renderer *r_info, comp_physics *p_info, int tex_id, int render_layer);
void generate_static_renderer_segments(comp_renderer *info, comp_renderer *r_info, comp_physics *p_info, phy_vect pos, int tex_id, int render_layer, float angle);
void free_renderer(comp_renderer *info);

void generate_destroyer(comp_destroyer *info);

void generate_animator(comp_animator *info, int min_tex_id, int max_tex_id, float frame_duration, bool destroy_on_max);

void generate_mover(comp_mover *info, comp_physics *p_info, float max_vel, float max_ang_vel, phy_vect *init_path, int init_path_length);
void free_mover(comp_mover *info);

void generate_rotator(comp_rotator *info, comp_physics *p_info, float init_ang);

void generate_shooter(comp_shooter *info, float barrel_length, float cooldown);

void generate_selector(comp_selector *info, int tex_id, int linked_tex_id, int selected_tex_id, int selected_linked_tex_id);

void generate_spawn(comp_spawn *info, phy_vect pos, phy_vect linked_pos);

void generate_stream_spawner(comp_stream_spawner *info, float spawn_cooldown, float update_cooldown, phy_vect spawn_offset, bool spawn_infinitely);
void free_stream_spawner(comp_stream_spawner *info);

void generate_commander(comp_commander *info, comp_mover *m_info);

void generate_targeter(comp_targeter *info, comp_physics *body_p_info, comp_physics *targeting_p_info, float range);
void free_targeter(comp_targeter *info);

void generate_health(comp_health *info, comp_physics *p_info, float max_health, float starting_health);

void generate_damage(comp_damage *info, float damage);

void generate_copier(comp_copier *info, comp_physics *ref);

void generate_status(comp_status *info, float bar_width, float bar_height, bool tracks_ammo, bool tracks_fuel, bool tracks_health);

void generate_life_taker(comp_life_taker *info, int num_lives);

