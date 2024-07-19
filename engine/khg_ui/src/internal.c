#include "khg_ui/internal.h"

ui_state state;

uint32_t shader_create(GLenum type, const char *src) {
  uint32_t shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
  int32_t compiled; 
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if(!compiled) {
    error_func("Failed to compile shader", user_defined_data);
    glDeleteShader(shader);
  }
  return shader;
}

ui_shader shader_prg_create(const char *vert_src, const char *frag_src) {
  uint32_t vertex_shader = shader_create(GL_VERTEX_SHADER, vert_src);
  uint32_t fragment_shader = shader_create(GL_FRAGMENT_SHADER, frag_src);
  ui_shader prg;
  prg.id = glCreateProgram();
  glAttachShader(prg.id, vertex_shader);
  glAttachShader(prg.id, fragment_shader);
  glLinkProgram(prg.id);
  int32_t linked;
  glGetProgramiv(prg.id, GL_LINK_STATUS, &linked);
  if(!linked) {
    error_func("Failed to link shader program", user_defined_data);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteProgram(prg.id);
    return prg;
  }
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  return prg;
}

void shader_set_mat(ui_shader prg, const char *name, mat4 mat) {
  glUniformMatrix4fv(glGetUniformLocation(prg.id, name), 1, GL_FALSE, mat[0]);
}

void set_projection_matrix() { 
  float left = 0.0f;
  float right = state.dsp_w;
  float bottom = state.dsp_h;
  float top = 0.0f;
  float near = 0.1f;
  float far = 100.0f;
  mat4 orthoMatrix = GLM_MAT4_IDENTITY_INIT;
  orthoMatrix[0][0] = 2.0f / (right - left);
  orthoMatrix[1][1] = 2.0f / (top - bottom);
  orthoMatrix[2][2] = -1;
  orthoMatrix[3][0] = -(right + left) / (right - left);
  orthoMatrix[3][1] = -(top + bottom) / (top - bottom);
  shader_set_mat(state.render.shader, "u_proj", orthoMatrix);
}

void renderer_init() {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  state.render.vert_count = 0;
  state.render.verts = (ui_vertex *)malloc(sizeof(ui_vertex) * MAX_RENDER_BATCH * 4);
  glCreateVertexArrays(1, &state.render.vao);
  glBindVertexArray(state.render.vao);
  glCreateBuffers(1, &state.render.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, state.render.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(ui_vertex) * MAX_RENDER_BATCH * 4, NULL, GL_DYNAMIC_DRAW);
  uint32_t *indices = (uint32_t *)malloc(sizeof(uint32_t) * MAX_RENDER_BATCH * 6);
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
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), NULL);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (void*)(intptr_t)offsetof(ui_vertex, border_color));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (void*)(intptr_t)offsetof(ui_vertex, border_width));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (void*)(intptr_t)offsetof(ui_vertex, color));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (void*)(intptr_t *)offsetof(ui_vertex, texcoord));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (void*)(intptr_t *)offsetof(ui_vertex, tex_index));
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (void*)(intptr_t *)offsetof(ui_vertex, scale));
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (void*)(intptr_t *)offsetof(ui_vertex, pos_px));
  glEnableVertexAttribArray(7);
  glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (void*)(intptr_t *)offsetof(ui_vertex, corner_radius));
  glEnableVertexAttribArray(8);
  glVertexAttribPointer(10, 2, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (void*)(intptr_t *)offsetof(ui_vertex, min_coord));
  glEnableVertexAttribArray(10);
  glVertexAttribPointer(11, 2, GL_FLOAT, GL_FALSE, sizeof(ui_vertex), (void*)(intptr_t *)offsetof(ui_vertex, max_coord));
  glEnableVertexAttribArray(11);
  const char *vert_src =
    "#version 450 core\n"
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
    "v_color = a_color;\n"
    "v_texcoord = a_texcoord;\n"
    "v_tex_index = a_tex_index;\n"
    "v_border_color = a_border_color;\n"
    "v_border_width = a_border_width;\n"
    "v_scale = a_scale;\n"
    "v_pos_px = a_pos_px;\n"
    "v_corner_radius = a_corner_radius;\n"
    "v_min_coord = a_min_coord;\n"
    "v_max_coord = a_max_coord;\n"
    "gl_Position = u_proj * vec4(a_pos.x, a_pos.y, 0.0f, 1.0);\n"
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
    "    return length(max(abs(center_pos)-size+radius,0.0))-radius;\n"
    "}\n"

    "void main() {\n"
    "     if(u_screen_size.y - gl_FragCoord.y < v_min_coord.y && v_min_coord.y != -1) {\n"
    "         discard;\n"
    "     }\n"
    "     if(u_screen_size.y - gl_FragCoord.y > v_max_coord.y && v_max_coord.y != -1) {\n"
    "         discard;\n"
    "     }\n"
    "     if ((gl_FragCoord.x < v_min_coord.x && v_min_coord.x != -1) || (gl_FragCoord.x > v_max_coord.x && v_max_coord.x != -1)) {\n"
    "         discard;\n" 
    "     }\n"
    "     vec2 size = v_scale;\n"
    "     vec4 opaque_color, display_color;\n"
    "     if(v_tex_index == -1) {\n"
    "       opaque_color = v_color;\n"
    "     } else {\n"
    "       opaque_color = texture(u_textures[int(v_tex_index)], v_texcoord) * v_color;\n"
    "     }\n"
    "     if(v_corner_radius != 0.0f) {"
    "       display_color = opaque_color;\n"
    "       vec2 location = vec2(v_pos_px.x, -v_pos_px.y);\n"
    "       location.y += u_screen_size.y - size.y;\n"
    "       float edge_softness = 1.0f;\n"
    "       float radius = v_corner_radius * 2.0f;\n"
    "       float distance = rounded_box_sdf(gl_FragCoord.xy - location - (size/2.0f), size / 2.0f, radius);\n"
    "       float smoothed_alpha = 1.0f-smoothstep(0.0f, edge_softness * 2.0f,distance);\n"
    "       vec3 fill_color;\n"
    "       if(v_border_width != 0.0f) {\n"
    "           vec2 location_border = vec2(location.x + v_border_width, location.y + v_border_width);\n"
    "           vec2 size_border = vec2(size.x - v_border_width * 2, size.y - v_border_width * 2);\n"
    "           float distance_border = rounded_box_sdf(gl_FragCoord.xy - location_border - (size_border / 2.0f), size_border / 2.0f, radius);\n"
    "           if(distance_border <= 0.0f) {\n"
    "               fill_color = display_color.xyz;\n"
    "           } else {\n"
    "               fill_color = v_border_color.xyz;\n"
    "           }\n"
    "       } else {\n"
    "           fill_color = display_color.xyz;\n"
    "       }\n"
    "       if(v_border_width != 0.0f)\n" 
    "         o_color =  mix(vec4(0.0f, 0.0f, 0.0f, 0.0f), vec4(fill_color, smoothed_alpha), smoothed_alpha);\n"
    "       else\n" 
    "         o_color = mix(vec4(0.0f, 0.0f, 0.0f, 0.0f), vec4(fill_color, display_color.a), smoothed_alpha);\n"
    "     } else {\n"
    "       vec4 fill_color = opaque_color;\n"
    "       if(v_border_width != 0.0f) {\n"
    "           vec2 location = vec2(v_pos_px.x, -v_pos_px.y);\n"
    "           location.y += u_screen_size.y - size.y;\n"
    "           vec2 location_border = vec2(location.x + v_border_width, location.y + v_border_width);\n"
    "           vec2 size_border = vec2(v_scale.x - v_border_width * 2, v_scale.y - v_border_width * 2);\n"
    "           float distance_border = rounded_box_sdf(gl_FragCoord.xy - location_border - (size_border / 2.0f), size_border / 2.0f, v_corner_radius);\n"
    "           if(distance_border > 0.0f) {\n"
    "               fill_color = v_border_color;\n"
    "}\n"
    "       }\n"
    "       o_color = fill_color;\n"
    " }\n"
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
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ui_vertex) * state.render.vert_count, state.render.verts);
  for(uint32_t i = 0; i < state.render.tex_count; i++) {
    glBindTextureUnit(i, state.render.textures[i].id);
    state.drawcalls++;
  }
  vec2s renderSize = (vec2s){(float)state.dsp_w, (float)state.dsp_h};
  glUniform2fv(glGetUniformLocation(state.render.shader.id, "u_screen_size"), 1, (float*)renderSize.raw);
  glBindVertexArray(state.render.vao);
  glDrawElements(GL_TRIANGLES, state.render.index_count, GL_UNSIGNED_INT, NULL);
}

ui_text_props text_render_simple(vec2s pos, const char *text, ui_font font, ui_color font_color, bool no_render) {
  return ui_text_render(pos, text, font, font_color, -1, (vec2s){-1, -1}, no_render, false, -1, -1);
}

ui_text_props text_render_simple_wide(vec2s pos, const wchar_t *text, ui_font font, ui_color font_color, bool no_render) {
  return ui_text_render_wchar(pos, text, font, font_color, -1, (vec2s){-1, -1}, no_render, false, -1, -1);
}

ui_clickable_item_state button(const char *file, int32_t line, vec2s pos, vec2s size, ui_element_props props, ui_color color, float border_width,  bool click_color, bool hover_color) {
  return button_ex(file, line, pos, size, props, color, border_width, click_color, hover_color, (vec2s){ -1, -1 }); 
}

ui_clickable_item_state button_ex(const char *file, int32_t line, vec2s pos, vec2s size, ui_element_props props, ui_color color, float border_width, bool click_color, bool hover_color, vec2s hitbox_override) {
  uint64_t id = DJB2_INIT;
  id = djb2_hash(id, file, strlen(file));
  id = djb2_hash(id, &line, sizeof(line));
  if(state.element_id_stack != -1) {
    id = djb2_hash(id, &state.element_id_stack, sizeof(state.element_id_stack));
  }
  if(item_should_cull((ui_aabb){ .pos = pos, .size= size })) {
    return ui_clickable_idle;
  }
  ui_color hover_color_rgb = hover_color ? (props.hover_color.a == 0.0f ? ui_color_brightness(color, 1.2) : props.hover_color) : color; 
  ui_color held_color_rgb = click_color ? ui_color_brightness(color, 1.3) : color; 

  bool is_hovered = ui_hovered(pos, (vec2s){ hitbox_override.x != -1 ? hitbox_override.x : size.x, hitbox_override.y != -1 ? hitbox_override.y : size.y });
  if(state.active_element_id == 0) {
    if(is_hovered && ui_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT)) {
      state.active_element_id = id;
    }
  } 
  else if(state.active_element_id == id) {
    if(is_hovered && ui_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)) {
      ui_rect_render(pos, size, hover_color_rgb, props.border_color, border_width, props.corner_radius);
      state.active_element_id = 0;
      return ui_clickable_clicked;
    }
  }
  if(is_hovered && ui_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)) {
    state.active_element_id = 0;
  }
  if(is_hovered && ui_mouse_button_is_down(GLFW_MOUSE_BUTTON_LEFT)) {
    ui_rect_render(pos, size, held_color_rgb, props.border_color, border_width, props.corner_radius);
    return ui_clickable_held;
  }
  if(is_hovered && (!ui_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT) && !ui_mouse_button_is_down(GLFW_MOUSE_BUTTON_LEFT))) {
    ui_rect_render(pos, size, hover_color ? hover_color_rgb : color, props.border_color, border_width, props.corner_radius);
    return ui_clickable_held;
  }
  ui_rect_render(pos, size, color, props.border_color, border_width, props.corner_radius);
  return ui_clickable_idle;
}

ui_clickable_item_state div_container(vec2s pos, vec2s size, ui_element_props props, ui_color color, float border_width, bool click_color, bool hover_color) {
  if(item_should_cull((ui_aabb){ .pos = pos, .size = size })) {
    return ui_clickable_idle;
  }
  ui_color hover_color_rgb = hover_color ? (props.hover_color.a == 0.0f ? ui_color_brightness(color, 1.5) : props.hover_color) : color; 
  ui_color held_color_rgb = click_color ? ui_color_brightness(color, 1.8) : color; 

  bool is_hovered = ui_hovered(pos, size);
  if(is_hovered && ui_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)) {
    ui_rect_render(pos, size, hover_color_rgb, props.border_color, border_width, props.corner_radius);
    return ui_clickable_clicked;
  }
  if(is_hovered && ui_mouse_button_is_down(GLFW_MOUSE_BUTTON_LEFT)) {
    ui_rect_render(pos, size, held_color_rgb, props.border_color, border_width, props.corner_radius);
    return ui_clickable_held;
  }
  if(is_hovered && (!ui_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT) && !ui_mouse_button_is_down(GLFW_MOUSE_BUTTON_LEFT))) {
    ui_rect_render(pos, size, hover_color ? hover_color_rgb : color, props.border_color, border_width, props.corner_radius);
    return ui_clickable_hovered;
  }
  ui_rect_render(pos, size, color, props.border_color, border_width, props.corner_radius);
  return ui_clickable_idle;
}

void next_line_on_overflow(vec2s size, float xoffset) {
  if(!state.line_overflow) return;
  if(state.pos_ptr.x - state.current_div.aabb.pos.x + size.x > state.current_div.aabb.size.x) {
    state.pos_ptr.y += state.current_line_height;
    state.pos_ptr.x = state.current_div.aabb.pos.x + xoffset;
    state.current_line_height = 0;
  }
  if(size.y > state.current_line_height) {
    state.current_line_height = size.y;
  }
}

bool item_should_cull(ui_aabb item) {
  bool intersect = true;
  ui_aabb window =  (ui_aabb){ .pos = (vec2s){ 0, 0 }, .size = (vec2s){ state.dsp_w, state.dsp_h } };
  if(item.size.x == -1 || item.size.y == -1) {
    item.size.x = state.dsp_w;
    item.size.y = get_current_font().font_size;
  }  
  if (item.pos.x + item.size.x <= window.pos.x || item.pos.x >= window.pos.x + window.size.x) {
    intersect = false;
  }
  if (item.pos.y + item.size.y <= window.pos.y || item.pos.y >= window.pos.y + window.size.y) {
    intersect = false;
  }
  return !intersect && state.current_div.id == state.scrollbar_div.id;
  return false;
}

void draw_scrollbar_on(ui_div *div) {
  ui_next_line();
  if (state.current_div.id == div->id) {
    state.scrollbar_div = *div;
    ui_div *selected = div;
    float scroll = *state.scroll_ptr;
    ui_element_props props = get_props_for(state.theme.scrollbar_props);
    selected->total_area.x = state.pos_ptr.x;
    selected->total_area.y = state.pos_ptr.y + state.div_props.corner_radius;
    if (*state.scroll_ptr < -((div->total_area.y - *state.scroll_ptr) - div->aabb.pos.y - div->aabb.size.y) && *state.scroll_velocity_ptr < 0 && state.theme.div_smooth_scroll) {
      *state.scroll_velocity_ptr = 0;
      *state.scroll_ptr = -((div->total_area.y - *state.scroll_ptr) - div->aabb.pos.y - div->aabb.size.y);
    }
    float total_area = selected->total_area.y - scroll;
    float visible_area = selected->aabb.size.y + selected->aabb.pos.y;
    if (total_area > visible_area) {
      const float min_scrollbar_height = 20;
      float area_mapped = visible_area / total_area;
      float scroll_mapped = (-1 * scroll) / total_area;
      float scrollbar_height = MAX((selected->aabb.size.y * area_mapped - props.margin_top * 2), min_scrollbar_height);
      ui_aabb scrollbar_area = (ui_aabb){
        .pos = (vec2s){
          selected->aabb.pos.x + selected->aabb.size.x - state.theme.scrollbar_width - props.margin_right - state.div_props.padding - state.div_props.border_width,
          MIN((selected->aabb.pos.y + selected->aabb.size.y * scroll_mapped + props.margin_top + state.div_props.padding + state.div_props.border_width + state.div_props.corner_radius),
              visible_area - scrollbar_height)},
        .size = (vec2s){
          state.theme.scrollbar_width,
          scrollbar_height - state.div_props.border_width * 2 - state.div_props.corner_radius * 2},
      };
      vec2s cursorpos = (vec2s){ ui_get_mouse_x(), ui_get_mouse_y() };
      if (ui_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT) && ui_hovered(scrollbar_area.pos, scrollbar_area.size)) {
          state.drag_state.is_dragging = true;
          state.drag_state.start_cursor_pos = cursorpos;
          state.drag_state.start_scroll = *state.scroll_ptr;
      } 
      if(state.drag_state.is_dragging) {
          float cursor_delta = (cursorpos.y - state.drag_state.start_cursor_pos.y);
          float new_scroll = state.drag_state.start_scroll - cursor_delta * (total_area / visible_area);
          *state.scroll_ptr = new_scroll;

          if (*state.scroll_ptr > 0) {
            *state.scroll_ptr = 0;
          } else if (*state.scroll_ptr < -(total_area - visible_area)) {
            *state.scroll_ptr = -(total_area - visible_area);
          }
      }
      if (ui_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)) {
        state.drag_state.is_dragging = false;
      }
      ui_rect_render(scrollbar_area.pos, scrollbar_area.size, props.color, props.border_color, props.border_width, props.corner_radius);
    }
  }
}

void input_field(ui_input_field *input, ui_input_field_type type, const char *file, int32_t line) {
  if(!input->buf) {
    return;
  }
  if(!input->init) {
    ui_input_field_unselect_all(input);
    input->init = true;
  }
  ui_element_props props = get_props_for(state.theme.input_field_props);
  ui_font font = get_current_font();
  state.pos_ptr.x += props.margin_left; 
  state.pos_ptr.y += props.margin_top; 
  float wrap_point = state.pos_ptr.x + input->width - props.padding;
  if(input->selected) {
    if(ui_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT) && (ui_get_mouse_x_delta() == 0 && ui_get_mouse_y_delta() == 0)) {
      ui_text_props selected_props = ui_text_render((vec2s){ state.pos_ptr.x + props.padding, state.pos_ptr.y + props.padding }, input->buf, font, ui_no_color, wrap_point, (vec2s){ ui_get_mouse_x(), ui_get_mouse_y() }, true, false, -1, -1);
      input->cursor_index = selected_props.rendered_count;
      ui_input_field_unselect_all(input);
      input->mouse_selection_end = input->cursor_index;
      input->mouse_selection_start = input->cursor_index;
    } 
    else if(ui_mouse_button_is_down(GLFW_MOUSE_BUTTON_LEFT) && (ui_get_mouse_x_delta() != 0 || ui_get_mouse_y_delta() != 0)) {
      if(input->mouse_dir == 0) {
        input->mouse_dir = (ui_get_mouse_x_delta() < 0) ? -1 : 1;
        input->mouse_selection_end = input->cursor_index;
        input->mouse_selection_start = input->cursor_index;
      }
      ui_text_props selected_props = ui_text_render((vec2s){ state.pos_ptr.x + props.padding, state.pos_ptr.y + props.padding }, input->buf, font, ui_no_color, wrap_point, (vec2s){ ui_get_mouse_x(), ui_get_mouse_y() }, true, false, -1, -1);
      input->cursor_index = selected_props.rendered_count;
      if(input->mouse_dir == -1) {
        input->mouse_selection_start = input->cursor_index;
      }
      else if(input->mouse_dir == 1) {
        input->mouse_selection_end = input->cursor_index;
      }
      input->selection_start = input->mouse_selection_start;
      input->selection_end = input->mouse_selection_end;
      if(input->mouse_selection_start == input->mouse_selection_end) {
        input->mouse_dir = (ui_get_mouse_x_delta() < 0) ? -1 : 1;
      }
    } 
    else if(ui_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)){
      input->mouse_dir = 0;
    } 
    if(ui_char_event_occur().happened && ui_char_event_occur().char_code >= 0 && ui_char_event_occur().char_code <= 127 &&
      strlen(input->buf) + 1 <= input->buf_size && (input->max_chars ? strlen(input->buf) + 1 <= input->max_chars : true)) { 
      if(input->insert_override_callback) {
        input->insert_override_callback(input);
      } else {
        if(input->selection_start != -1) {
          int start = input->selection_dir != 0 ?  input->selection_start : input->selection_start - 1;
          int end = input->selection_end;

          remove_substr_str(input->buf, start, end);

          input->cursor_index = input->selection_start;
          ui_input_field_unselect_all(input);
        }
        ui_input_insert_char_idx(input, ui_char_event_occur().char_code, input->cursor_index++);
      }
    }
    if(ui_key_event_occur().happened && ui_key_event_occur().pressed) {
      switch(ui_key_event_occur().key_code) {
        case GLFW_KEY_BACKSPACE: {
          if(input->selection_start != -1) {
            int start = input->selection_dir != 0 ?  input->selection_start : input->selection_start - 1;
            int end = input->selection_end;
            remove_substr_str(input->buf, start, end);
            input->cursor_index = input->selection_start;
            ui_input_field_unselect_all(input);
          }
          else {
            if(input->cursor_index - 1 < 0) {
              break;
            }
            remove_i_str(input->buf, input->cursor_index - 1);
            input->cursor_index--;
          }
          break;
        }
        case GLFW_KEY_LEFT: {
          if(input->cursor_index - 1 < 0 ) {
            if(!ui_key_is_down(GLFW_KEY_LEFT_SHIFT)) {
              ui_input_field_unselect_all(input);
            }
            break;
          }
          if(ui_key_is_down(GLFW_KEY_LEFT_SHIFT)) {
            if(input->selection_end == -1) {
              input->selection_end = input->cursor_index - 1;
              input->selection_dir = -1;
            }
            input->cursor_index--;
            if(input->selection_dir == 1) {
              if(input->cursor_index != input->selection_start) {
                input->selection_end = input->cursor_index - 1;
              } 
              else { 
                ui_input_field_unselect_all(input);
              }
            } 
            else {
              input->selection_start = input->cursor_index;
            }
          } 
          else {
            if(input->selection_end == -1) {
              input->cursor_index--;
            }
            ui_input_field_unselect_all(input);
          }
          break;
        }
        case GLFW_KEY_RIGHT: {
          if(input->cursor_index + 1 > strlen(input->buf)){
            if(!ui_key_is_down(GLFW_KEY_LEFT_SHIFT)) {
              ui_input_field_unselect_all(input);
            }
            break;
          }
          if(ui_key_is_down(GLFW_KEY_LEFT_SHIFT)) {
            if(input->selection_start == -1) {
              input->selection_start = input->cursor_index;
              input->selection_dir = 1;
            }
            if(input->selection_dir == -1) {
              input->cursor_index++;
              if(input->cursor_index - 1 != input->selection_end) {
                input->selection_start = input->cursor_index;
              } 
              else {
                ui_input_field_unselect_all(input);
              }
            } 
            else {
              input->selection_end = input->cursor_index;
              input->cursor_index++;
            }
          } 
          else {
            if(input->selection_end == -1)
              input->cursor_index++;
            ui_input_field_unselect_all(input);
          }
          break;
        }
        case GLFW_KEY_ENTER: {
          break;
        }
        case GLFW_KEY_TAB: {
          if(strlen(input->buf) + 1 <= input->buf_size && (input->max_chars ? strlen(input->buf) + 1 <= input->max_chars : true)) {
            for(uint32_t i = 0; i < 2; i++) {
              insert_i_str(input->buf, ' ', input->cursor_index++);
            }
          }
          break;
        }
        case GLFW_KEY_A: {
          if(!ui_key_is_down(GLFW_KEY_LEFT_CONTROL)) {
            break;
          }
          bool selected_all = input->selection_start == 0 && input->selection_end == strlen(input->buf);
          if(selected_all) {
            ui_input_field_unselect_all(input);
          } 
          else {
            input->selection_start = 0;
            input->selection_end = strlen(input->buf);
          }
          break;
        }
        case GLFW_KEY_C: {
          if(!ui_key_is_down(GLFW_KEY_LEFT_CONTROL)) {
            break;
          }
          char selection[strlen(input->buf)];
          memset(selection, 0, strlen(input->buf));
          substr_str(input->buf, input->selection_start, input->selection_end, selection);

          clipboard_set_text(state.clipboard, selection);
          break;
        }
        case GLFW_KEY_V: {
          if(!ui_key_is_down(GLFW_KEY_LEFT_CONTROL)) {
            break;
          }
          int32_t length;
          const char *clipboard_content = clipboard_text_ex(state.clipboard, &length, LCB_CLIPBOARD);
          if(strlen(input->buf) + length > input->buf_size || (input->max_chars ? strlen(input->buf) + length > input->max_chars : false)) {
            break;
          }
          ui_input_insert_str_idx(input, clipboard_content, length, input->cursor_index);
          input->cursor_index += strlen(clipboard_content);
          break;

        }
        case GLFW_KEY_X: {
          if (!ui_key_is_down(GLFW_KEY_LEFT_CONTROL)) {
            break;
          }
          char selection[strlen(input->buf)];
          memset(selection, 0, strlen(input->buf));
          substr_str(input->buf, input->selection_start, input->selection_end, selection);
          clipboard_set_text(state.clipboard, selection);
          int start = input->selection_dir != 0 ?  input->selection_start : input->selection_start - 1;
          remove_substr_str(input->buf, start, input->selection_end);
          input->cursor_index = input->selection_start;
          ui_input_field_unselect_all(input);
          break;
        }
        default: {
          break;
        }
      }
    }
    if(input->key_callback) {
      input->key_callback(input);
    }
  }
  ui_text_props textprops =  ui_text_render((vec2s){state.pos_ptr.x + props.padding, state.pos_ptr.y + props.padding}, input->buf, font, ui_no_color, wrap_point, (vec2s){ -1, -1 }, true, false, -1, -1); 
  if(!input->retain_height) {
    input->height = (input->start_height) ? MAX(input->start_height, textprops.height) : (textprops.height ? textprops.height : get_max_char_height_font(font)); 
  }
  else {
    input->height = (input->start_height) ? input->start_height : get_max_char_height_font(font);
  }
  next_line_on_overflow((vec2s){ input->width + props.padding * 2.0f + props.margin_right + props.margin_left, input->height + props.padding * 2.0f + props.margin_bottom + props.margin_top }, state.div_props.border_width);
  ui_aabb input_aabb = (ui_aabb){ .pos = state.pos_ptr, .size = (vec2s){ input->width + props.padding * 2.0f, input->height + props.padding * 2.0f } };
  ui_clickable_item_state inputfield = button(file, line, input_aabb.pos, input_aabb.size, props, props.color, props.border_width, false, false);
  if(ui_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT) && input->selected && inputfield == ui_clickable_idle) {
    input->selected = false;
    state.input_grabbed = false;
    ui_input_field_unselect_all(input);
  } 
  else if(inputfield == ui_clickable_clicked) {
    input->selected = true;
    state.input_grabbed = true;
    ui_text_props selected_props = ui_text_render((vec2s){
      state.pos_ptr.x + props.padding, 
      state.pos_ptr.y + props.padding
    }, input->buf, font, ui_no_color, wrap_point, (vec2s){ui_get_mouse_x(), ui_get_mouse_y()}, true, false, -1, -1);
    input->cursor_index = selected_props.rendered_count;
  }
  if(input->selected) {
    char selected_buf[strlen(input->buf)];
    strncpy(selected_buf, input->buf, input->cursor_index);
    selected_buf[input->cursor_index] = '\0';
    ui_text_props selected_props =  ui_text_render((vec2s){state.pos_ptr.x + props.padding, ui_get_mouse_y() + props.padding}, selected_buf, font, ui_no_color, wrap_point, (vec2s){ -1, -1 }, true, false, -1, -1);
    vec2s cursor_pos = { (strlen(input->buf) > 0) ? selected_props.end_x : state.pos_ptr.x + props.padding, state.pos_ptr.y + props.padding + (selected_props.height - get_max_char_height_font(font)) }; 
    if(input->selection_start == -1 || input->selection_end == -1) {
      ui_rect_render(cursor_pos, (vec2s){1, get_max_char_height_font(font)}, props.text_color, ui_no_color, 0.0f, 0.0f);
    } 
    else {
      ui_text_render((vec2s){state.pos_ptr.x + props.padding, state.pos_ptr.y + props.padding}, input->buf, font, (ui_color){ 255, 255, 255, 80 }, wrap_point, (vec2s){ -1, -1 }, false, true, input->selection_start, input->selection_end);
    }
  }
  ui_text_render((vec2s){state.pos_ptr.x + props.padding, state.pos_ptr.y + props.padding}, (!strlen(input->buf) && !input->selected) ? input->placeholder : input->buf, font, !strlen(input->buf) ? ui_color_brightness(props.text_color, 0.75f) : props.text_color, wrap_point, (vec2s){-1, -1}, false, false, -1, -1); 
  state.pos_ptr.x += input->width + props.margin_right + props.padding * 2.0f;
  state.pos_ptr.y -= props.margin_top;
}

ui_font load_font(const char *filepath, uint32_t pixelsize, uint32_t tex_width, uint32_t tex_height,  uint32_t line_gap_add) {
  ui_font font = { 0 };
  FILE *file = fopen(filepath, "rb");
  if (file == NULL) {
    error_func("Failed to open font file", user_defined_data);
  }
  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);
  uint8_t *buffer = (uint8_t *)malloc(fileSize);
  size_t bytesRead = fread(buffer, 1, fileSize, file);
  fclose(file); 
  if (bytesRead != fileSize) {
    error_func("Failed to read font file", user_defined_data);
    free(buffer);
    ui_font emptyFont = { 0 };
    return emptyFont;
  }
  font.font_info = malloc(sizeof(stbtt_fontinfo));
  stbtt_InitFont((stbtt_fontinfo*)font.font_info, buffer, stbtt_GetFontOffsetForIndex(buffer, 0));
  stbtt_fontinfo *fontinfo = (stbtt_fontinfo *)font.font_info;
  int numglyphs = fontinfo->numGlyphs;
  uint8_t *bitmap = (uint8_t *)malloc(tex_width * tex_height * sizeof(uint32_t));
  uint8_t *bitmap_4bpp = (uint8_t *)malloc(tex_width * tex_height * 4 * sizeof(uint32_t));
  font.cdata = malloc(sizeof(stbtt_bakedchar) * numglyphs);
  font.tex_width = tex_width;
  font.tex_height = tex_height;
  font.line_gap_add = line_gap_add;
  font.font_size = pixelsize;
  font.num_glyphs = numglyphs;
  stbtt_BakeFontBitmap(buffer, 0, pixelsize, bitmap, tex_width, tex_height, 32, numglyphs, (stbtt_bakedchar *)font.cdata);
  uint32_t bitmap_index = 0;
  for(uint32_t i = 0; i < (uint32_t)(tex_width * tex_height * 4); i++) {
    bitmap_4bpp[i] = bitmap[bitmap_index];
    if((i + 1) % 4 == 0) {
      bitmap_index++;
    }
  }
  glGenTextures(1, &font.bitmap.id);
  glBindTexture(GL_TEXTURE_2D, font.bitmap.id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap_4bpp);
  glGenerateMipmap(GL_TEXTURE_2D);
  free(bitmap);
  free(bitmap_4bpp);
  return font;
}

ui_font get_current_font() {
  return state.font_stack ? *state.font_stack : state.theme.font;
}

ui_clickable_item_state button_element_loc(void *text, const char *file, int32_t line, bool wide) {
  ui_element_props props = get_props_for(state.theme.button_props);
  float padding = props.padding;
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom = props.margin_bottom; 
  state.pos_ptr.x += margin_left;
  state.pos_ptr.y += margin_top;
  ui_font font = get_current_font();
  ui_text_props text_props;
  if(wide) {
    text_props = text_render_simple_wide(state.pos_ptr, (const wchar_t *)text, font, ui_no_color, true);
  }
  else {
    text_props = text_render_simple(state.pos_ptr, (const char*)text, font, ui_no_color, true);
  }
  ui_color color = props.color;
  ui_color text_color = ui_hovered(state.pos_ptr, (vec2s){ text_props.width, text_props.height }) && props.hover_text_color.a != 0.0f ? props.hover_text_color : props.text_color;
  next_line_on_overflow((vec2s){ text_props.width + padding * 2.0f + margin_right + margin_left, text_props.height + padding * 2.0f + margin_bottom + margin_top }, state.div_props.border_width);
  ui_clickable_item_state ret = button(file, line, state.pos_ptr, (vec2s){ text_props.width + padding * 2, text_props.height + padding * 2 }, props, color, props.border_width, true, true);
  if(wide) {
    text_render_simple_wide((vec2s){state.pos_ptr.x + padding, state.pos_ptr.y + padding}, (const wchar_t *)text, font, text_color, false);
  }
  else {
    text_render_simple((vec2s){state.pos_ptr.x + padding, state.pos_ptr.y + padding}, (const char *)text, font, text_color, false);
  }
  state.pos_ptr.x += text_props.width + margin_right + padding * 2.0f;
  state.pos_ptr.y -= margin_top;
  return ret;
}

ui_clickable_item_state button_fixed_element_loc(void *text, float width, float height, const char *file, int32_t line, bool wide) {
  ui_element_props props = get_props_for(state.theme.button_props);
  float padding = props.padding;
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom = props.margin_bottom;
  ui_font font = state.font_stack ? *state.font_stack : state.theme.font;
  ui_text_props text_props; 
  if(wide) {
    text_props = text_render_simple_wide(state.pos_ptr, (const wchar_t *)text, font, ui_no_color, true);
  }
  else {
    text_props = text_render_simple(state.pos_ptr, (const char *)text, font, ui_no_color, true);
  }
  ui_color color = props.color;
  ui_color text_color = ui_hovered(state.pos_ptr, (vec2s){ text_props.width, text_props.height }) && props.hover_text_color.a != 0.0f ? props.hover_text_color : props.text_color;
  int32_t render_width = ((width == -1) ? text_props.width : width);
  int32_t render_height = ((height == -1) ? text_props.height : height);
  next_line_on_overflow((vec2s){ render_width + padding * 2.0f + margin_right + margin_left, render_height + padding * 2.0f + margin_bottom + margin_top }, state.div_props.border_width); 
  state.pos_ptr.x += margin_left;
  state.pos_ptr.y += margin_top; 
  ui_clickable_item_state ret = button(file, line, state.pos_ptr, (vec2s){ render_width + padding * 2.0f, render_height + padding * 2.0f }, props, color, props.border_width, false, true);
  ui_set_cull_end_x(state.pos_ptr.x + render_width + padding);
  if(wide) {
    text_render_simple_wide((vec2s){ state.pos_ptr.x + padding + ((width != -1) ? (width - text_props.width) / 2.0f : 0), state.pos_ptr.y + padding + ((height != -1) ? (height - text_props.height) / 2.0f : 0) }, (const wchar_t *)text, font, text_color, false);
  } else {
    text_render_simple((vec2s){ state.pos_ptr.x + padding + ((width != -1) ? (width - text_props.width) / 2.0f : 0), state.pos_ptr.y + padding + ((height != -1) ? (height - text_props.height) / 2.0f : 0) }, (const char *)text, font, text_color, false);
  }
  ui_unset_cull_end_x();
  state.pos_ptr.x += render_width + margin_right + padding * 2.0f;
  state.pos_ptr.y -= margin_top;
  return ret;
}

ui_clickable_item_state checkbox_element_loc(void *text, bool *val, ui_color tick_color, ui_color tex_color, const char *file, int32_t line, bool wide) {
  ui_font font = get_current_font();
  ui_element_props props = get_props_for(state.theme.checkbox_props);
  float margin_left = props.margin_left;
  float margin_right = props.margin_right;
  float margin_top = props.margin_top;
  float margin_bottom = props.margin_bottom;
  float checkbox_size; 
  if(wide) {
    checkbox_size = ui_text_dimension_wide((const wchar_t*)text).y;
  }
  else {
    checkbox_size = ui_text_dimension((const char*)text).y;
  }
  next_line_on_overflow((vec2s){ checkbox_size + margin_left + margin_right + props.padding * 2.0f, checkbox_size + margin_top + margin_bottom + props.padding * 2.0f }, state.div_props.border_width);
  state.pos_ptr.x += margin_left; 
  state.pos_ptr.y += margin_top;
  ui_color checkbox_color = (*val) ? ((tick_color.a == 0) ? props.color : tick_color) : props.color;
  ui_clickable_item_state checkbox = button(file, line, state.pos_ptr, (vec2s){ checkbox_size + props.padding * 2.0f, checkbox_size + props.padding * 2.0f }, props, checkbox_color, props.border_width, false, false);
  if(wide) {
    text_render_simple_wide((vec2s){state.pos_ptr.x + checkbox_size + props.padding * 2.0f + margin_right, state.pos_ptr.y + props.padding}, (const wchar_t*)text, font, props.text_color, false);
  }
  else {
    text_render_simple((vec2s){state.pos_ptr.x + checkbox_size + props.padding * 2.0f + margin_right, state.pos_ptr.y + props.padding}, (const char*)text, font, props.text_color, false);
  }
  if(checkbox == ui_clickable_clicked) {
    *val = !*val;
  }
  if(*val) {
    ui_image_render((vec2s){ state.pos_ptr.x + props.padding, state.pos_ptr.y + props.padding }, tex_color, (ui_texture){ .id = state.tex_tick.id, .width = (uint32_t)(checkbox_size), .height = (uint32_t)(checkbox_size)}, (ui_color){0.0f, 0.0f, 0.0f, 0.0f}, 0, props.corner_radius);
  }
  state.pos_ptr.x += checkbox_size + props.padding * 2.0f + margin_right + ((wide) ? ui_text_dimension_wide((const wchar_t*)text).x : ui_text_dimension((const char*)text).x) + margin_right;
  state.pos_ptr.y -= margin_top;
  return checkbox;
}

void dropdown_menu_item_loc(void **items, void *placeholder, uint32_t item_count, float width, float height, int32_t *selected_index, bool *opened, const char *file, int32_t line, bool wide) {
  ui_element_props props = get_props_for(state.theme.button_props);
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom = props.margin_bottom;
  float padding = props.padding;
  ui_font font = get_current_font();
  int32_t placeholder_strlen = (wide) ? wcslen((const wchar_t *)placeholder) : strlen((const char *)placeholder);
  void *button_text = (void *)((*selected_index != -1) ? items[*selected_index] : (placeholder_strlen != 0) ? placeholder : "Select");
  ui_text_props text_props;  
  if(wide) {
    text_props = text_render_simple_wide((vec2s){ state.pos_ptr.x + padding, state.pos_ptr.y + padding }, (const wchar_t *)button_text, font, props.text_color, true);
  }
  else {
    text_props = text_render_simple((vec2s){ state.pos_ptr.x + padding, state.pos_ptr.y + padding }, (const char *)button_text, font, props.text_color, true);
  }
  float item_height = get_max_char_height_font(font) + ((*opened) ? height + padding * 4.0f + margin_top : padding * 2.0f); 
  next_line_on_overflow((vec2s){ width + padding * 2.0f + margin_right, item_height + margin_top + margin_bottom }, 0.0f);
  state.pos_ptr.x += margin_left;
  state.pos_ptr.y += margin_top;
  vec2s button_pos = state.pos_ptr;
  ui_clickable_item_state dropdown_button = button(file, line, state.pos_ptr, (vec2s){ (float)width + padding * 2.0f, (float)text_props.height + padding * 2.0f },  props, props.color, props.border_width, false, true);
  if(wide) {
    text_render_simple_wide((vec2s){state.pos_ptr.x + padding, state.pos_ptr.y + padding}, (const wchar_t*)button_text, font, props.text_color, false);
  }
  else {
    text_render_simple((vec2s){state.pos_ptr.x + padding, state.pos_ptr.y + padding}, (const char*)button_text, font, props.text_color, false);
  }
  vec2s image_size = (vec2s){20, 10};
  ui_image_render((vec2s){ state.pos_ptr.x + width + padding - image_size.x, state.pos_ptr.y + ((text_props.height + padding * 2) - image_size.y) / 2.0f }, props.text_color, (ui_texture){ .id = state.tex_arrow_down.id, .width = (uint32_t)image_size.x, .height = (uint32_t)image_size.y }, ui_no_color, 0.0f, 0.0f);
  if(dropdown_button == ui_clickable_clicked) {
    *opened = !*opened;
  }
  if(*opened) {
    if((ui_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT) && dropdown_button != ui_clickable_clicked) || (!ui_input_grabbed() && ui_key_went_down(GLFW_KEY_ESCAPE))) {
      *opened = false;
    }
    ui_element_props div_props = ui_get_theme().div_props;
    div_props.corner_radius = props.corner_radius;
    div_props.border_color = props.border_color;
    div_props.border_width = props.border_width;
    div_props.color = props.color;
    ui_push_style_props(div_props);
    ui_div_begin(((vec2s){ state.pos_ptr.x, state.pos_ptr.y + text_props.height + padding * 2.0f }), ((vec2s){ width + props.padding * 2.0f, height + props.padding * 2.0f }), false);
    ui_pop_style_props();
    for(uint32_t i = 0; i < item_count; i++) {
      ui_element_props text_props = ui_get_theme().text_props;
      text_props.text_color = props.text_color;
      bool hovered = ui_hovered((vec2s){ state.pos_ptr.x + text_props.margin_left, state.pos_ptr.y + text_props.margin_top }, (vec2s){ width + props.padding * 2.0f, ui_get_theme().font.font_size });
      if(hovered) {
        ui_rect_render(((vec2s){ state.pos_ptr.x, state.pos_ptr.y }), (vec2s){ width + props.padding * 2.0f, ui_get_theme().font.font_size + props.margin_top }, ui_color_brightness(div_props.color, 1.2f), ui_no_color, 0.0f, 0.0f);
      }
      if(hovered && ui_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)) {
        *selected_index = i;
      }
      ui_push_style_props(text_props);
      ui_text(items[i]);
      ui_pop_style_props();
      ui_next_line();
    }
    ui_div_end();
  }
  state.pos_ptr.x += width + padding * 2.0f + margin_right;
  state.pos_ptr.y -= margin_top;
  ui_push_style_props(props);
}

int32_t menu_item_list_item_loc(void **items, uint32_t item_count, int32_t selected_index, ui_menu_item_callback per_cb, bool vertical, const char *file, int32_t line, bool wide) {
  ui_element_props props = get_props_for(state.theme.button_props);
  float padding = props.padding;
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom = props.margin_bottom; 
  ui_color color = props.color;
  ui_font font = get_current_font();
  ui_text_props text_props[item_count];
  float width = 0;
  for(uint32_t i  = 0; i < item_count; i++) {
    if(wide) {
      text_props[i] = text_render_simple_wide((vec2s){state.pos_ptr.x, state.pos_ptr.y + margin_top}, (const wchar_t*)items[i], font, props.text_color, true);
    }
    else {
      text_props[i] = text_render_simple((vec2s){state.pos_ptr.x, state.pos_ptr.y + margin_top}, (const char*)items[i], font, props.text_color, true);
    }
    width += text_props[i].width + padding * 2.0f;
  }
  next_line_on_overflow((vec2s){ width + padding * 2.0f + margin_right + margin_left, font.font_size + padding * 2.0f + margin_bottom + margin_top }, state.div_props.border_width);
  state.pos_ptr.y += margin_top; 
  state.pos_ptr.x += margin_left;
  uint32_t element_width = 0;
  uint32_t clicked_item = -1;
  for(uint32_t i = 0; i < item_count; i++) {
    ui_element_props props = state.theme.button_props;
    props.margin_left = 0;
    props.margin_right = 0;
    ui_element_props button_props = state.theme.button_props;
    ui_push_style_props(props);
    if(i == selected_index) {
      props.color = ui_color_brightness(props.color, 1.2); 
    }
    ui_push_style_props(props);
    if(wide) {
      if(ui_button_wide_loc((const wchar_t*)items[i], file, line) == ui_clickable_clicked) {
        clicked_item = i;  
      } 
    } 
    else {
      if(ui_button_loc((const char *)items[i], file, line) == ui_clickable_clicked) {
        clicked_item = i;  
      } 
    }
    ui_pop_style_props();
    per_cb(&i);
  } 
  next_line_on_overflow((vec2s){element_width + margin_right, font.font_size + margin_top + margin_bottom}, state.div_props.border_width);
  state.pos_ptr.y -= margin_top;
  return clicked_item;
}

int32_t get_max_char_height_font(ui_font font) {
  float fontScale = stbtt_ScaleForPixelHeight((stbtt_fontinfo *)font.font_info, font.font_size);
  int32_t xmin, ymin, xmax, ymax;
  int32_t codepoint = 'p';
  stbtt_GetCodepointBitmapBox((stbtt_fontinfo *)font.font_info, codepoint, fontScale, fontScale, &xmin, &ymin, &xmax, &ymax);
  return ymax - ymin;
}

void remove_i_str(char *str, int32_t index) {
  int32_t len = strlen(str);
  if (index >= 0 && index < len) {
    for (int32_t i = index; i < len - 1; i++) {
      str[i] = str[i + 1];
    }
    str[len - 1] = '\0';
  }
}

void remove_substr_str(char *str, int start_index, int end_index) {
  int len = strlen(str);
  memmove(str + start_index, str + end_index + 1, len - end_index);
  str[len - (end_index - start_index) + 1] = '\0'; 
}

void insert_i_str(char *str, char ch, int32_t index) {
  int len = strlen(str);
  if (index < 0 || index > len) {
    error_func("Invalid string index for inserting", user_defined_data);
    return;
  }
  for (int i = len; i > index; i--) {
    str[i] = str[i - 1];
  }
  str[index] = ch;
  str[len + 1] = '\0'; 
}

void insert_str_str(char *source, const char *insert, int32_t index) {
  int source_len = strlen(source);
  int insert_len = strlen(insert);
  if (index < 0 || index > source_len) {
    error_func("Index for inserting out of bounds", user_defined_data);
    return;
  }
  memmove(source + index + insert_len, source + index, source_len - index + 1);
  memcpy(source + index, insert, insert_len);
}

void substr_str(const char *str, int start_index, int end_index, char *substring) {
  int substring_length = end_index - start_index + 1; 
  strncpy(substring, str + start_index, substring_length);
  substring[substring_length] = '\0';
}

int map_vals(int value, int from_min, int from_max, int to_min, int to_max) {
  return (value - from_min) * (to_max - to_min) / (from_max - from_min) + to_min;
}

void glfw_key_callback(GLFWwindow *window, int32_t key, int scancode, int action, int mods) {
  (void)window;
  (void)mods;
  (void)scancode;
  if(action != GLFW_RELEASE) {
    if(!state.input.keyboard.keys[key]) 
      state.input.keyboard.keys[key] = true;
  }  
  else {
    state.input.keyboard.keys[key] = false;
  }
  state.input.keyboard.keys_changed[key] = (action != GLFW_REPEAT);
  for(uint32_t i = 0; i < state.input.key_cb_count; i++) {
    state.input.key_cbs[i](window, key, scancode, action, mods);
  }
  state.key_ev.happened = true;
  state.key_ev.pressed = action != GLFW_RELEASE;
  state.key_ev.key_code = key;
}

void glfw_mouse_button_callback(GLFWwindow *window, int32_t button, int action, int mods) {
  (void)window;
  (void)mods;
  if(action != GLFW_RELEASE)  {
    if(!state.input.mouse.buttons_current[button])
      state.input.mouse.buttons_current[button] = true;
  } 
  else {
    state.input.mouse.buttons_current[button] = false;
  }
  for(uint32_t i = 0; i < state.input.mouse_button_cb_count; i++) {
    state.input.mouse_button_cbs[i](window, button, action, mods);
  }
  state.mb_ev.happened = true;
  state.mb_ev.pressed = action != GLFW_RELEASE;
  state.mb_ev.button_code = button;
}

void glfw_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  (void)window;
  state.input.mouse.xscroll_delta = xoffset;
  state.input.mouse.yscroll_delta = yoffset;
  for(uint32_t i = 0; i< state.input.scroll_cb_count; i++) {
    state.input.scroll_cbs[i](window, xoffset, yoffset);
  }
  state.scr_ev.happened = true;
  state.scr_ev.x_offset = xoffset;
  state.scr_ev.y_offset = yoffset;
  ui_div *selected_div = &state.selected_div;
  if(!selected_div->scrollable) return;
  if((state.grabbed_div.id != -1 && selected_div->id != state.grabbed_div.id)) return;
  if(yoffset < 0.0f) {
    if(selected_div->total_area.y > (selected_div->aabb.size.y + selected_div->aabb.pos.y)) { 
      if(state.theme.div_smooth_scroll) {
        *state.scroll_velocity_ptr -= state.theme.div_scroll_acceleration;
        state.div_velocity_accelerating = true;
      } else {
        *state.scroll_ptr -= state.theme.div_scroll_amount_px;
      }
    } 
  } 
  else if (yoffset > 0.0f) {
    if(*state.scroll_ptr) {
      if(state.theme.div_smooth_scroll) {
        *state.scroll_velocity_ptr += state.theme.div_scroll_acceleration;
        state.div_velocity_accelerating = false;
      } else {
        *state.scroll_ptr += state.theme.div_scroll_amount_px;
      }
    }        
  }
  if(state.theme.div_smooth_scroll) {
    *state.scroll_velocity_ptr = MIN(MAX(*state.scroll_velocity_ptr, -state.theme.div_scroll_max_velocity), state.theme.div_scroll_max_velocity);
  }
}

void glfw_cursor_callback(GLFWwindow *window, double xpos, double ypos) {
  (void)window;
  ui_mouse *mouse = &state.input.mouse;
  mouse->xpos = xpos;
  mouse->ypos = ypos;
  if(mouse->first_mouse_press) {
    mouse->xpos_last = xpos;
    mouse->ypos_last = ypos;
    mouse->first_mouse_press = false;
  }
  mouse->xpos_delta = mouse->xpos - mouse->xpos_last;
  mouse->ypos_delta = mouse->ypos - mouse->ypos_last;
  mouse->xpos_last = xpos;
  mouse->ypos_last = ypos;
  for(uint32_t i = 0; i < state.input.cursor_pos_cb_count; i++) {
    state.input.cursor_pos_cbs[i](window, xpos, ypos);
  }
  state.cp_ev.happened = true;
  state.cp_ev.x = xpos;
  state.cp_ev.y = ypos;
}

void glfw_char_callback(GLFWwindow *window, uint32_t charcode) {
  (void)window;
  state.ch_ev.char_code = charcode;
  state.ch_ev.happened = true;
}

void update_input() {
  memcpy(state.input.mouse.buttons_last, state.input.mouse.buttons_current, sizeof(bool) * MAX_MOUSE_BUTTONS);
}

void clear_events() {
  state.key_ev.happened = false;
  state.mb_ev.happened = false;
  state.cp_ev.happened = false;
  state.scr_ev.happened = false;
  state.ch_ev.happened = false;
  state.input.mouse.xpos_delta = 0;
  state.input.mouse.ypos_delta = 0;
}

uint64_t djb2_hash(uint64_t hash, const void *buf, size_t size) {
  uint8_t *bytes = (uint8_t *)buf;
  int c;
  while ((c = *bytes++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash;
}

void props_stack_create(ui_props_stack *stack) {
  stack->data = (ui_element_props *)malloc(UI_STACK_INIT_CAP * sizeof(ui_element_props));
  if(!stack->data) {
    error_func("Failed to allocate memory for stack data structure", user_defined_data);
  }
  stack->count = 0;
  stack->cap = UI_STACK_INIT_CAP;
}

void props_stack_resize(ui_props_stack *stack, uint32_t newcap) {
  ui_element_props* newdata = (ui_element_props *)realloc(stack->data, newcap * sizeof(ui_element_props));
  if(!newdata) {
    error_func("Failed to reallocate memory for stack datastructure", user_defined_data);
  }
  stack->data = newdata;
  stack->cap = newcap;
}

void props_stack_push(ui_props_stack *stack, ui_element_props props) {
  if(stack->count == stack->cap) {
    props_stack_resize(stack, stack->cap * 2);
  }
  stack->data[stack->count++] = props;
}

ui_element_props props_stack_pop(ui_props_stack *stack) {
  ui_element_props val = stack->data[--stack->count];
  if(stack->count > 0 && stack->count == stack->cap / 4) {
    props_stack_resize(stack, stack->cap / 2);
  }
  return val;
}

ui_element_props props_stack_peak(ui_props_stack *stack) {
  return stack->data[stack->count - 1];
}

bool props_stack_empty(ui_props_stack *stack) {
  return stack->count == 0;
}

ui_element_props get_props_for(ui_element_props props) {
  return (!props_stack_empty(&state.props_stack)) ? props_stack_peak(&state.props_stack) : props; 
}

