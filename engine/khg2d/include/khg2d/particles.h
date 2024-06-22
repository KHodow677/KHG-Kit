#pragma once

#include "khgmath/vec2.h"
#include "khgmath/vec4.h"
#include "framebuffer.h"
#include "renderer2d.h"
#include "shader.h"
#include "texture.h"

extern shader default_particle_shader;
extern char *default_particle_vertex_shader;
extern char *default_particle_fragment_shader; 

void init_khg2d_particle_system(void);
void cleanup_khg2d_particle_system(void);

typedef struct {
  vec2 size;
  vec4 color_1;
  vec4 color_2;
} particle_appearance;

typedef enum {
  none = 0,
  linear,
  curve,
  abrupt_curve,
  wave,
  wave_2,
  delay,
  delay_2
} transition_types; 

typedef struct particle_settings particle_settings;

struct particle_settings {
  particle_settings *death_rattle;
  particle_settings *sub_emit_particle;
  int on_create_count;
  vec2 sub_emit_particle_time;
  vec2 position_x;
  vec2 position_y;
  vec2 particle_life_time;
  vec2 direction_x;
  vec2 direction_y;
  vec2 drag_x;
  vec2 drag_y;
  vec2 rotation;
  vec2 rotation_speed;
  vec2 rotation_drag;
  particle_appearance create_appearance;
  particle_appearance create_end_appearance;
  texture *texture_ptr;
  int transition_type;
};

typedef struct {
  bool post_processing;
	float pixelate_factor;
  int size;
  float *pos_x;
  float *pos_y;
  float *direction_x;
  float *direction_y;
  float *rotation;
  float *size_xy;
  float *drag_x;
  float *drag_y;
  float *duration;
  float *duration_total;
  vec4 *color;
  float *rotation_speed;
  float *rotation_drag;
  float *emit_time;
  char *transition_type;
  particle_settings **death_rattle;
  particle_settings **this_particle_settings;
  particle_settings **emit_particle;
  texture **textures;
  framebuffer fb;
} particle_system;

void init_particle_system(particle_system *ps, int size);
void cleanup_particle_system(particle_system *ps);
void emit_particle_wave(particle_system *ps, particle_settings *p_settings, vec2 pos);
void apply_movement(particle_system *ps, float delta_time);
void draw(particle_system *ps, renderer_2d *r);


