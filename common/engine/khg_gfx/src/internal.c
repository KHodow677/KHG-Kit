#include "glad/glad.h"
#include "khg_gfx/internal.h"
#include "khg_gfx/elements.h"
#include "khg_gfx/font.h"
#include "khg_gfx/ui.h"
#include "khg_utl/error_func.h"
#include "cglm/mat4.h"
#include "khg_utl/file_reader.h"
#include "libclipboard/libclipboard.h"
#include <stdio.h>
#include <string.h>

gfx_state GFX_STATE;

unsigned int gfx_internal_shader_create(unsigned int type, const char *src) {
  unsigned int shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
  int compiled; 
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    utl_error_func("Failed to compile shader", utl_user_defined_data);
    glDeleteShader(shader);
  }
  return shader;
}

gfx_shader gfx_internal_shader_prg_create(const char *vert_src, const char *frag_src) {
  unsigned int vertex_shader = gfx_internal_shader_create(GL_VERTEX_SHADER, vert_src);
  unsigned int fragment_shader = gfx_internal_shader_create(GL_FRAGMENT_SHADER, frag_src);
  gfx_shader prg;
  prg.id = glCreateProgram();
  glAttachShader(prg.id, vertex_shader);
  glAttachShader(prg.id, fragment_shader);
  glLinkProgram(prg.id);
  int linked;
  glGetProgramiv(prg.id, GL_LINK_STATUS, &linked);
  if (!linked) {
    utl_error_func("Failed to link shader program", utl_user_defined_data);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteProgram(prg.id);
    return prg;
  }
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  return prg;
}

void gfx_internal_shader_set_mat(gfx_shader prg, const char *name, mat4 mat) {
  glUniformMatrix4fv(glGetUniformLocation(prg.id, name), 1, GL_FALSE, mat[0]);
}

void gfx_internal_set_projection_matrix() { 
  float left = 0.0f;
  float right = GFX_STATE.dsp_w;
  float bottom = GFX_STATE.dsp_h;
  float top = 0.0f;
  float near_val = 0.1f;
  float far_val = 100.0f;
  mat4 orthoMatrix = GLM_MAT4_IDENTITY_INIT;
  orthoMatrix[0][0] = 2.0f / (right - left);
  orthoMatrix[1][1] = 2.0f / (top - bottom);
  orthoMatrix[2][2] = -1;
  orthoMatrix[3][0] = -(right + left) / (right - left);
  orthoMatrix[3][1] = -(top + bottom) / (top - bottom);
  gfx_internal_shader_set_mat(GFX_STATE.render.shader, "u_proj", orthoMatrix);
}

void gfx_internal_renderer_init() {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  GFX_STATE.render.vert_count = 0;
  GFX_STATE.render.verts = (gfx_vertex *)malloc(sizeof(gfx_vertex) * GFX_MAX_RENDER_BATCH * 4);
  glCreateVertexArrays(1, &GFX_STATE.render.vao);
  glBindVertexArray(GFX_STATE.render.vao);
  glCreateBuffers(1, &GFX_STATE.render.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, GFX_STATE.render.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(gfx_vertex) * GFX_MAX_RENDER_BATCH * 4, NULL, GL_DYNAMIC_DRAW);
  unsigned int *indices = malloc(sizeof(unsigned int) * GFX_MAX_RENDER_BATCH * 6);
  unsigned int offset = 0;
  for (unsigned int i = 0; i < GFX_MAX_RENDER_BATCH * 6; i += 6) {
    indices[i + 0] = offset + 0;
    indices[i + 1] = offset + 1;
    indices[i + 2] = offset + 2;
    indices[i + 3] = offset + 2;
    indices[i + 4] = offset + 3;
    indices[i + 5] = offset + 0;
    offset += 4;
  }
  glCreateBuffers(1, &GFX_STATE.render.ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GFX_STATE.render.ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, GFX_MAX_RENDER_BATCH * 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
  free(indices); 
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(gfx_vertex), NULL);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(gfx_vertex), (void *)(int *)offsetof(gfx_vertex, border_color));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(gfx_vertex), (void *)(int *)offsetof(gfx_vertex, border_width));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(gfx_vertex), (void *)(int *)offsetof(gfx_vertex, color));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(gfx_vertex), (void *)(int **)offsetof(gfx_vertex, texcoord));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(gfx_vertex), (void *)(int **)offsetof(gfx_vertex, tex_index));
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(gfx_vertex), (void *)(int **)offsetof(gfx_vertex, scale));
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(gfx_vertex), (void *)(int **)offsetof(gfx_vertex, pos_px));
  glEnableVertexAttribArray(7);
  glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(gfx_vertex), (void *)(int **)offsetof(gfx_vertex, corner_radius));
  glEnableVertexAttribArray(8);
  glVertexAttribPointer(10, 2, GL_FLOAT, GL_FALSE, sizeof(gfx_vertex), (void *)(int **)offsetof(gfx_vertex, min_coord));
  glEnableVertexAttribArray(10);
  glVertexAttribPointer(11, 2, GL_FLOAT, GL_FALSE, sizeof(gfx_vertex), (void *)(int **)offsetof(gfx_vertex, max_coord));
  glEnableVertexAttribArray(11);
  utl_file_reader *reader = utl_file_reader_open("res/assets/shaders/vertex/primary.vert", UTL_READ_TEXT);
  char vert_src[(int)pow(2, 16)];
  utl_file_reader_read(vert_src, sizeof(char), sizeof(vert_src) - 1, reader);
  utl_file_reader_close(reader);
  reader = utl_file_reader_open("res/assets/shaders/fragment/primary.frag", UTL_READ_TEXT);
  char frag_src[(int)pow(2, 16)];
  utl_file_reader_read(frag_src, sizeof(char), sizeof(frag_src) - 1, reader);
  utl_file_reader_close(reader);
  GFX_STATE.render.shader = gfx_internal_shader_prg_create(vert_src, frag_src);
  GFX_STATE.render.vert_pos[0] = (vec4s){ -0.5f, -0.5f, 0.0f, 1.0f };
  GFX_STATE.render.vert_pos[1] = (vec4s){ 0.5f, -0.5f, 0.0f, 1.0f };
  GFX_STATE.render.vert_pos[2] = (vec4s){ 0.5f, 0.5f, 0.0f, 1.0f };
  GFX_STATE.render.vert_pos[3] = (vec4s){ -0.5f, 0.5f, 0.0f, 1.0f };
  int tex_slots[GFX_MAX_TEX_COUNT_BATCH];
  for (unsigned int i = 0; i < GFX_MAX_TEX_COUNT_BATCH; i++) {
    tex_slots[i] = i;
  }
  glUseProgram(GFX_STATE.render.shader.id);
  gfx_internal_set_projection_matrix();
  glUniform1iv(glGetUniformLocation(GFX_STATE.render.shader.id, "u_textures"), GFX_MAX_TEX_COUNT_BATCH, tex_slots);
}

void gfx_internal_renderer_set_shader(gfx_shader shader) {
  GFX_STATE.render.shader = shader;
  GFX_STATE.render.vert_pos[0] = (vec4s){ -0.5f, -0.5f, 0.0f, 1.0f };
  GFX_STATE.render.vert_pos[1] = (vec4s){ 0.5f, -0.5f, 0.0f, 1.0f };
  GFX_STATE.render.vert_pos[2] = (vec4s){ 0.5f, 0.5f, 0.0f, 1.0f };
  GFX_STATE.render.vert_pos[3] = (vec4s){ -0.5f, 0.5f, 0.0f, 1.0f };
  int tex_slots[GFX_MAX_TEX_COUNT_BATCH];
  for (unsigned int i = 0; i < GFX_MAX_TEX_COUNT_BATCH; i++) {
    tex_slots[i] = i;
  }
  glUseProgram(GFX_STATE.render.shader.id);
  gfx_internal_set_projection_matrix();
  glUniform1iv(glGetUniformLocation(GFX_STATE.render.shader.id, "u_textures"), GFX_MAX_TEX_COUNT_BATCH, tex_slots);
}

void gfx_internal_renderer_begin() {
  GFX_STATE.render.vert_count = 0;
  GFX_STATE.render.index_count = 0;
  GFX_STATE.render.tex_index = 0;
  GFX_STATE.render.tex_count = 0;
  GFX_STATE.drawcalls = 0;
}

void gfx_internal_renderer_flush() {
  if (GFX_STATE.render.vert_count <= 0) {
    return;
  }
  glUseProgram(GFX_STATE.render.shader.id);
  glBindBuffer(GL_ARRAY_BUFFER, GFX_STATE.render.vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(gfx_vertex) * GFX_STATE.render.vert_count, GFX_STATE.render.verts);
  for (unsigned int i = 0; i < GFX_STATE.render.tex_count; i++) {
    glBindTextureUnit(i, GFX_STATE.render.textures[i].id);
    GFX_STATE.drawcalls++;
  }
  vec2s renderSize = (vec2s){ (float)GFX_STATE.dsp_w, (float)GFX_STATE.dsp_h };
  glUniform2fv(glGetUniformLocation(GFX_STATE.render.shader.id, "u_screen_size"), 1, (float*)renderSize.raw);
  glBindVertexArray(GFX_STATE.render.vao);
  glDrawElements(GL_TRIANGLES, GFX_STATE.render.index_count, GL_UNSIGNED_INT, NULL);
}

gfx_text_props gfx_internal_text_render_simple(vec2s pos, const char *text, gfx_font font, gfx_color font_color, bool no_render) {
  return gfx_text_render(pos, text, font, font_color, -1, (vec2s){-1, -1}, no_render, false, -1, -1);
}

gfx_clickable_item_state gfx_internal_button(const char *file, int line, vec2s pos, vec2s size, gfx_element_props props, gfx_color color, float border_width,  bool click_color, bool hover_color) {
  return gfx_internal_button_ex(file, line, pos, size, props, color, border_width, click_color, hover_color, (vec2s){ -1, -1 }); 
}

gfx_clickable_item_state gfx_internal_button_ex(const char *file, int line, vec2s pos, vec2s size, gfx_element_props props, gfx_color color, float border_width, bool click_color, bool hover_color, vec2s hitbox_override) {
  unsigned long id = GFX_DJB2_INIT;
  id = gfx_internal_djb2_hash(id, file, strlen(file));
  id = gfx_internal_djb2_hash(id, &line, sizeof(line));
  if (GFX_STATE.element_id_stack != -1) {
    id = gfx_internal_djb2_hash(id, &GFX_STATE.element_id_stack, sizeof(GFX_STATE.element_id_stack));
  }
  if (gfx_internal_item_should_cull((gfx_aabb){ .pos = pos, .size= size }, true)) {
    return GFX_CLICKABLE_IDLE;
  }
  gfx_color hover_color_rgb = hover_color ? (props.hover_color.a == 0.0f ? gfx_color_brightness(color, 1.2) : props.hover_color) : color; 
  gfx_color held_color_rgb = click_color ? gfx_color_brightness(color, 1.3) : color; 

  bool is_hovered = gfx_hovered(pos, (vec2s){ hitbox_override.x != -1 ? hitbox_override.x : size.x, hitbox_override.y != -1 ? hitbox_override.y : size.y });
  if (GFX_STATE.active_element_id == 0) {
    if (is_hovered && gfx_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT)) {
      GFX_STATE.active_element_id = id;
    }
  } 
  else if (GFX_STATE.active_element_id == id) {
    if(is_hovered && gfx_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)) {
      gfx_rect_render(pos, size, hover_color_rgb, props.border_color, border_width, props.corner_radius, 0.0f);
      GFX_STATE.active_element_id = 0;
      return GFX_CLICKABLE_CLICKED;
    }
  }
  if (is_hovered && gfx_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)) {
    GFX_STATE.active_element_id = 0;
  }
  if (is_hovered && gfx_mouse_button_is_down(GLFW_MOUSE_BUTTON_LEFT)) {
    gfx_rect_render(pos, size, held_color_rgb, props.border_color, border_width, props.corner_radius, 0.0f);
    return GFX_CLICKABLE_HELD;
  }
  if (is_hovered && (!gfx_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT) && !gfx_mouse_button_is_down(GLFW_MOUSE_BUTTON_LEFT))) {
    gfx_rect_render(pos, size, hover_color ? hover_color_rgb : color, props.border_color, border_width, props.corner_radius, 0.0f);
    return GFX_CLICKABLE_HELD;
  }
  gfx_rect_render(pos, size, color, props.border_color, border_width, props.corner_radius, 0.0f);
  return GFX_CLICKABLE_IDLE;
}

gfx_clickable_item_state gfx_internal_div_container(vec2s pos, vec2s size, gfx_element_props props, gfx_color color, float border_width, bool click_color, bool hover_color) {
  if (gfx_internal_item_should_cull((gfx_aabb){ .pos = pos, .size = size }, true)) {
    return GFX_CLICKABLE_IDLE;
  }
  gfx_color hover_color_rgb = hover_color ? (props.hover_color.a == 0.0f ? gfx_color_brightness(color, 1.5) : props.hover_color) : color; 
  gfx_color held_color_rgb = click_color ? gfx_color_brightness(color, 1.8) : color; 

  bool is_hovered = gfx_hovered(pos, size);
  if (is_hovered && gfx_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)) {
    gfx_rect_render(pos, size, hover_color_rgb, props.border_color, border_width, props.corner_radius, 0.0f);
    return GFX_CLICKABLE_CLICKED;
  }
  if (is_hovered && gfx_mouse_button_is_down(GLFW_MOUSE_BUTTON_LEFT)) {
    gfx_rect_render(pos, size, held_color_rgb, props.border_color, border_width, props.corner_radius, 0.0f);
    return GFX_CLICKABLE_HELD;
  }
  if (is_hovered && (!gfx_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT) && !gfx_mouse_button_is_down(GLFW_MOUSE_BUTTON_LEFT))) {
    gfx_rect_render(pos, size, hover_color ? hover_color_rgb : color, props.border_color, border_width, props.corner_radius, 0.0f);
    return GFX_CLICKABLE_HOVERED;
  }
  gfx_rect_render(pos, size, color, props.border_color, border_width, props.corner_radius, 0.0f);
  return GFX_CLICKABLE_IDLE;
}

void gfx_internal_next_line_on_overflow(vec2s size, float xoffset) {
  if (!GFX_STATE.line_overflow) {
    return;
  }
  if (GFX_STATE.pos_ptr.x - GFX_STATE.current_div.aabb.pos.x + size.x > GFX_STATE.current_div.aabb.size.x) {
    GFX_STATE.pos_ptr.y += GFX_STATE.current_line_height;
    GFX_STATE.pos_ptr.x = GFX_STATE.current_div.aabb.pos.x + xoffset;
    GFX_STATE.current_line_height = 0;
  }
  if (size.y > GFX_STATE.current_line_height) {
    GFX_STATE.current_line_height = size.y;
  }
}

bool gfx_internal_item_should_cull(gfx_aabb item, bool cullable) {
  if (!cullable) {
    return false;
  }
  bool intersect = true;
  gfx_aabb window =  (gfx_aabb){ .pos = (vec2s){ 0, 0 }, .size = (vec2s){ GFX_STATE.dsp_w, GFX_STATE.dsp_h } };
  if (item.size.x == -1 || item.size.y == -1) {
    item.size.x = GFX_STATE.dsp_w;
    item.size.y = gfx_internal_get_current_font().font_size;
  }  
  if (item.pos.x + item.size.x <= window.pos.x || item.pos.x >= window.pos.x + window.size.x) {
    intersect = false;
  }
  if (item.pos.y + item.size.y <= window.pos.y || item.pos.y >= window.pos.y + window.size.y) {
    intersect = false;
  }
  return !intersect && GFX_STATE.current_div.id == GFX_STATE.scrollbar_div.id;
}

void gfx_internal_draw_scrollbar_on(gfx_div *div) {
  gfx_next_line();
  if (GFX_STATE.current_div.id == div->id) {
    GFX_STATE.scrollbar_div = *div;
    gfx_div *selected = div;
    float scroll = *GFX_STATE.scroll_ptr;
    gfx_element_props props = gfx_internal_get_props_for(GFX_STATE.theme.scrollbar_props);
    selected->total_area.x = GFX_STATE.pos_ptr.x;
    selected->total_area.y = GFX_STATE.pos_ptr.y + GFX_STATE.div_props.corner_radius;
    if (*GFX_STATE.scroll_ptr < -((div->total_area.y - *GFX_STATE.scroll_ptr) - div->aabb.pos.y - div->aabb.size.y) && *GFX_STATE.scroll_velocity_ptr < 0 && GFX_STATE.theme.div_smooth_scroll) {
      *GFX_STATE.scroll_velocity_ptr = 0;
      *GFX_STATE.scroll_ptr = -((div->total_area.y - *GFX_STATE.scroll_ptr) - div->aabb.pos.y - div->aabb.size.y);
    }
    float total_area = selected->total_area.y - scroll;
    float visible_area = selected->aabb.size.y + selected->aabb.pos.y;
    if (total_area > visible_area) {
      const float min_scrollbar_height = 20;
      float area_mapped = visible_area / total_area;
      float scroll_mapped = (-1 * scroll) / total_area;
      float scrollbar_height = GFX_MAX((selected->aabb.size.y * area_mapped - props.margin_top * 2), min_scrollbar_height);
      gfx_aabb scrollbar_area = (gfx_aabb){
        .pos = (vec2s){
          selected->aabb.pos.x + selected->aabb.size.x - GFX_STATE.theme.scrollbar_width - props.margin_right - GFX_STATE.div_props.padding - GFX_STATE.div_props.border_width,
          GFX_MIN((selected->aabb.pos.y + selected->aabb.size.y * scroll_mapped + props.margin_top + GFX_STATE.div_props.padding + GFX_STATE.div_props.border_width + GFX_STATE.div_props.corner_radius),
              visible_area - scrollbar_height)},
        .size = (vec2s){
          GFX_STATE.theme.scrollbar_width,
          scrollbar_height - GFX_STATE.div_props.border_width * 2 - GFX_STATE.div_props.corner_radius * 2},
      };
      vec2s cursorpos = (vec2s){ gfx_get_mouse_x(), gfx_get_mouse_y() };
      if (gfx_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT) && gfx_hovered(scrollbar_area.pos, scrollbar_area.size)) {
        GFX_STATE.drag_state.is_dragging = true;
        GFX_STATE.drag_state.start_cursor_pos = cursorpos;
        GFX_STATE.drag_state.start_scroll = *GFX_STATE.scroll_ptr;
      } 
      if (GFX_STATE.drag_state.is_dragging) {
        float cursor_delta = (cursorpos.y - GFX_STATE.drag_state.start_cursor_pos.y);
        float new_scroll = GFX_STATE.drag_state.start_scroll - cursor_delta * (total_area / visible_area);
        *GFX_STATE.scroll_ptr = new_scroll;
        if (*GFX_STATE.scroll_ptr > 0) {
          *GFX_STATE.scroll_ptr = 0;
        } 
        else if (*GFX_STATE.scroll_ptr < -(total_area - visible_area)) {
          *GFX_STATE.scroll_ptr = -(total_area - visible_area);
        }
      }
      if (gfx_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)) {
        GFX_STATE.drag_state.is_dragging = false;
      }
      gfx_rect_render(scrollbar_area.pos, scrollbar_area.size, props.color, props.border_color, props.border_width, props.corner_radius, 0.0f);
    }
  }
}

void gfx_internal_input_field(gfx_input_field *input, gfx_input_field_type type, const char *file, int line) {
  if (!input->buf) {
    return;
  }
  if (!input->init) {
    gfx_input_field_unselect_all(input);
    input->init = true;
  }
  gfx_element_props props = gfx_internal_get_props_for(GFX_STATE.theme.input_field_props);
  gfx_font font = gfx_internal_get_current_font();
  GFX_STATE.pos_ptr.x += props.margin_left; 
  GFX_STATE.pos_ptr.y += props.margin_top; 
  float wrap_point = GFX_STATE.pos_ptr.x + input->width - props.padding;
  if (input->selected) {
    if (gfx_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT) && (gfx_get_mouse_x_delta() == 0 && gfx_get_mouse_y_delta() == 0)) {
      gfx_text_props selected_props = gfx_text_render((vec2s){ GFX_STATE.pos_ptr.x + props.padding, GFX_STATE.pos_ptr.y + props.padding }, input->buf, font, GFX_NO_COLOR, wrap_point, (vec2s){ gfx_get_mouse_x(), gfx_get_mouse_y() }, true, false, -1, -1);
      input->cursor_index = selected_props.rendered_count;
      gfx_input_field_unselect_all(input);
      input->mouse_selection_end = input->cursor_index;
      input->mouse_selection_start = input->cursor_index;
    } 
    else if (gfx_mouse_button_is_down(GLFW_MOUSE_BUTTON_LEFT) && (gfx_get_mouse_x_delta() != 0 || gfx_get_mouse_y_delta() != 0)) {
      if (input->mouse_dir == 0) {
        input->mouse_dir = (gfx_get_mouse_x_delta() < 0) ? -1 : 1;
        input->mouse_selection_end = input->cursor_index;
        input->mouse_selection_start = input->cursor_index;
      }
      gfx_text_props selected_props = gfx_text_render((vec2s){ GFX_STATE.pos_ptr.x + props.padding, GFX_STATE.pos_ptr.y + props.padding }, input->buf, font, GFX_NO_COLOR, wrap_point, (vec2s){ gfx_get_mouse_x(), gfx_get_mouse_y() }, true, false, -1, -1);
      input->cursor_index = selected_props.rendered_count;
      if (input->mouse_dir == -1) {
        input->mouse_selection_start = input->cursor_index;
      }
      else if (input->mouse_dir == 1) {
        input->mouse_selection_end = input->cursor_index;
      }
      input->selection_start = input->mouse_selection_start;
      input->selection_end = input->mouse_selection_end;
      if (input->mouse_selection_start == input->mouse_selection_end) {
        input->mouse_dir = (gfx_get_mouse_x_delta() < 0) ? -1 : 1;
      }
    } 
    else if (gfx_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)){
      input->mouse_dir = 0;
    } 
    if (gfx_char_event_occur().happened && gfx_char_event_occur().char_code >= 0 && gfx_char_event_occur().char_code <= 127 &&
      strlen(input->buf) + 1 <= input->buf_size && (input->max_chars ? strlen(input->buf) + 1 <= input->max_chars : true)) { 
      if (input->insert_override_callback) {
        input->insert_override_callback(input);
      } 
      else {
        if (input->selection_start != -1) {
          int start = input->selection_dir != 0 ?  input->selection_start : input->selection_start - 1;
          int end = input->selection_end;

          gfx_internal_remove_substr_str(input->buf, start, end);

          input->cursor_index = input->selection_start;
          gfx_input_field_unselect_all(input);
        }
        gfx_input_insert_char_idx(input, gfx_char_event_occur().char_code, input->cursor_index++);
      }
    }
    if (gfx_key_event_occur().happened && gfx_key_event_occur().pressed) {
      switch(gfx_key_event_occur().key_code) {
        case GLFW_KEY_BACKSPACE: {
          if (input->selection_start != -1) {
            int start = input->selection_dir != 0 ?  input->selection_start : input->selection_start - 1;
            int end = input->selection_end;
            gfx_internal_remove_substr_str(input->buf, start, end);
            input->cursor_index = input->selection_start;
            gfx_input_field_unselect_all(input);
          }
          else {
            if (input->cursor_index - 1 < 0) {
              break;
            }
            gfx_internal_remove_i_str(input->buf, input->cursor_index - 1);
            input->cursor_index--;
          }
          break;
        }
        case GLFW_KEY_LEFT: {
          if (input->cursor_index - 1 < 0 ) {
            if (!gfx_key_is_down(GLFW_KEY_LEFT_SHIFT)) {
              gfx_input_field_unselect_all(input);
            }
            break;
          }
          if (gfx_key_is_down(GLFW_KEY_LEFT_SHIFT)) {
            if(input->selection_end == -1) {
              input->selection_end = input->cursor_index - 1;
              input->selection_dir = -1;
            }
            input->cursor_index--;
            if (input->selection_dir == 1) {
              if (input->cursor_index != input->selection_start) {
                input->selection_end = input->cursor_index - 1;
              } 
              else { 
                gfx_input_field_unselect_all(input);
              }
            } 
            else {
              input->selection_start = input->cursor_index;
            }
          } 
          else {
            if (input->selection_end == -1) {
              input->cursor_index--;
            }
            gfx_input_field_unselect_all(input);
          }
          break;
        }
        case GLFW_KEY_RIGHT: {
          if(input->cursor_index + 1 > strlen(input->buf)){
            if (!gfx_key_is_down(GLFW_KEY_LEFT_SHIFT)) {
              gfx_input_field_unselect_all(input);
            }
            break;
          }
          if(gfx_key_is_down(GLFW_KEY_LEFT_SHIFT)) {
            if (input->selection_start == -1) {
              input->selection_start = input->cursor_index;
              input->selection_dir = 1;
            }
            if (input->selection_dir == -1) {
              input->cursor_index++;
              if (input->cursor_index - 1 != input->selection_end) {
                input->selection_start = input->cursor_index;
              } 
              else {
                gfx_input_field_unselect_all(input);
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
            gfx_input_field_unselect_all(input);
          }
          break;
        }
        case GLFW_KEY_ENTER: {
          break;
        }
        case GLFW_KEY_TAB: {
          if (strlen(input->buf) + 1 <= input->buf_size && (input->max_chars ? strlen(input->buf) + 1 <= input->max_chars : true)) {
            for (unsigned int i = 0; i < 2; i++) {
              gfx_internal_insert_i_str(input->buf, ' ', input->cursor_index++);
            }
          }
          break;
        }
        case GLFW_KEY_A: {
          if (!gfx_key_is_down(GLFW_KEY_LEFT_CONTROL)) {
            break;
          }
          bool selected_all = input->selection_start == 0 && input->selection_end == strlen(input->buf);
          if (selected_all) {
            gfx_input_field_unselect_all(input);
          } 
          else {
            input->selection_start = 0;
            input->selection_end = strlen(input->buf);
          }
          break;
        }
        case GLFW_KEY_C: {
          if (!gfx_key_is_down(GLFW_KEY_LEFT_CONTROL)) {
            break;
          }
          char selection[strlen(input->buf)];
          memset(selection, 0, strlen(input->buf));
          gfx_internal_substr_str(input->buf, input->selection_start, input->selection_end, selection);
          clipboard_set_text(GFX_STATE.clipboard, selection);
          break;
        }
        case GLFW_KEY_V: {
          if (!gfx_key_is_down(GLFW_KEY_LEFT_CONTROL)) {
            break;
          }
          int length;
          const char *clipboard_content = clipboard_text_ex(GFX_STATE.clipboard, &length, LCB_CLIPBOARD);
          if (strlen(input->buf) + length > input->buf_size || (input->max_chars ? strlen(input->buf) + length > input->max_chars : false)) {
            break;
          }
          gfx_input_insert_str_idx(input, clipboard_content, length, input->cursor_index);
          input->cursor_index += strlen(clipboard_content);
          break;
        }
        case GLFW_KEY_X: {
          if (!gfx_key_is_down(GLFW_KEY_LEFT_CONTROL)) {
            break;
          }
          char selection[strlen(input->buf)];
          memset(selection, 0, strlen(input->buf));
          gfx_internal_substr_str(input->buf, input->selection_start, input->selection_end, selection);
          clipboard_set_text(GFX_STATE.clipboard, selection);
          int start = input->selection_dir != 0 ?  input->selection_start : input->selection_start - 1;
          gfx_internal_remove_substr_str(input->buf, start, input->selection_end);
          input->cursor_index = input->selection_start;
          gfx_input_field_unselect_all(input);
          break;
        }
        default: {
          break;
        }
      }
    }
    if (input->key_callback) {
      input->key_callback(input);
    }
  }
  gfx_text_props textprops =  gfx_text_render((vec2s){ GFX_STATE.pos_ptr.x + props.padding, GFX_STATE.pos_ptr.y + props.padding }, input->buf, font, GFX_NO_COLOR, wrap_point, (vec2s){ -1, -1 }, true, false, -1, -1); 
  if (!input->retain_height) {
    input->height = (input->start_height) ? GFX_MAX(input->start_height, textprops.height) : (textprops.height ? textprops.height : get_max_char_height_font(font)); 
  }
  else {
    input->height = (input->start_height) ? input->start_height : get_max_char_height_font(font);
  }
  gfx_internal_next_line_on_overflow((vec2s){ input->width + props.padding * 2.0f + props.margin_right + props.margin_left, input->height + props.padding * 2.0f + props.margin_bottom + props.margin_top }, GFX_STATE.div_props.border_width);
  gfx_aabb input_aabb = (gfx_aabb){ .pos = GFX_STATE.pos_ptr, .size = (vec2s){ input->width + props.padding * 2.0f, input->height + props.padding * 2.0f } };
  gfx_clickable_item_state inputfield = gfx_internal_button(file, line, input_aabb.pos, input_aabb.size, props, props.color, props.border_width, false, false);
  if (gfx_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT) && input->selected && inputfield == GFX_CLICKABLE_IDLE) {
    input->selected = false;
    GFX_STATE.input_grabbed = false;
    gfx_input_field_unselect_all(input);
  } 
  else if (inputfield == GFX_CLICKABLE_CLICKED) {
    input->selected = true;
    GFX_STATE.input_grabbed = true;
    gfx_text_props selected_props = gfx_text_render((vec2s){ GFX_STATE.pos_ptr.x + props.padding, GFX_STATE.pos_ptr.y + props.padding }, input->buf, font, GFX_NO_COLOR, wrap_point, (vec2s){ gfx_get_mouse_x(), gfx_get_mouse_y() }, true, false, -1, -1);
    input->cursor_index = selected_props.rendered_count;
  }
  if (input->selected) {
    char selected_buf[strlen(input->buf)];
    strncpy(selected_buf, input->buf, input->cursor_index);
    selected_buf[input->cursor_index] = '\0';
    gfx_text_props selected_props =  gfx_text_render((vec2s){ GFX_STATE.pos_ptr.x + props.padding, gfx_get_mouse_y() + props.padding }, selected_buf, font, GFX_NO_COLOR, wrap_point, (vec2s){ -1, -1 }, true, false, -1, -1);
    vec2s cursor_pos = { (strlen(input->buf) > 0) ? selected_props.end_x : GFX_STATE.pos_ptr.x + props.padding, GFX_STATE.pos_ptr.y + props.padding + (selected_props.height - get_max_char_height_font(font)) }; 
    if (input->selection_start == -1 || input->selection_end == -1) {
      gfx_rect_render(cursor_pos, (vec2s){ 1, get_max_char_height_font(font) }, props.text_color, GFX_NO_COLOR, 0.0f, 0.0f, 0.0f);
    } 
    else {
      gfx_text_render((vec2s){GFX_STATE.pos_ptr.x + props.padding, GFX_STATE.pos_ptr.y + props.padding}, input->buf, font, (gfx_color){ 255, 255, 255, 80 }, wrap_point, (vec2s){ -1, -1 }, false, true, input->selection_start, input->selection_end);
    }
  }
  gfx_text_render((vec2s){GFX_STATE.pos_ptr.x + props.padding, GFX_STATE.pos_ptr.y + props.padding}, (!strlen(input->buf) && !input->selected) ? input->placeholder : input->buf, font, !strlen(input->buf) ? gfx_color_brightness(props.text_color, 0.75f) : props.text_color, wrap_point, (vec2s){-1, -1}, false, false, -1, -1); 
  GFX_STATE.pos_ptr.x += input->width + props.margin_right + props.padding * 2.0f;
  GFX_STATE.pos_ptr.y -= props.margin_top;
}

gfx_font gfx_internal_get_current_font() {
  return GFX_STATE.font_stack ? *GFX_STATE.font_stack : GFX_STATE.theme.font;
}

gfx_clickable_item_state gfx_internal_button_element_loc(void *text, const char *file, int line) {
  gfx_element_props props = gfx_internal_get_props_for(GFX_STATE.theme.button_props);
  float padding = props.padding;
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom = props.margin_bottom; 
  GFX_STATE.pos_ptr.x += margin_left;
  GFX_STATE.pos_ptr.y += margin_top;
  gfx_font font = gfx_internal_get_current_font();
  gfx_text_props text_props;
  text_props = gfx_internal_text_render_simple(GFX_STATE.pos_ptr, (const char*)text, font, GFX_NO_COLOR, true);
  gfx_color color = props.color;
  gfx_color text_color = gfx_hovered(GFX_STATE.pos_ptr, (vec2s){ text_props.width, text_props.height }) && props.hover_text_color.a != 0.0f ? props.hover_text_color : props.text_color;
  gfx_internal_next_line_on_overflow((vec2s){ text_props.width + padding * 2.0f + margin_right + margin_left, text_props.height + padding * 2.0f + margin_bottom + margin_top }, GFX_STATE.div_props.border_width);
  gfx_clickable_item_state ret = gfx_internal_button(file, line, GFX_STATE.pos_ptr, (vec2s){ text_props.width + padding * 2, text_props.height + padding * 2 }, props, color, props.border_width, true, true);
  gfx_internal_text_render_simple((vec2s){ GFX_STATE.pos_ptr.x + padding, GFX_STATE.pos_ptr.y + padding }, (const char *)text, font, text_color, false);
  GFX_STATE.pos_ptr.x += text_props.width + margin_right + padding * 2.0f;
  GFX_STATE.pos_ptr.y -= margin_top;
  return ret;
}

gfx_clickable_item_state gfx_internal_button_fixed_element_loc(void *text, float width, float height, const char *file, int line) {
  gfx_element_props props = gfx_internal_get_props_for(GFX_STATE.theme.button_props);
  float padding = props.padding;
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom = props.margin_bottom;
  gfx_font font = GFX_STATE.font_stack ? *GFX_STATE.font_stack : GFX_STATE.theme.font;
  gfx_text_props text_props; 
  text_props = gfx_internal_text_render_simple(GFX_STATE.pos_ptr, (const char *)text, font, GFX_NO_COLOR, true);
  gfx_color color = props.color;
  gfx_color text_color = gfx_hovered(GFX_STATE.pos_ptr, (vec2s){ text_props.width, text_props.height }) && props.hover_text_color.a != 0.0f ? props.hover_text_color : props.text_color;
  int render_width = ((width == -1) ? text_props.width : width);
  int render_height = ((height == -1) ? text_props.height : height);
  gfx_internal_next_line_on_overflow((vec2s){ render_width + padding * 2.0f + margin_right + margin_left, render_height + padding * 2.0f + margin_bottom + margin_top }, GFX_STATE.div_props.border_width); 
  GFX_STATE.pos_ptr.x += margin_left;
  GFX_STATE.pos_ptr.y += margin_top; 
  gfx_clickable_item_state ret = gfx_internal_button(file, line, GFX_STATE.pos_ptr, (vec2s){ render_width + padding * 2.0f, render_height + padding * 2.0f }, props, color, props.border_width, false, true);
  gfx_set_cull_end_x(GFX_STATE.pos_ptr.x + render_width + padding);
  gfx_internal_text_render_simple((vec2s){ GFX_STATE.pos_ptr.x + padding + ((width != -1) ? (width - text_props.width) / 2.0f : 0), GFX_STATE.pos_ptr.y + padding + ((height != -1) ? (height - text_props.height) / 2.0f : 0) }, (const char *)text, font, text_color, false);
  gfx_unset_cull_end_x();
  GFX_STATE.pos_ptr.x += render_width + margin_right + padding * 2.0f;
  GFX_STATE.pos_ptr.y -= margin_top;
  return ret;
}

gfx_clickable_item_state gfx_internal_checkbox_element_loc(void *text, bool *val, gfx_color tick_color, gfx_color tex_color, const char *file, int line) {
  gfx_font font = gfx_internal_get_current_font();
  gfx_element_props props = gfx_internal_get_props_for(GFX_STATE.theme.checkbox_props);
  float margin_left = props.margin_left;
  float margin_right = props.margin_right;
  float margin_top = props.margin_top;
  float margin_bottom = props.margin_bottom;
  float checkbox_size; 
  checkbox_size = gfx_text_dimension((const char*)text).y;
  gfx_internal_next_line_on_overflow((vec2s){ checkbox_size + margin_left + margin_right + props.padding * 2.0f, checkbox_size + margin_top + margin_bottom + props.padding * 2.0f }, GFX_STATE.div_props.border_width);
  GFX_STATE.pos_ptr.x += margin_left; 
  GFX_STATE.pos_ptr.y += margin_top;
  gfx_color checkbox_color = (*val) ? ((tick_color.a == 0) ? props.color : tick_color) : props.color;
  gfx_clickable_item_state checkbox = gfx_internal_button(file, line, GFX_STATE.pos_ptr, (vec2s){ checkbox_size + props.padding * 2.0f, checkbox_size + props.padding * 2.0f }, props, checkbox_color, props.border_width, false, false);
  gfx_internal_text_render_simple((vec2s){ GFX_STATE.pos_ptr.x + checkbox_size + props.padding * 2.0f + margin_right, GFX_STATE.pos_ptr.y + props.padding }, (const char *)text, font, props.text_color, false);
  if (checkbox == GFX_CLICKABLE_CLICKED) {
    *val = !*val;
  }
  if (*val) {
    gfx_image_render((vec2s){ GFX_STATE.pos_ptr.x + props.padding, GFX_STATE.pos_ptr.y + props.padding }, tex_color, (gfx_texture){ .id = GFX_STATE.tex_tick.id, .width = (unsigned int)(checkbox_size), .height = (unsigned int)(checkbox_size)}, (gfx_color){ 0.0f, 0.0f, 0.0f, 0.0f }, 0, props.corner_radius, 0.0f, 0.0f, 0.0f, 1.0f, true, false);
  }
  GFX_STATE.pos_ptr.x += checkbox_size + props.padding * 2.0f + margin_right + gfx_text_dimension((const char *)text).x + margin_right;
  GFX_STATE.pos_ptr.y -= margin_top;
  return checkbox;
}

void gfx_internal_dropdown_menu_item_loc(void **items, void *placeholder, unsigned int item_count, float width, float height, int *selected_index, bool *opened, const char *file, int line) {
  gfx_element_props props = gfx_internal_get_props_for(GFX_STATE.theme.button_props);
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom = props.margin_bottom;
  float padding = props.padding;
  gfx_font font = gfx_internal_get_current_font();
  int placeholder_strlen = strlen((const char *)placeholder);
  void *button_text = (void *)((*selected_index != -1) ? items[*selected_index] : (placeholder_strlen != 0) ? placeholder : "Select");
  gfx_text_props text_props;  
  text_props = gfx_internal_text_render_simple((vec2s){ GFX_STATE.pos_ptr.x + padding, GFX_STATE.pos_ptr.y + padding }, (const char *)button_text, font, props.text_color, true);
  float item_height = get_max_char_height_font(font) + ((*opened) ? height + padding * 4.0f + margin_top : padding * 2.0f); 
  gfx_internal_next_line_on_overflow((vec2s){ width + padding * 2.0f + margin_right, item_height + margin_top + margin_bottom }, 0.0f);
  GFX_STATE.pos_ptr.x += margin_left;
  GFX_STATE.pos_ptr.y += margin_top;
  vec2s button_pos = GFX_STATE.pos_ptr;
  gfx_clickable_item_state dropdown_button = gfx_internal_button(file, line, GFX_STATE.pos_ptr, (vec2s){ (float)width + padding * 2.0f, (float)text_props.height + padding * 2.0f },  props, props.color, props.border_width, false, true);
  gfx_internal_text_render_simple((vec2s){ GFX_STATE.pos_ptr.x + padding, GFX_STATE.pos_ptr.y + padding }, (const char*)button_text, font, props.text_color, false);
  vec2s image_size = (vec2s){ 20, 10 };
  gfx_image_render((vec2s){ GFX_STATE.pos_ptr.x + width + padding - image_size.x, GFX_STATE.pos_ptr.y + ((text_props.height + padding * 2) - image_size.y) / 2.0f }, props.text_color, (gfx_texture){ .id = GFX_STATE.tex_arrow_down.id, .width = (uint32_t)image_size.x, .height = (uint32_t)image_size.y }, GFX_NO_COLOR, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, true, false);
  if (dropdown_button == GFX_CLICKABLE_CLICKED) {
    *opened = !*opened;
  }
  if (*opened) {
    if ((gfx_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT) && dropdown_button != GFX_CLICKABLE_CLICKED) || (!gfx_input_grabbed() && gfx_key_went_down(GLFW_KEY_ESCAPE))) {
      *opened = false;
    }
    gfx_element_props div_props = gfx_get_theme().div_props;
    div_props.corner_radius = props.corner_radius;
    div_props.border_color = props.border_color;
    div_props.border_width = props.border_width;
    div_props.color = props.color;
    gfx_push_style_props(div_props);
    gfx_div_begin(((vec2s){ GFX_STATE.pos_ptr.x, GFX_STATE.pos_ptr.y + text_props.height + padding * 2.0f }), ((vec2s){ width + props.padding * 2.0f, height + props.padding * 2.0f }), false);
    gfx_pop_style_props();
    for (unsigned int i = 0; i < item_count; i++) {
      gfx_element_props text_props = gfx_get_theme().text_props;
      text_props.text_color = props.text_color;
      bool hovered = gfx_hovered((vec2s){ GFX_STATE.pos_ptr.x + text_props.margin_left, GFX_STATE.pos_ptr.y + text_props.margin_top }, (vec2s){ width + props.padding * 2.0f, gfx_get_theme().font.font_size });
      if (hovered) {
        gfx_rect_render(((vec2s){ GFX_STATE.pos_ptr.x, GFX_STATE.pos_ptr.y }), (vec2s){ width + props.padding * 2.0f, gfx_get_theme().font.font_size + props.margin_top }, gfx_color_brightness(div_props.color, 1.2f), GFX_NO_COLOR, 0.0f, 0.0f, 0.0f);
      }
      if (hovered && gfx_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)) {
        *selected_index = i;
      }
      gfx_push_style_props(text_props);
      gfx_text(items[i]);
      gfx_pop_style_props();
      gfx_next_line();
    }
    gfx_div_end();
  }
  GFX_STATE.pos_ptr.x += width + padding * 2.0f + margin_right;
  GFX_STATE.pos_ptr.y -= margin_top;
  gfx_push_style_props(props);
}

int gfx_internal_menu_item_list_item_loc(void **items, unsigned int item_count, int selected_index, gfx_menu_item_callback per_cb, bool vertical, const char *file, int line) {
  gfx_element_props props = gfx_internal_get_props_for(GFX_STATE.theme.button_props);
  float padding = props.padding;
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom = props.margin_bottom; 
  gfx_color color = props.color;
  gfx_font font = gfx_internal_get_current_font();
  gfx_text_props text_props[item_count];
  float width = 0;
  for (unsigned int i  = 0; i < item_count; i++) {
    text_props[i] = gfx_internal_text_render_simple((vec2s){GFX_STATE.pos_ptr.x, GFX_STATE.pos_ptr.y + margin_top}, (const char *)items[i], font, props.text_color, true);
    width += text_props[i].width + padding * 2.0f;
  }
  gfx_internal_next_line_on_overflow((vec2s){ width + padding * 2.0f + margin_right + margin_left, font.font_size + padding * 2.0f + margin_bottom + margin_top }, GFX_STATE.div_props.border_width);
  GFX_STATE.pos_ptr.y += margin_top; 
  GFX_STATE.pos_ptr.x += margin_left;
  unsigned int element_width = 0;
  unsigned int clicked_item = -1;
  for (unsigned int i = 0; i < item_count; i++) {
    gfx_element_props props = GFX_STATE.theme.button_props;
    props.margin_left = 0;
    props.margin_right = 0;
    gfx_element_props button_props = GFX_STATE.theme.button_props;
    gfx_push_style_props(props);
    if (i == selected_index) {
      props.color = gfx_color_brightness(props.color, 1.2); 
    }
    gfx_push_style_props(props);
    if (gfx_button_loc((const char *)items[i], file, line) == GFX_CLICKABLE_CLICKED) {
      clicked_item = i;  
    } 
    gfx_pop_style_props();
    per_cb(&i);
  } 
  gfx_internal_next_line_on_overflow((vec2s){ element_width + margin_right, font.font_size + margin_top + margin_bottom }, GFX_STATE.div_props.border_width);
  GFX_STATE.pos_ptr.y -= margin_top;
  return clicked_item;
}

void gfx_internal_remove_i_str(char *str, unsigned int index) {
  int len = strlen(str);
  if (index >= 0 && index < len) {
    for (unsigned int i = index; i < len - 1; i++) {
      str[i] = str[i + 1];
    }
    str[len - 1] = '\0';
  }
}

void gfx_internal_remove_substr_str(char *str, unsigned int start_index, unsigned int end_index) {
  unsigned int len = strlen(str);
  memmove(str + start_index, str + end_index + 1, len - end_index);
  str[len - (end_index - start_index) + 1] = '\0'; 
}

void gfx_internal_insert_i_str(char *str, char ch, unsigned int index) {
  unsigned int len = strlen(str);
  if (index < 0 || index > len) {
    utl_error_func("Invalid string index for inserting", utl_user_defined_data);
    return;
  }
  for (unsigned int i = len; i > index; i--) {
    str[i] = str[i - 1];
  }
  str[index] = ch;
  str[len + 1] = '\0'; 
}

void gfx_internal_insert_str_str(char *source, const char *insert, unsigned int index) {
  unsigned int source_len = strlen(source);
  unsigned int insert_len = strlen(insert);
  if (index < 0 || index > source_len) {
    utl_error_func("Index for inserting out of bounds", utl_user_defined_data);
    return;
  }
  memmove(source + index + insert_len, source + index, source_len - index + 1);
  memcpy(source + index, insert, insert_len);
}

void gfx_internal_substr_str(const char *str, unsigned int start_index, unsigned int end_index, char *substring) {
  unsigned int substring_length = end_index - start_index + 1; 
  strncpy(substring, str + start_index, substring_length);
  substring[substring_length] = '\0';
}

int gfx_internal_map_vals(int value, int from_min, int from_max, int to_min, int to_max) {
  return (value - from_min) * (to_max - to_min) / (from_max - from_min) + to_min;
}

void gfx_internal_glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (action != GLFW_RELEASE) {
    if (!GFX_STATE.input.keyboard.keys[key]) {
      GFX_STATE.input.keyboard.keys[key] = true;
    }
  }  
  else {
    GFX_STATE.input.keyboard.keys[key] = false;
  }
  GFX_STATE.input.keyboard.keys_changed[key] = (action != GLFW_REPEAT);
  for (unsigned int i = 0; i < GFX_STATE.input.key_cb_count; i++) {
    GFX_STATE.input.key_cbs[i](window, key, scancode, action, mods);
  }
  GFX_STATE.key_ev.happened = true;
  GFX_STATE.key_ev.pressed = action != GLFW_RELEASE;
  GFX_STATE.key_ev.key_code = key;
}

void gfx_internal_glfw_mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
  if (action != GLFW_RELEASE)  {
    if (!GFX_STATE.input.mouse.buttons_current[button]) {
      GFX_STATE.input.mouse.buttons_current[button] = true;
    }
  } 
  else {
    GFX_STATE.input.mouse.buttons_current[button] = false;
  }
  for (unsigned int i = 0; i < GFX_STATE.input.mouse_button_cb_count; i++) {
    GFX_STATE.input.mouse_button_cbs[i](window, button, action, mods);
  }
  GFX_STATE.mb_ev.happened = true;
  GFX_STATE.mb_ev.pressed = action != GLFW_RELEASE;
  GFX_STATE.mb_ev.button_code = button;
}

void gfx_internal_glfw_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  GFX_STATE.input.mouse.xscroll_delta = xoffset;
  GFX_STATE.input.mouse.yscroll_delta = yoffset;
  for (unsigned int i = 0; i< GFX_STATE.input.scroll_cb_count; i++) {
    GFX_STATE.input.scroll_cbs[i](window, xoffset, yoffset);
  }
  GFX_STATE.scr_ev.happened = true;
  GFX_STATE.scr_ev.x_offset = xoffset;
  GFX_STATE.scr_ev.y_offset = yoffset;
  gfx_div *selected_div = &GFX_STATE.selected_div;
  if (!selected_div->scrollable) {
    return;
  }
  if ((GFX_STATE.grabbed_div.id != -1 && selected_div->id != GFX_STATE.grabbed_div.id)) {
    return;
  }
  if (yoffset < 0.0f) {
    if (selected_div->total_area.y > (selected_div->aabb.size.y + selected_div->aabb.pos.y)) { 
      if (GFX_STATE.theme.div_smooth_scroll) {
        *GFX_STATE.scroll_velocity_ptr -= GFX_STATE.theme.div_scroll_acceleration;
        GFX_STATE.div_velocity_accelerating = true;
      } 
      else {
        *GFX_STATE.scroll_ptr -= GFX_STATE.theme.div_scroll_amount_px;
      }
    } 
  } 
  else if (yoffset > 0.0f) {
    if(*GFX_STATE.scroll_ptr) {
      if(GFX_STATE.theme.div_smooth_scroll) {
        *GFX_STATE.scroll_velocity_ptr += GFX_STATE.theme.div_scroll_acceleration;
        GFX_STATE.div_velocity_accelerating = false;
      } else {
        *GFX_STATE.scroll_ptr += GFX_STATE.theme.div_scroll_amount_px;
      }
    }        
  }
  if(GFX_STATE.theme.div_smooth_scroll) {
    *GFX_STATE.scroll_velocity_ptr = GFX_MIN(GFX_MAX(*GFX_STATE.scroll_velocity_ptr, -GFX_STATE.theme.div_scroll_max_velocity), GFX_STATE.theme.div_scroll_max_velocity);
  }
}

void gfx_internal_glfw_cursor_callback(GLFWwindow *window, double xpos, double ypos) {
  (void)window;
  gfx_mouse *mouse = &GFX_STATE.input.mouse;
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
  for(unsigned int i = 0; i < GFX_STATE.input.cursor_pos_cb_count; i++) {
    GFX_STATE.input.cursor_pos_cbs[i](window, xpos, ypos);
  }
  GFX_STATE.cp_ev.happened = true;
  GFX_STATE.cp_ev.x = xpos;
  GFX_STATE.cp_ev.y = ypos;
}

void gfx_internal_glfw_char_callback(GLFWwindow *window, unsigned int charcode) {
  (void)window;
  GFX_STATE.ch_ev.char_code = charcode;
  GFX_STATE.ch_ev.happened = true;
}

void gfx_internal_glfw_window_size_callback(GLFWwindow *window, int width, int height) {
  (void)window;
  glViewport(0, 0, width, height);
  gfx_resize_display(width, height);
}

void gfx_internal_update_input() {
  memcpy(GFX_STATE.input.mouse.buttons_last, GFX_STATE.input.mouse.buttons_current, sizeof(bool) * GFX_MAX_MOUSE_BUTTONS);
}

void gfx_internal_clear_events() {
  GFX_STATE.key_ev.happened = false;
  GFX_STATE.mb_ev.happened = false;
  GFX_STATE.cp_ev.happened = false;
  GFX_STATE.scr_ev.happened = false;
  GFX_STATE.ch_ev.happened = false;
  GFX_STATE.input.mouse.xpos_delta = 0;
  GFX_STATE.input.mouse.ypos_delta = 0;
}

unsigned long gfx_internal_djb2_hash(unsigned long hash, const void *buf, unsigned int size) {
  unsigned char *bytes = (unsigned char *)buf;
  char c;
  while ((c = *bytes++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash;
}

void gfx_internal_props_stack_create(gfx_props_stack *stack) {
  stack->data = (gfx_element_props *)malloc(GFX_UI_STACK_INIT_CAP * sizeof(gfx_element_props));
  if(!stack->data) {
    utl_error_func("Failed to allocate memory for stack data structure", utl_user_defined_data);
  }
  stack->count = 0;
  stack->cap = GFX_UI_STACK_INIT_CAP;
}

void gfx_internal_props_stack_resize(gfx_props_stack *stack, unsigned int newcap) {
  gfx_element_props* newdata = (gfx_element_props *)realloc(stack->data, newcap * sizeof(gfx_element_props));
  if(!newdata) {
    utl_error_func("Failed to reallocate memory for stack datastructure", utl_user_defined_data);
  }
  stack->data = newdata;
  stack->cap = newcap;
}

void gfx_internal_props_stack_push(gfx_props_stack *stack, gfx_element_props props) {
  if(stack->count == stack->cap) {
    gfx_internal_props_stack_resize(stack, stack->cap * 2);
  }
  stack->data[stack->count++] = props;
}

gfx_element_props gfx_internal_props_stack_pop(gfx_props_stack *stack) {
  gfx_element_props val = stack->data[--stack->count];
  if(stack->count > 0 && stack->count == stack->cap / 4) {
    gfx_internal_props_stack_resize(stack, stack->cap / 2);
  }
  return val;
}

gfx_element_props gfx_internal_props_stack_peak(gfx_props_stack *stack) {
  return stack->data[stack->count - 1];
}

bool gfx_internal_props_stack_empty(gfx_props_stack *stack) {
  return stack->count == 0;
}

void gfx_internal_props_stack_clear(gfx_props_stack *stack) {
  while (stack->count > 0) {
    gfx_internal_props_stack_pop(stack);
  }
}

gfx_element_props gfx_internal_get_props_for(gfx_element_props props) {
  return (!gfx_internal_props_stack_empty(&GFX_STATE.props_stack)) ? gfx_internal_props_stack_peak(&GFX_STATE.props_stack) : props; 
}

