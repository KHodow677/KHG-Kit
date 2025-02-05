#version 450 core
layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec2 a_tex_coord;
out vec2 TexCoord;
uniform vec2 u_screen_size;
uniform vec2 u_target_size;
void main() {
  float screen_aspect = u_screen_size.x / u_screen_size.y;
  float target_aspect = u_target_size.x / u_target_size.y;
  vec2 scale = vec2(1.0);
  if (screen_aspect > target_aspect) {
    scale.x = target_aspect / screen_aspect;
  } 
  else {
    scale.y = screen_aspect / target_aspect;
  }
  gl_Position = vec4(a_pos * scale, 0.0, 1.0);
  TexCoord = a_tex_coord;
}
