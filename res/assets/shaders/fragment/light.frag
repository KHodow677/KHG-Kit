#version 450 core
out vec4 o_color;
in vec4 v_color;
in float v_tex_index;
in vec4 v_border_color;
in float v_border_width;
in vec2 v_texcoord;
flat in vec2 v_scale;
flat in vec2 v_pos_px;
in float v_corner_radius;
uniform sampler2D u_textures[32];
uniform vec2 u_screen_size;
in vec2 v_min_coord;
in vec2 v_max_coord;
const int num_lights = 2;
uniform vec3 u_light_color;
uniform vec2 u_light_pos_percs[num_lights];
uniform float u_light_intensities[num_lights];
float rounded_box_sdf(vec2 center_pos, vec2 size, float radius) {
  return length(max(abs(center_pos) - size + radius,0.0)) - radius;
}
void main() {
  if (u_screen_size.y - gl_FragCoord.y < v_min_coord.y && v_min_coord.y != -1) {
    discard;
  }
  if (u_screen_size.y - gl_FragCoord.y > v_max_coord.y && v_max_coord.y != -1) {
    discard;
  }
  if ((gl_FragCoord.x < v_min_coord.x && v_min_coord.x != -1) || (gl_FragCoord.x > v_max_coord.x && v_max_coord.x != -1)) {
    discard;\
  }
  vec2 size = v_scale;
  vec4 opaque_color, display_color;
  if (v_tex_index == -1) {
    opaque_color = v_color;
  }\
  else {
    opaque_color = texture(u_textures[int(v_tex_index)], v_texcoord) * v_color;
  }
  vec4 fill_color = opaque_color;
  o_color = fill_color;
  for (int i = 0; i < num_lights; i++) {
    vec2 u_light_pos_perc = u_light_pos_percs[i];
    float u_light_intensity = u_light_intensities[i];
    vec2 light_pos = vec2(v_pos_px.x + v_scale.x * u_light_pos_perc.x, u_screen_size.y - (v_pos_px.y + v_scale.y * u_light_pos_perc.y));
    float dist_to_light = distance(gl_FragCoord.xy, light_pos);
    float intensity = clamp(1.0 / (1.0 + dist_to_light * dist_to_light / (u_light_intensity * u_light_intensity)), 0.0, 1.0);
    float smoothed_intensity = smoothstep(0.0, 1.0, pow(intensity, 1.0 / 3.2));
    float noise = fract(sin(dot(gl_FragCoord.xy, vec2(12.9898, 78.233))) * 43758.5453);
    float dithered_intensity = smoothed_intensity + noise * 0.005;
    o_color.a *= clamp(1.0 - dithered_intensity, 0.0, 1.0);
  }
}
