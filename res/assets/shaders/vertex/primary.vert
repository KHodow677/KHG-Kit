#version 450 core
layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec4 a_border_color;
layout (location = 2) in float a_border_width;
layout (location = 3) in vec4 a_color;
layout (location = 4) in vec2 a_texcoord;
layout (location = 5) in float a_tex_index;
layout (location = 6) in vec2 a_scale;
layout (location = 7) in vec2 a_pos_px;
layout (location = 8) in float a_corner_radius;
layout (location = 10) in vec2 a_min_coord;
layout (location = 11) in vec2 a_max_coord;
uniform mat4 u_proj;
out vec4 v_border_color;
out float v_border_width;
out vec4 v_color;
out vec2 v_texcoord;
out float v_tex_index;
flat out vec2 v_scale;
flat out vec2 v_pos_px;
flat out float v_is_gradient;
out float v_corner_radius;
out vec2 v_min_coord;
out vec2 v_max_coord;
void main() {
  v_color = a_color;
  v_texcoord = a_texcoord;
  v_tex_index = a_tex_index;
  v_border_color = a_border_color;
  v_border_width = a_border_width;
  v_scale = a_scale;
  v_pos_px = a_pos_px;
  v_corner_radius = a_corner_radius;
  v_min_coord = a_min_coord;
  v_max_coord = a_max_coord;
  gl_Position = u_proj * vec4(a_pos.x, a_pos.y, 0.0f, 1.0);
}
