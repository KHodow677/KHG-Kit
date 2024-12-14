#pragma once

#include <sys/wait.h>
extern const char *framebuffer_vert_src;
extern const char *framebuffer_frag_src;

const char *framebuffer_vert_src = 
  "#version 450 core\n"
  "layout (location = 0) in vec2 a_pos;\n"
  "layout (location = 1) in vec2 a_tex_coord;\n"
  "out vec2 TexCoord;\n"
  "void main() {\n"
    "gl_Position = vec4(a_pos, 0.0, 1.0);\n"
    "TexCoord = a_tex_coord;\n"
  "}\n";

const char *framebuffer_frag_src = 
  "#version 450 core\n"
  "out vec4 frag_color;\n"
  "in vec2 TexCoord;\n"
  "uniform sampler2D u_framebuffer_texture;\n"
  "uniform float bloom_spread = 1.0;\n"
  "uniform float bloom_intensity = 1.0;\n"
  "uniform float time;\n"
  "uniform float noise_intensity;\n"
  "float random(vec2 uv) {\n"
  "  return fract(sin(dot(uv.xy, vec2(12.9898, 78.233))) * 43758.5453);\n"
  "}\n"
  "vec4 apply_noise(vec4 color, vec2 uv) {\n"
  "  float noise = random(uv * time) * noise_intensity;\n"
  "  return vec4(color.rgb + noise, color.a);\n"
  "}\n"
  "void main() {\n"
  "  ivec2 size = textureSize(u_framebuffer_texture, 0);\n"
  "  vec4 sum = vec4(0.0);\n"
  "  for (int n = 0; n < 9; ++n) {\n"
  "    float uv_y = (TexCoord.y * size.y) + (bloom_spread * float(n - 4));\n"
  "    vec4 h_sum = vec4(0.0);\n"
  "    h_sum += texelFetch(u_framebuffer_texture, ivec2(TexCoord.x * size.x - (4.0 * bloom_spread), uv_y), 0);\n"
  "    h_sum += texelFetch(u_framebuffer_texture, ivec2(TexCoord.x * size.x - (3.0 * bloom_spread), uv_y), 0);\n"
  "    h_sum += texelFetch(u_framebuffer_texture, ivec2(TexCoord.x * size.x - (2.0 * bloom_spread), uv_y), 0);\n"
  "    h_sum += texelFetch(u_framebuffer_texture, ivec2(TexCoord.x * size.x - bloom_spread, uv_y), 0);\n"
  "    h_sum += texelFetch(u_framebuffer_texture, ivec2(TexCoord.x * size.x, uv_y), 0);\n"
  "    h_sum += texelFetch(u_framebuffer_texture, ivec2(TexCoord.x * size.x + bloom_spread, uv_y), 0);\n"
  "    h_sum += texelFetch(u_framebuffer_texture, ivec2(TexCoord.x * size.x + (2.0 * bloom_spread), uv_y), 0);\n"
  "    h_sum += texelFetch(u_framebuffer_texture, ivec2(TexCoord.x * size.x + (3.0 * bloom_spread), uv_y), 0);\n"
  "    h_sum += texelFetch(u_framebuffer_texture, ivec2(TexCoord.x * size.x + (4.0 * bloom_spread), uv_y), 0);\n"
  "    sum += h_sum / 9.0;\n"
  "  }\n"
  "  vec4 originalColor = texture(u_framebuffer_texture, TexCoord);\n"
  "  vec4 bloomColor = (sum / 9.0) * bloom_intensity;\n"
  "  vec4 noisyColor = apply_noise(originalColor + bloomColor, TexCoord);\n"
  "  frag_color = clamp(noisyColor, 0.0, 1.0);\n"
  "}\n";

/*const char *framebuffer_frag_src = */
/*  "#version 450 core\n"*/
/*  "out vec4 frag_color;\n"*/
/*  "in vec2 TexCoord;\n"*/
/*  "uniform sampler2D u_framebuffer_texture;\n"*/
/*  "uniform float time;\n"*/
/*  "uniform float brightness_decrease;\n"*/
/*  "uniform float noise_intensity;\n"*/
/*  "uniform float distortion_strength;\n"*/
/*  "float random(vec2 uv) {\n"*/
/*  "  return fract(sin(dot(uv.xy, vec2(12.9898, 78.233))) * 43758.5453);\n"*/
/*  "}\n"*/
/*  "vec4 apply_noise(vec4 color, vec2 uv) {\n"*/
/*  "  float noise = random(uv * time) * noise_intensity;\n"*/
/*  "  return vec4(color.rgb + noise, color.a);\n"*/
/*  "}\n"*/
/*  "void main() {\n"*/
/*  "  vec4 originalColor = texture(u_framebuffer_texture, TexCoord);\n"*/
/*  "  vec2 distortedTexCoord = TexCoord + vec2(sin(TexCoord.y * 10.0 + time) * distortion_strength, 0.0);\n"*/
/*  "  distortedTexCoord = clamp(distortedTexCoord, vec2(0.0), vec2(1.0));\n"*/
/*  "  vec4 distortedColor = texture(u_framebuffer_texture, distortedTexCoord);\n"*/
/*  "  distortedColor.rgb = distortedColor.rgb - brightness_decrease;\n"*/
/*  "  distortedColor = apply_noise(distortedColor, distortedTexCoord);\n"*/
/*  "  frag_color = distortedColor;\n"*/
/*  "}\n";*/
/**/
