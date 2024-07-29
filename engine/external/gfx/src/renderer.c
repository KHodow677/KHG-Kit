#include "gfx/renderer.h"
#include "gfx/gfx.h"
#include "gfx/shader.h"

void renderer_init() {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.render.vert_count = 0;
  state.render.verts = (Vertex *)malloc(sizeof(Vertex) * MAX_RENDER_BATCH * 4);
  glCreateVertexArrays(1, &state.render.vao);
  glBindVertexArray(state.render.vao);
  glCreateBuffers(1, &state.render.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, state.render.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_RENDER_BATCH * 4, NULL, GL_DYNAMIC_DRAW);
  uint32_t *indices = (uint32_t*)malloc(sizeof(uint32_t) * MAX_RENDER_BATCH * 6);
  uint32_t offset = 0;
  for (uint32_t i = 0; i < MAX_RENDER_BATCH * 6; i += 6) {
    indices[i + 0] = offset + 0;
    indices[i + 1] = offset + 1;
    indices[i + 2] = offset + 2;

    indices[i + 3] = offset + 2;
    indices[i + 4] = offset + 3;
    indices[i + 5] = offset + 0;
    offset += 4;
  }
  glCreateBuffers(1, &state.render.ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, state.render.ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_RENDER_BATCH * 6 * sizeof(uint32_t), indices, GL_STATIC_DRAW);
  free(indices); 
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(intptr_t)offsetof(Vertex, border_color));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(intptr_t)offsetof(Vertex, border_width));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(intptr_t)offsetof(Vertex, color));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(intptr_t *)offsetof(Vertex, texcoord));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(intptr_t *)offsetof(Vertex, tex_index));
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(intptr_t *)offsetof(Vertex, scale));
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(intptr_t *)offsetof(Vertex, pos_px));
  glEnableVertexAttribArray(7);
  glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(intptr_t *)offsetof(Vertex, corner_radius));
  glEnableVertexAttribArray(8);
  glVertexAttribPointer(10, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(intptr_t *)offsetof(Vertex, min_coord));
  glEnableVertexAttribArray(10);
  glVertexAttribPointer(11, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(intptr_t *)offsetof(Vertex, max_coord));
  glEnableVertexAttribArray(11);
  const char *vert_src = "#version 450 core\n"
    "layout (location = 0) in vec2 a_pos;\n"
    "layout (location = 1) in vec4 a_border_color;\n"
    "layout (location = 2) in float a_border_width;\n"
    "layout (location = 3) in vec4 a_color;\n"
    "layout (location = 4) in vec2 a_texcoord;\n"
    "layout (location = 5) in float a_tex_index;\n"
    "layout (location = 6) in vec2 a_scale;\n"
    "layout (location = 7) in vec2 a_pos_px;\n"
    "layout (location = 8) in float a_corner_radius;\n"
    "layout (location = 10) in vec2 a_min_coord;\n"
    "layout (location = 11) in vec2 a_max_coord;\n"
    "uniform mat4 u_proj;\n"
    "out vec4 v_border_color;\n"
    "out float v_border_width;\n"
    "out vec4 v_color;\n"
    "out vec2 v_texcoord;\n"
    "out float v_tex_index;\n"
    "flat out vec2 v_scale;\n"
    "flat out vec2 v_pos_px;\n"
    "flat out float v_is_gradient;\n"
    "out float v_corner_radius;\n"
    "out vec2 v_min_coord;\n"
    "out vec2 v_max_coord;\n"
    "void main() {\n"
    "  v_color = a_color;\n"
    "  v_texcoord = a_texcoord;\n"
    "  v_tex_index = a_tex_index;\n"
    "  v_border_color = a_border_color;\n"
    "  v_border_width = a_border_width;\n"
    "  v_scale = a_scale;\n"
    "  v_pos_px = a_pos_px;\n"
    "  v_corner_radius = a_corner_radius;\n"
    "  v_min_coord = a_min_coord;\n"
    "  v_max_coord = a_max_coord;\n"
    "  gl_Position = u_proj * vec4(a_pos.x, a_pos.y, 0.0f, 1.0);\n"
    "}\n";
  const char *frag_src = "#version 450 core\n"
    "out vec4 o_color;\n"
    "in vec4 v_color;\n"
    "in float v_tex_index;\n"
    "in vec4 v_border_color;\n"
    "in float v_border_width;\n"
    "in vec2 v_texcoord;\n"
    "flat in vec2 v_scale;\n"
    "flat in vec2 v_pos_px;\n"
    "in float v_corner_radius;\n"
    "uniform sampler2D u_textures[32];\n"
    "uniform vec2 u_screen_size;\n"
    "in vec2 v_min_coord;\n"
    "in vec2 v_max_coord;\n"
    "float rounded_box_sdf(vec2 center_pos, vec2 size, float radius) {\n"
    "  return length(max(abs(center_pos)-size+radius,0.0))-radius;\n"
    "}\n"
    "void main() {\n"
    "  if(u_screen_size.y - gl_FragCoord.y < v_min_coord.y && v_min_coord.y != -1) {\n"
    "    discard;\n"
    "  }\n"
    "  if(u_screen_size.y - gl_FragCoord.y > v_max_coord.y && v_max_coord.y != -1) {\n"
    "    discard;\n"
    "  }\n"
    "  if ((gl_FragCoord.x < v_min_coord.x && v_min_coord.x != -1) || (gl_FragCoord.x > v_max_coord.x && v_max_coord.x != -1)) {\n"
    "    discard;\n" 
    "  }\n"
    "  vec2 size = v_scale;\n"
    "  vec4 opaque_color, display_color;\n"
    "  if(v_tex_index == -1) {\n"
    "    opaque_color = v_color;\n"
    "  }\n"
    "  else {\n"
    "    opaque_color = texture(u_textures[int(v_tex_index)], v_texcoord) * v_color;\n"
    "  }\n"
    "  if(v_corner_radius != 0.0f) {"
    "    display_color = opaque_color;\n"
    "    vec2 location = vec2(v_pos_px.x, -v_pos_px.y);\n"
    "    location.y += u_screen_size.y - size.y;\n"
    "    float edge_softness = 1.0f;\n"
    "    float radius = v_corner_radius * 2.0f;\n"
    "    float distance = rounded_box_sdf(gl_FragCoord.xy - location - (size/2.0f), size / 2.0f, radius);\n"
    "    float smoothed_alpha = 1.0f-smoothstep(0.0f, edge_softness * 2.0f,distance);\n"
    "    vec3 fill_color;\n"
    "    if(v_border_width != 0.0f) {\n"
    "      vec2 location_border = vec2(location.x + v_border_width, location.y + v_border_width);\n"
    "      vec2 size_border = vec2(size.x - v_border_width * 2, size.y - v_border_width * 2);\n"
    "      float distance_border = rounded_box_sdf(gl_FragCoord.xy - location_border - (size_border / 2.0f), size_border / 2.0f, radius);\n"
    "      if(distance_border <= 0.0f) {\n"
    "        fill_color = display_color.xyz;\n"
    "      }\n"
    "      else {\n"
    "        fill_color = v_border_color.xyz;\n"
    "      }\n"
    "    }\n"
    "    else {\n"
    "      fill_color = display_color.xyz;\n"
    "    }\n"
    "    if(v_border_width != 0.0f)\n" 
    "      o_color =  mix(vec4(0.0f, 0.0f, 0.0f, 0.0f), vec4(fill_color, smoothed_alpha), smoothed_alpha);\n"
    "    else\n" 
    "      o_color = mix(vec4(0.0f, 0.0f, 0.0f, 0.0f), vec4(fill_color, display_color.a), smoothed_alpha);\n"
    "  }\n"
    "  else {\n"
    "    vec4 fill_color = opaque_color;\n"
    "    if(v_border_width != 0.0f) {\n"
    "      vec2 location = vec2(v_pos_px.x, -v_pos_px.y);\n"
    "      location.y += u_screen_size.y - size.y;\n"
    "      vec2 location_border = vec2(location.x + v_border_width, location.y + v_border_width);\n"
    "      vec2 size_border = vec2(v_scale.x - v_border_width * 2, v_scale.y - v_border_width * 2);\n"
    "      float distance_border = rounded_box_sdf(gl_FragCoord.xy - location_border - (size_border / 2.0f), size_border / 2.0f, v_corner_radius);\n"
    "      if(distance_border > 0.0f) {\n"
    "        fill_color = v_border_color;\n"
    "      }\n"
    "    }\n"
    "    o_color = fill_color;\n"
    "  }\n"
    "}\n";
  state.render.shader = shader_prg_create(vert_src, frag_src);
  state.render.vert_pos[0] = (vec4s){ -0.5f, -0.5f, 0.0f, 1.0f };
  state.render.vert_pos[1] = (vec4s){ 0.5f, -0.5f, 0.0f, 1.0f };
  state.render.vert_pos[2] = (vec4s){ 0.5f, 0.5f, 0.0f, 1.0f };
  state.render.vert_pos[3] = (vec4s){ -0.5f, 0.5f, 0.0f, 1.0f };
  int32_t tex_slots[MAX_TEX_COUNT_BATCH];
  for(uint32_t i = 0; i < MAX_TEX_COUNT_BATCH; i++) {
    tex_slots[i] = i;
  }
  glUseProgram(state.render.shader.id);
  set_projection_matrix();
  glUniform1iv(glGetUniformLocation(state.render.shader.id, "u_textures"), MAX_TEX_COUNT_BATCH, tex_slots);
}

void renderer_begin() {
  state.render.vert_count = 0;
  state.render.index_count = 0;
  state.render.tex_index = 0;
  state.render.tex_count = 0;
  state.drawcalls = 0;
}

void renderer_flush() {
  if(state.render.vert_count <= 0) {
    return;
  }
  glUseProgram(state.render.shader.id);
  glBindBuffer(GL_ARRAY_BUFFER, state.render.vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * state.render.vert_count, state.render.verts);
  for(uint32_t i = 0; i < state.render.tex_count; i++) {
    glBindTextureUnit(i, state.render.textures[i].id);
    state.drawcalls++;
  }
  vec2s renderSize = (vec2s){ (float)state.dsp_w, (float)state.dsp_h };
  glUniform2fv(glGetUniformLocation(state.render.shader.id, "u_screen_size"), 1, (float*)renderSize.raw);
  glBindVertexArray(state.render.vao);
  glDrawElements(GL_TRIANGLES, state.render.index_count, GL_UNSIGNED_INT, NULL);
}