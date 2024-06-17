#version 330
precision highp float;
out vec4 color;
in vec4 v_color;
in vec2 v_texture;
uniform sampler2D u_sampler;
void main() {
    color = v_color * texture2D(u_sampler, v_texture);
}
