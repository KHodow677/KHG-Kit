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
float rounded_box_sdf(vec2 center_pos, vec2 size, float radius) {
  return length(max(abs(center_pos) - size + radius, 0.0)) - radius;
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
  if (v_corner_radius != 0.0f) {
    display_color = opaque_color;
    vec2 location = vec2(v_pos_px.x, -v_pos_px.y);
    location.y += u_screen_size.y - size.y;
    float edge_softness = 1.0f;
    float radius = v_corner_radius * 2.0f;
    float distance = rounded_box_sdf(gl_FragCoord.xy - location - (size/2.0f), size / 2.0f, radius);
    float smoothed_alpha = 1.0f-smoothstep(0.0f, edge_softness * 2.0f,distance);
    vec3 fill_color;
    if (v_border_width != 0.0f) {
      vec2 location_border = vec2(location.x + v_border_width, location.y + v_border_width);
      vec2 size_border = vec2(size.x - v_border_width * 2, size.y - v_border_width * 2);
      float distance_border = rounded_box_sdf(gl_FragCoord.xy - location_border - (size_border / 2.0f), size_border / 2.0f, radius);
      if(distance_border <= 0.0f) {
        fill_color = display_color.xyz;
      }\
      else {
        fill_color = v_border_color.xyz;
      }
    }\
    else {
      fill_color = display_color.xyz;
    }
    if (v_border_width != 0.0f) {\
      o_color =  mix(vec4(0.0f, 0.0f, 0.0f, 0.0f), vec4(fill_color, smoothed_alpha), smoothed_alpha);
    }
    else {\
      o_color = mix(vec4(0.0f, 0.0f, 0.0f, 0.0f), vec4(fill_color, display_color.a), smoothed_alpha);
    }
  }
  else {
    vec4 fill_color = opaque_color;
    if (v_border_width != 0.0f) {
      vec2 location = vec2(v_pos_px.x, -v_pos_px.y);
      location.y += u_screen_size.y - size.y;
      vec2 location_border = vec2(location.x + v_border_width, location.y + v_border_width);
      vec2 size_border = vec2(v_scale.x - v_border_width * 2, v_scale.y - v_border_width * 2);
      float distance_border = rounded_box_sdf(gl_FragCoord.xy - location_border - (size_border / 2.0f), size_border / 2.0f, v_corner_radius);
      if (distance_border > 0.0f) {
        fill_color = v_border_color;
      }
    }
    o_color = fill_color;
  }
}
