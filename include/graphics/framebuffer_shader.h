#pragma once

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
  "uniform float time;\n"
  "uniform float brightness_decrease;\n"
  "uniform float noise_intensity;\n"
  "uniform float distortion_strength;\n"

  "float random(vec2 uv) {\n"
  "    return fract(sin(dot(uv.xy, vec2(12.9898, 78.233))) * 43758.5453);\n"
  "}\n"

  "vec4 apply_noise(vec4 color, vec2 uv) {\n"
  "    float noise = random(uv * time) * noise_intensity;\n"
  "    return vec4(color.rgb + noise, color.a);\n"
  "}\n"

  "void main() {\n"
  "    // Sample the original texture color without distortion first\n"
  "    vec4 originalColor = texture(u_framebuffer_texture, TexCoord);\n"

  // Calculate distorted texture coordinates
  "    vec2 distortedTexCoord = TexCoord + vec2(sin(TexCoord.y * 10.0 + time) * distortion_strength, 0.0);\n"

  // Clamp the texture coordinates to prevent sampling outside the texture
  "    distortedTexCoord = clamp(distortedTexCoord, vec2(0.0), vec2(1.0));\n"

  // Sample the texture with the distorted coordinates
  "    vec4 distortedColor = texture(u_framebuffer_texture, distortedTexCoord);\n"

  // Apply grayscale effect
  "    distortedColor.rgb = distortedColor.rgb - brightness_decrease;\n"

  // Apply noise effect
  "    distortedColor = apply_noise(distortedColor, distortedTexCoord);\n"


  // Combine the original color with the distorted texture color
  "    frag_color = distortedColor;\n"
  "}\n";
