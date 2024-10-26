#pragma once

extern const char *framebuffer_vert_src;
extern const char *framebuffer_frag_src;


const char *framebuffer_vert_src = 
  "#version 450 core\n"
  "layout (location = 0) in vec2 aPos;\n"
  "layout (location = 1) in vec2 aTexCoord;\n"
  "out vec2 TexCoord;\n"
  "void main() {\n"
    "gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "TexCoord = aTexCoord;\n"
  "}\n";

const char *framebuffer_frag_src = 
  "#version 450 core\n"
  "out vec4 frag_color;\n"
  "in vec2 TexCoord;\n"
  "uniform sampler2D u_framebuffer_texture;\n"
  "void main() {\n"
    "frag_color = texture(u_framebuffer_texture, TexCoord);\n"
  "}\n";

