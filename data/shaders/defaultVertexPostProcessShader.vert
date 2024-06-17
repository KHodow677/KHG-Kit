#version 330
precision highp float;
in vec2 quad_positions;
out vec2 v_positions;\n
out vec2 v_texture;
out vec4 v_color;
void main() {
  gl_Position = vec4(quad_positions, 0, 1);
  v_positions = gl_Position.xy;
  v_color = vec4(1,1,1,1);
  v_texture = (gl_Position.xy + vec2(1))/2.f;
}
