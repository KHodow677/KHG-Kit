#include "khg_2d/particles.h"
#include "khg_2d/utils.h"
#include "khg_math/math.h"
#include <math.h>
#include <stdlib.h>

shader default_particle_shader = { 0 };
char *default_particle_vertex_shader = "";
char *default_particle_fragment_shader = "";

void swap(float *a, float *b) {
  float temp = *a;
  *a = *b;
  *b = temp;
}

float randFloatRange(float min, float max) {
  return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

float randParticleSystem(vec2 v) {
  if (v.x > v.y) {
    swap(&v.x, &v.y);
  }
  return randFloatRange(v.x, v.y);
}

float interpolate(float a, float b, float perc) {
  return a * perc + b * (1 - perc);
}

void init_khg2d_particle_system(void) {
  default_particle_vertex_shader = load_file_contents("./res/shaders/defaultParticleVertexShader.vert");
  default_particle_fragment_shader = load_file_contents("./res/shaders/defaultParticleFragmentShader.frag");
  default_particle_shader = create_shader(default_particle_vertex_shader, default_particle_fragment_shader);
}

void cleanup_khg2d_particle_system(void){
  glDeleteShader(default_particle_shader.id);
}

void init_particle_system(particle_system *ps, int size) {
	cleanup_particle_system(ps);
	size += 4 - (size % 4);
	ps->size = size;
	int size_32_aligned = size + (4 - (size % 4));
  ps->pos_x = (float *)malloc(size_32_aligned * sizeof(float));
  ps->pos_y = (float *)malloc(size_32_aligned * sizeof(float));
  ps->direction_x = (float *)malloc(size_32_aligned * sizeof(float));
  ps->direction_y = (float *)malloc(size_32_aligned * sizeof(float));
  ps->rotation = (float *)malloc(size_32_aligned * sizeof(float));
  ps->size_xy = (float *)malloc(size_32_aligned * sizeof(float));
  ps->drag_x = (float *)malloc(size_32_aligned * sizeof(float));
  ps->drag_y = (float *)malloc(size_32_aligned * sizeof(float));
  ps->duration = (float *)malloc(size_32_aligned * sizeof(float));
  ps->duration_total = (float *)malloc(size_32_aligned * sizeof(float));
  ps->color = (vec4 *)malloc(size * sizeof(vec4));
  ps->rotation_speed = (float *)malloc(size_32_aligned * sizeof(float));
  ps->rotation_drag = (float *)malloc(size_32_aligned * sizeof(float));
  ps->death_rattle = (particle_settings **)malloc(size_32_aligned * sizeof(particle_settings *));
  ps->this_particle_settings = (particle_settings **)malloc(size_32_aligned * sizeof(particle_settings *));
  ps->emit_particle = (particle_settings **)malloc(size_32_aligned * sizeof(particle_settings *));
  ps->transition_type = (char *)malloc(size_32_aligned * sizeof(char));
  ps->textures = (texture **)malloc(size_32_aligned * sizeof(texture *));
  ps->emit_time = (float *)malloc(size_32_aligned * sizeof(float));
	for (int i = 0; i < size; i++) {
	  ps->duration[i] = 0;
		ps->size_xy[i] = 0;
		ps->death_rattle[i] = 0;
		ps->textures[i] = NULL;
		ps->this_particle_settings[i] = NULL;
		ps->emit_particle[i] = NULL;
	}
	create_framebuffer(&ps->fb, 100, 100);
}

void cleanup_particle_system(particle_system *ps) {
  free(ps->pos_x);
  free(ps->pos_y);
  free(ps->direction_x);
  free(ps->direction_y);
  free(ps->rotation);
  free(ps->size_xy);
  free(ps->drag_x);
  free(ps->drag_y);
  free(ps->duration);
  free(ps->duration_total);
  free(ps->color);
  free(ps->rotation_speed);
  free(ps->rotation_drag);
  free(ps->emit_time);
  free(ps->transition_type);
  free(ps->death_rattle);
  free(ps->this_particle_settings);
  free(ps->emit_particle);
  free(ps->textures);
	ps->pos_x = 0;
	ps->pos_y = 0;
	ps->direction_x = 0;
	ps->direction_y = 0;
	ps->rotation = 0;
	ps->size_xy = 0;
	ps->drag_x = 0;
	ps->drag_y = 0;
	ps->duration = 0;
	ps->duration_total = 0;
	ps->color = 0;
	ps->rotation_speed = 0;
	ps->rotation_drag = 0;
	ps->emit_time = 0;
	ps->transition_type = 0;
	ps->death_rattle = 0;
	ps->this_particle_settings = 0;
	ps->emit_particle = 0;
	ps->textures = 0;
	ps->size = 0;
  cleanup_framebuffer(&ps->fb);
}

void emit_particle_wave(particle_system *ps, particle_settings *p_settings, vec2 pos) {
  int recreated_particles_this_frame = 0;
  for (int i = 0; i < ps->size; i++) {
    if (recreated_particles_this_frame < p_settings->on_create_count && ps->size_xy[i] == 0) {
      ps->duration[i] = randParticleSystem(p_settings->particle_life_time);
      ps->duration_total[i] = ps->duration[i];
      ps->pos_x[i] = pos.x + randParticleSystem(p_settings->position_x);
      ps->pos_y[i] = pos.y + randParticleSystem(p_settings->position_y);
      ps->direction_x[i] = randParticleSystem(p_settings->direction_x);
      ps->direction_y[i] = randParticleSystem(p_settings->direction_y);
      ps->rotation[i] = randParticleSystem(p_settings->rotation);
      ps->size_xy[i] = randParticleSystem(p_settings->create_appearance.size);
      ps->drag_x[i] = randParticleSystem(p_settings->drag_x);
      ps->drag_y[i] = randParticleSystem(p_settings->drag_y);
      ps->color[i].x = randParticleSystem((vec2){ p_settings->create_appearance.color_1.x, p_settings->create_appearance.color_2.x });
      ps->color[i].y = randParticleSystem((vec2){ p_settings->create_appearance.color_1.y, p_settings->create_appearance.color_2.y });
      ps->color[i].z = randParticleSystem((vec2){ p_settings->create_appearance.color_1.z, p_settings->create_appearance.color_2.z });
      ps->color[i].w = randParticleSystem((vec2){ p_settings->create_appearance.color_1.w, p_settings->create_appearance.color_2.w });
      ps->rotation_speed[i] = randParticleSystem(p_settings->rotation_speed);
      ps->rotation_drag[i] = randParticleSystem(p_settings->rotation_drag);
      ps->textures[i] = p_settings->texture_ptr;
      ps->death_rattle[i] = p_settings->death_rattle;
      ps->transition_type[i] = p_settings->transition_type;
      ps->this_particle_settings[i] = p_settings;
      ps->emit_particle[i] = p_settings->sub_emit_particle;
      ps->emit_time[i] = randParticleSystem(ps->this_particle_settings[i]->sub_emit_particle_time);
      recreated_particles_this_frame++;
    }
  }
}

void apply_movement(particle_system *ps, float delta_time) {
  for (int i = 0; i < ps->size; i++) {
    if (ps->duration[i] > 0) {
      ps->duration[i] -= delta_time;
    }
    if (ps->emit_time[i] > 0 && ps->emit_particle[i]) {
      ps->emit_time[i] -= delta_time;
    }
    if (ps->duration[i] <= 0) {
      if (ps->death_rattle[i] != NULL && ps->death_rattle[i]->on_create_count) {
        emit_particle_wave(ps, ps->death_rattle[i], (vec2){ ps->pos_x[i], ps->pos_y[i] });
      }
      ps->death_rattle[i] = NULL;
      ps->duration[i] = 0;
      ps->size_xy[i] = 0;
      ps->emit_particle[i] = NULL;
    }
    else if (ps->emit_time[i] <= 0 && ps->emit_particle[i]) {
      ps->emit_time[i] = randParticleSystem(ps->this_particle_settings[i]->sub_emit_particle_time);
      emit_particle_wave(ps, ps->emit_particle[i], (vec2){ ps->pos_x[i], ps->pos_y[i] });
    }
  }
	for (int i = 0; i < ps->size; i++) {
		ps->pos_x[i] += delta_time * ps->direction_x[i];
	}
	for (int i = 0; i < ps->size; i++) {
		ps->pos_y[i] += delta_time * ps->direction_y[i];
	}
	for (int i = 0; i < ps->size; i++) {
		ps->rotation[i] += delta_time * ps->rotation_speed[i];
	}
}

void draw(particle_system *ps, renderer_2d *r2d) {
  unsigned int w = r2d->window_w;
  unsigned int h = r2d->window_h;
  camera cam = r2d->current_camera;
  if (ps->post_processing) {
    vec2 tex_size;
    flush(r2d, true);
    tex_size = get_texture_size(&ps->fb.texture);
    if (tex_size.x != (w / ps->pixelate_factor) || tex_size.y != (w / ps->pixelate_factor)) {
      resize_framebuffer(&ps->fb, w / ps->pixelate_factor, h / ps->pixelate_factor);
    }
    update_window_metrics(r2d, w / ps->pixelate_factor, h / ps->pixelate_factor);
  }
  for (int i = 0; i < ps->size; i++) {
    vec4 pos, c, p;
    if (ps->size_xy[i] == 0) {
      continue;
    }
    float life_perc = ps->duration[i] / ps->duration_total[i];
    switch (ps->transition_type[i]) {
      case none:
        life_perc = 1;
        break;
      case linear:
        break;
      case curve:
        life_perc *= life_perc;
        break;
      case abrupt_curve:
        life_perc *= life_perc * life_perc;
        break;
      case wave:
        life_perc = (cos(life_perc * 5.0f * PI) * life_perc + life_perc) * 2.0f;
        break;
      case wave_2:
        life_perc = cos(life_perc * 5.0f * PI) * sqrt(life_perc) * 0.9f * 0.1f;
        break;
  		case delay:
		    life_perc = (cos(life_perc * PI * 2) * sin(life_perc * life_perc)) / 2.f;
        break;
      case delay_2:
        life_perc = atan(2 * life_perc * life_perc * life_perc * PI) / 2.f;
        break;
		  default:
        break;
    }
    if (ps->this_particle_settings[i]) {
      pos = (vec4){ 
        ps->pos_x[i], 
        ps->pos_y[i], 
        interpolate(ps->size_xy[i], ps->this_particle_settings[i]->create_appearance.size.x, life_perc),
        pos.z 
      };
      c = (vec4){
        interpolate(ps->color[i].x, ps->this_particle_settings[i]->create_appearance.color_1.x, life_perc),
        interpolate(ps->color[i].y, ps->this_particle_settings[i]->create_appearance.color_1.y, life_perc),
        interpolate(ps->color[i].z, ps->this_particle_settings[i]->create_appearance.color_1.z, life_perc),
        interpolate(ps->color[i].w, ps->this_particle_settings[i]->create_appearance.color_1.w, life_perc)
      };
    }
    else {
      pos = (vec4){ ps->pos_x[i], ps->pos_y[i], ps->size_xy[i], pos.z };
      c = (vec4) { ps->color[i].x, ps->color[i].y, ps->color[i].z, ps->color[i].w };
    }
    if (ps->post_processing) {
      r2d->current_camera = cam;
      p = vec4_multiply_num_on_vec4(1.0f / ps->pixelate_factor, &pos);
      p.x -= r2d->current_camera.position.x / ps->pixelate_factor;
      p.y -= r2d->current_camera.position.y / ps->pixelate_factor;
      r2d->current_camera.position = (vec2){ 0.0f, 0.0f };
    }
    else {
      p = pos;
    }
    if (ps->textures[i] != NULL) {
      vec2 origin = { 0.0f, 0.0f };
      vec4 c_data[4] = { c, c, c, c };
      render_rectangle_texture(r2d, p, *ps->textures[i], c_data, origin, ps->rotation[i], default_texture_coords);
    }
    else {
      vec2 origin = { 0.0f, 0.0f };
      vec4 c_data[4] = { c, c, c, c };
      render_rectangle(r2d, p, c_data, origin, ps->rotation[i]);
    }
    if (ps->post_processing) {
      vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
      vec4 c_data[4] = { color, color, color, color };
      vec2 origin = { 0.0f, 0.0f };
      clear_framebuffer(&ps->fb);
      flush_fbo(r2d, ps->fb, true);
      update_window_metrics(r2d, w, h);
      set_default(&r2d->current_camera);
      shader s = r2d->current_shader;
      vec4 transform = { 0.0f, 0.0f, w, h };
      render_rectangle_texture(r2d, transform, ps->fb.texture, c_data, origin, 0.0f, default_texture_coords);
      set_shader(r2d, default_particle_shader);
      flush(r2d, true);
      set_shader(r2d, s);
    }
    r2d->current_camera = cam;
  }
}

