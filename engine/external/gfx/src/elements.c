#include "gfx/elements.h"
#include "gfx/inputs.h"
#include "gfx/stb.h"
#include "gfx/strman.h"
#include <string.h>

LfTextProps text_render_simple(vec2s pos, const char *text, LfFont font, LfColor font_color, bool no_render) {
  return lf_text_render(pos, text, font, font_color, -1, (vec2s){ -1, -1 }, no_render, false, -1, -1);
}

LfTextProps text_render_simple_wide(vec2s pos, const wchar_t *text, LfFont font, LfColor font_color, bool no_render) {
  return lf_text_render_wchar(pos, text, font, font_color, -1, (vec2s){ -1, -1 }, no_render, false, -1, -1);
}

LfClickableItemState button(const char *file, int32_t line, vec2s pos, vec2s size, LfUIElementProps props, LfColor color, float border_width,  bool click_color, bool hover_color) {
  return button_ex(file, line, pos, size, props, color, border_width, click_color, hover_color, (vec2s){ -1, -1 }); 
}

LfClickableItemState button_ex(const char *file, int32_t line, vec2s pos, vec2s size, LfUIElementProps props, LfColor color, float border_width, bool click_color, bool hover_color, vec2s hitbox_override) {
  uint64_t id = DJB2_INIT;
  id = djb2_hash(id, file, strlen(file));
  id = djb2_hash(id, &line, sizeof(line));
  if(state.element_id_stack != -1) {
    id = djb2_hash(id, &state.element_id_stack, sizeof(state.element_id_stack));
  }
  if(item_should_cull((LfAABB){.pos = pos, .size= size})) {
    return LF_IDLE;
  }
  LfColor hover_color_rgb = hover_color ? (props.hover_color.a == 0.0f ? lf_color_brightness(color, 1.2) : props.hover_color) : color; 
  LfColor held_color_rgb = click_color ? lf_color_brightness(color, 1.3) : color; 
  bool is_hovered = lf_hovered(pos, (vec2s){ hitbox_override.x != -1 ? hitbox_override.x : size.x, hitbox_override.y != -1 ? hitbox_override.y : size.y });
  if(state.active_element_id == 0) {
    if(is_hovered && lf_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT)) {
      state.active_element_id = id;
    }
  } 
  else if(state.active_element_id == id) {
    if(is_hovered && lf_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)) {
      lf_rect_render(pos, size, hover_color_rgb, props.border_color, border_width, props.corner_radius);
      state.active_element_id = 0;
      return LF_CLICKED;
    }
  }
  if(is_hovered && lf_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)) {
    state.active_element_id = 0;
  }
  if(is_hovered && lf_mouse_button_is_down(GLFW_MOUSE_BUTTON_LEFT)) {
    lf_rect_render(pos, size, held_color_rgb, props.border_color, border_width, props.corner_radius);
    return LF_HELD;
  }
  if(is_hovered && (!lf_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT) && !lf_mouse_button_is_down(GLFW_MOUSE_BUTTON_LEFT))) {
    lf_rect_render(pos, size, hover_color ? hover_color_rgb : color, props.border_color, border_width, props.corner_radius);
    return LF_HOVERED;
  }
  lf_rect_render(pos, size, color, props.border_color, border_width, props.corner_radius);
  return LF_IDLE;
}

LfClickableItemState div_container(vec2s pos, vec2s size, LfUIElementProps props, LfColor color, float border_width, bool click_color, bool hover_color) {
  if(item_should_cull((LfAABB){ .pos = pos, .size = size })) {
    return LF_IDLE;
  }
  LfColor hover_color_rgb = hover_color ? (props.hover_color.a == 0.0f ? lf_color_brightness(color, 1.5) : props.hover_color) : color; 
  LfColor held_color_rgb = click_color ? lf_color_brightness(color, 1.8) : color; 
  bool is_hovered = lf_hovered(pos, size);
  if(is_hovered && lf_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)) {
    lf_rect_render(pos, size, hover_color_rgb, props.border_color, border_width, props.corner_radius);
    return LF_CLICKED;
  }
  if(is_hovered && lf_mouse_button_is_down(GLFW_MOUSE_BUTTON_LEFT)) {
    lf_rect_render(pos, size, held_color_rgb, props.border_color, border_width, props.corner_radius);
    return LF_HELD;
  }
  if(is_hovered && (!lf_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT) && !lf_mouse_button_is_down(GLFW_MOUSE_BUTTON_LEFT))) {
    lf_rect_render(pos, size, hover_color ? hover_color_rgb : color, props.border_color, border_width, props.corner_radius);
    return LF_HOVERED;
  }
  lf_rect_render(pos, size, color, props.border_color, border_width, props.corner_radius);
  return LF_IDLE;
}

void next_line_on_overflow(vec2s size, float xoffset) {
  if(!state.line_overflow) {
    return;
  }
  if(state.pos_ptr.x - state.current_div.aabb.pos.x + size.x > state.current_div.aabb.size.x) {
    state.pos_ptr.y += state.current_line_height;
    state.pos_ptr.x = state.current_div.aabb.pos.x + xoffset;
    state.current_line_height = 0;
  }
  if(size.y > state.current_line_height) {
    state.current_line_height = size.y;
  }
}

bool item_should_cull(LfAABB item) {
  bool intersect = true;
  LfAABB window =  (LfAABB){.pos = (vec2s){0, 0}, .size = (vec2s){state.dsp_w, state.dsp_h}};
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

void draw_scrollbar_on(LfDiv *div) {
  lf_next_line();
  if (state.current_div.id == div->id) {
    state.scrollbar_div = *div;
    LfDiv *selected = div;
    float scroll = *state.scroll_ptr;
    LfUIElementProps props = get_props_for(state.theme.scrollbar_props);
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
      LfAABB scrollbar_area = (LfAABB){
        .pos = (vec2s){
          selected->aabb.pos.x + selected->aabb.size.x - state.theme.scrollbar_width - props.margin_right - state.div_props.padding - state.div_props.border_width,
          MIN((selected->aabb.pos.y + selected->aabb.size.y * scroll_mapped + props.margin_top + state.div_props.padding + state.div_props.border_width + state.div_props.corner_radius),
              visible_area - scrollbar_height)
        },
        .size = (vec2s){
          state.theme.scrollbar_width,
          scrollbar_height - state.div_props.border_width * 2 - state.div_props.corner_radius * 2
        },
      };
      vec2s cursorpos = (vec2s){ lf_get_mouse_x(), lf_get_mouse_y() };
      if (lf_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT) && lf_hovered(scrollbar_area.pos, scrollbar_area.size)) {
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
        } 
        else if (*state.scroll_ptr < -(total_area - visible_area)) {
          *state.scroll_ptr = -(total_area - visible_area);
        }
      }
      if (lf_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)) {
        state.drag_state.is_dragging = false;
      }
      lf_rect_render(scrollbar_area.pos, scrollbar_area.size, props.color, props.border_color, props.border_width, props.corner_radius);
    }
  }
}

void input_field(LfInputField *input, InputFieldType type, const char *file, int32_t line) {
  if(!input->buf) {
    return;
  }
  if(!input->_init) {
    lf_input_field_unselect_all(input);
    input->_init = true;
  }
  LfUIElementProps props = get_props_for(state.theme.inputfield_props);
  LfFont font = get_current_font();
  state.pos_ptr.x += props.margin_left; 
  state.pos_ptr.y += props.margin_top; 
  float wrap_point = state.pos_ptr.x + input->width - props.padding;
  if(input->selected) {
    if(lf_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT) && (lf_get_mouse_x_delta() == 0 && lf_get_mouse_y_delta() == 0)) {
      LfTextProps selected_props = lf_text_render((vec2s){ state.pos_ptr.x + props.padding, state.pos_ptr.y + props.padding }, input->buf, font, LF_NO_COLOR, wrap_point, (vec2s){ lf_get_mouse_x(), lf_get_mouse_y() }, true, false, -1, -1);
      input->cursor_index = selected_props.rendered_count;
      lf_input_field_unselect_all(input);
      input->mouse_selection_end = input->cursor_index;
      input->mouse_selection_start = input->cursor_index;
    } 
    else if(lf_mouse_button_is_down(GLFW_MOUSE_BUTTON_LEFT) && (lf_get_mouse_x_delta() != 0 || lf_get_mouse_y_delta() != 0)) {
      if(input->mouse_dir == 0) {
        input->mouse_dir = (lf_get_mouse_x_delta() < 0) ? -1 : 1;
        input->mouse_selection_end = input->cursor_index;
        input->mouse_selection_start = input->cursor_index;
      }
      LfTextProps selected_props = lf_text_render((vec2s){ state.pos_ptr.x + props.padding, state.pos_ptr.y + props.padding }, input->buf, font, LF_NO_COLOR, wrap_point, (vec2s){ lf_get_mouse_x(), lf_get_mouse_y() }, true, false, -1, -1);
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
        input->mouse_dir = (lf_get_mouse_x_delta() < 0) ? -1 : 1;
      }
    } 
    else if(lf_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)){
      input->mouse_dir = 0;
    } 
    if(lf_char_event().happened && lf_char_event().charcode >= 0 && lf_char_event().charcode <= 127 &&
      strlen(input->buf) + 1 <= input->buf_size && (input->max_chars ? strlen(input->buf) + 1 <= input->max_chars : true)) { 
      if(input->insert_override_callback) {
        input->insert_override_callback(input);
      } 
      else {
        if(input->selection_start != -1) {
          int start = input->selection_dir != 0 ?  input->selection_start : input->selection_start - 1;
          int end = input->selection_end;
          remove_substr_str(input->buf, start, end);
          input->cursor_index = input->selection_start;
          lf_input_field_unselect_all(input);
        }
        lf_input_insert_char_idx(input, lf_char_event().charcode, input->cursor_index++);
      }
    }
    if(lf_key_event().happened && lf_key_event().pressed) {
      switch(lf_key_event().keycode) {
        case GLFW_KEY_BACKSPACE: {
          if(input->selection_start != -1) {
            int start = input->selection_dir != 0 ?  input->selection_start : input->selection_start - 1;
            int end = input->selection_end;

            remove_substr_str(input->buf, start, end);

            input->cursor_index = input->selection_start;
            lf_input_field_unselect_all(input);
          }
          else {
            if(input->cursor_index - 1 < 0) break;
            remove_i_str(input->buf, input->cursor_index - 1);
            input->cursor_index--;
          }
          break;
        }
        case GLFW_KEY_LEFT: {
          if(input->cursor_index - 1 < 0 ) {
            if(!lf_key_is_down(GLFW_KEY_LEFT_SHIFT))
              lf_input_field_unselect_all(input);
            break;
          }
          if(lf_key_is_down(GLFW_KEY_LEFT_SHIFT)) {
            if(input->selection_end == -1) {
              input->selection_end = input->cursor_index - 1;
              input->selection_dir = -1;
            }
            input->cursor_index--;
            if(input->selection_dir == 1) {
              if(input->cursor_index != input->selection_start) {
                input->selection_end = input->cursor_index - 1;
              } else { 
                lf_input_field_unselect_all(input);
              }
            } else {
              input->selection_start = input->cursor_index;
            }
          } else {
            if(input->selection_end == -1)
              input->cursor_index--;
            lf_input_field_unselect_all(input);
          }
          break;
        }
        case GLFW_KEY_RIGHT: {
          if(input->cursor_index + 1 > strlen(input->buf)){
            if(!lf_key_is_down(GLFW_KEY_LEFT_SHIFT))
              lf_input_field_unselect_all(input);
            break;
          }
          if(lf_key_is_down(GLFW_KEY_LEFT_SHIFT)) {
            if(input->selection_start == -1) {
              input->selection_start = input->cursor_index;
              input->selection_dir = 1;
            }
            if(input->selection_dir == -1) {
              input->cursor_index++;
              if(input->cursor_index - 1 != input->selection_end) {
                input->selection_start = input->cursor_index;
              } else {
                lf_input_field_unselect_all(input);
              }
            } else {
              input->selection_end = input->cursor_index;
              input->cursor_index++;
            }
          } else {
            if(input->selection_end == -1)
              input->cursor_index++;
            lf_input_field_unselect_all(input);
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
          if(!lf_key_is_down(GLFW_KEY_LEFT_CONTROL)) break;
          bool selected_all = input->selection_start == 0 && input->selection_end == strlen(input->buf);
          if(selected_all) {
            lf_input_field_unselect_all(input);
          } else {
            input->selection_start = 0;
            input->selection_end = strlen(input->buf);
          }
          break;
        }
        case GLFW_KEY_C: {
          if(!lf_key_is_down(GLFW_KEY_LEFT_CONTROL)) break;
          char selection[strlen(input->buf)];
          memset(selection, 0, strlen(input->buf));
          substr_str(input->buf, input->selection_start, input->selection_end, selection);

          clipboard_set_text(state.clipboard, selection);
          break;
        }
        case GLFW_KEY_V: {
          if(!lf_key_is_down(GLFW_KEY_LEFT_CONTROL)) break;
          int32_t length;
          const char* clipboard_content = clipboard_text_ex(state.clipboard, &length, LCB_CLIPBOARD);
          if(strlen(input->buf) + length > input->buf_size || (input->max_chars ? strlen(input->buf) + length > input->max_chars : false)) break;

          lf_input_insert_str_idx(input, clipboard_content, length, input->cursor_index);
          input->cursor_index += strlen(clipboard_content);
          break;

        }
        case GLFW_KEY_X: {
          if (!lf_key_is_down(GLFW_KEY_LEFT_CONTROL)) break;
          char selection[strlen(input->buf)];
          memset(selection, 0, strlen(input->buf));
          substr_str(input->buf, input->selection_start, input->selection_end, selection);

          clipboard_set_text(state.clipboard, selection);

          int start = input->selection_dir != 0 ?  input->selection_start : input->selection_start - 1;
          remove_substr_str(input->buf, start, input->selection_end);
          input->cursor_index = input->selection_start;
          lf_input_field_unselect_all(input);
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

  LfTextProps textprops =  lf_text_render((vec2s){ state.pos_ptr.x + props.padding, state.pos_ptr.y + props.padding }, input->buf, font, LF_NO_COLOR, wrap_point, (vec2s){ -1, -1 }, true, false, -1, -1); 
  if(!input->retain_height) {
    input->height = (input->start_height) ? MAX(input->start_height, textprops.height) : (textprops.height ? textprops.height : get_max_char_height_font(font)); 
  } 
  else {
    input->height = (input->start_height) ? input->start_height : get_max_char_height_font(font);
  }
  next_line_on_overflow((vec2s){ input->width + props.padding * 2.0f + props.margin_right + props.margin_left, input->height + props.padding * 2.0f + props.margin_bottom + props.margin_top }, state.div_props.border_width);
  LfAABB input_aabb = (LfAABB){ .pos = state.pos_ptr, .size = (vec2s){ input->width + props.padding * 2.0f, input->height + props.padding * 2.0f } };
  LfClickableItemState inputfield = button(file, line, input_aabb.pos, input_aabb.size, props, props.color, props.border_width, false, false);
  if(lf_mouse_button_went_down(GLFW_MOUSE_BUTTON_LEFT) && input->selected && inputfield == LF_IDLE) {
    input->selected = false;
    state.input_grabbed = false;
    lf_input_field_unselect_all(input);
  } 
  else if(inputfield == LF_CLICKED) {
    input->selected = true;
    state.input_grabbed = true;
    LfTextProps selected_props = lf_text_render((vec2s){ state.pos_ptr.x + props.padding, state.pos_ptr.y + props.padding }, input->buf, font, LF_NO_COLOR, wrap_point, (vec2s){ lf_get_mouse_x(), lf_get_mouse_y() }, true, false, -1, -1);
    input->cursor_index = selected_props.rendered_count;
  }
  if(input->selected) {
    char selected_buf[strlen(input->buf)];
    strncpy(selected_buf, input->buf, input->cursor_index);
    selected_buf[input->cursor_index] = '\0';
    LfTextProps selected_props =  lf_text_render((vec2s){ state.pos_ptr.x + props.padding, lf_get_mouse_y() + props.padding }, selected_buf, font, LF_NO_COLOR, wrap_point, (vec2s){ -1, -1 }, true, false, -1, -1);
    vec2s cursor_pos = { (strlen(input->buf) > 0) ? selected_props.end_x : state.pos_ptr.x + props.padding, state.pos_ptr.y + props.padding + (selected_props.height - get_max_char_height_font(font)) }; 
    if(input->selection_start == -1 || input->selection_end == -1) {
      lf_rect_render(cursor_pos, (vec2s){ 1, get_max_char_height_font(font) }, props.text_color, LF_NO_COLOR, 0.0f, 0.0f);
    } 
    else {
      lf_text_render((vec2s){ state.pos_ptr.x + props.padding, state.pos_ptr.y + props.padding }, input->buf, font, (LfColor){ 255, 255, 255, 80 }, wrap_point, (vec2s){ -1, -1 }, false, true, input->selection_start, input->selection_end);
    }
  }
  lf_text_render((vec2s){ state.pos_ptr.x + props.padding, state.pos_ptr.y + props.padding }, (!strlen(input->buf) && !input->selected) ? input->placeholder : input->buf, font, !strlen(input->buf) ? lf_color_brightness(props.text_color, 0.75f) : props.text_color, wrap_point, (vec2s){ -1, -1 }, false, false, -1, -1); 
  state.pos_ptr.x += input->width + props.margin_right + props.padding * 2.0f;
  state.pos_ptr.y -= props.margin_top;
}

LfClickableItemState button_element_loc(void* text, const char* file, int32_t line, bool wide) {
  // Retrieving the property data of the button
  LfUIElementProps props = get_props_for(state.theme.button_props);
  float padding = props.padding;
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom = props.margin_bottom; 

  // Advancing the position pointer by the margins
  state.pos_ptr.x += margin_left;
  state.pos_ptr.y += margin_top;
  LfFont font = get_current_font();

  LfTextProps text_props;
  if(wide) 
    text_props = text_render_simple_wide(state.pos_ptr, (const wchar_t*)text, font, LF_NO_COLOR, true);
  else 
    text_props = text_render_simple(state.pos_ptr, (const char*)text, font, LF_NO_COLOR, true);

  LfColor color = props.color;
  LfColor text_color = lf_hovered(state.pos_ptr, (vec2s){text_props.width, text_props.height}) && props.hover_text_color.a != 0.0f ? props.hover_text_color : props.text_color;

  // If the button does not fit onto the current div, advance to the next line
  next_line_on_overflow(
    (vec2s){text_props.width + padding * 2.0f + margin_right + margin_left, 
      text_props.height + padding * 2.0f + margin_bottom + margin_top}, state.div_props.border_width);


  // Rendering the button
  LfClickableItemState ret = button(file, line, state.pos_ptr, (vec2s){text_props.width + padding * 2, text_props.height + padding * 2}, 
                                    props, color, props.border_width, true, true);
  // Rendering the text of the button
  if(wide)
    text_render_simple_wide((vec2s){state.pos_ptr.x + padding, state.pos_ptr.y + padding}, (const wchar_t*)text, font, text_color, false);
  else 
    text_render_simple((vec2s){state.pos_ptr.x + padding, state.pos_ptr.y + padding}, (const char*)text, font, text_color, false);

  // Advancing the position pointer by the width of the button
  state.pos_ptr.x += text_props.width + margin_right + padding * 2.0f;
  state.pos_ptr.y -= margin_top;

  return ret;
}
LfClickableItemState button_fixed_element_loc(void* text, float width, float height, const char* file, int32_t line, bool wide) {
  // Retrieving the property data of the button
  LfUIElementProps props = get_props_for(state.theme.button_props);
  float padding = props.padding;
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom = props.margin_bottom;

  LfFont font = state.font_stack ? *state.font_stack : state.theme.font;
  LfTextProps text_props; 
  if(wide)
    text_props = text_render_simple_wide(state.pos_ptr, (const wchar_t*)text, font, LF_NO_COLOR, true);
  else 
    text_props = text_render_simple(state.pos_ptr, (const char*)text, font, LF_NO_COLOR, true);

  LfColor color = props.color;
  LfColor text_color = lf_hovered(state.pos_ptr, (vec2s){text_props.width, text_props.height}) && props.hover_text_color.a != 0.0f ? props.hover_text_color : props.text_color;

  // If the button does not fit onto the current div, advance to the next line
  int32_t render_width = ((width == -1) ? text_props.width : width);
  int32_t render_height = ((height == -1) ? text_props.height : height);

  next_line_on_overflow(
    (vec2s){render_width + padding * 2.0f + margin_right + margin_left,
      render_height + padding * 2.0f + margin_bottom + margin_top}, 
    state.div_props.border_width); 

  // Advancing the position pointer by the margins
  state.pos_ptr.x += margin_left;
  state.pos_ptr.y += margin_top; 

  // Rendering the button
  LfClickableItemState ret = button(file, line, state.pos_ptr, 
                                    (vec2s){render_width + padding * 2.0f, render_height + padding * 2.0f}, props, 
                                    color, props.border_width, false, true);

  // Rendering the text of the button

  lf_set_cull_end_x(state.pos_ptr.x + render_width + padding);
  if(wide) {
    text_render_simple_wide((vec2s)
      {state.pos_ptr.x + padding + ((width != -1) ? (width - text_props.width) / 2.0f : 0),
        state.pos_ptr.y + padding + ((height != -1) ? (height - text_props.height) / 2.0f : 0)}, (const wchar_t*)text, font, text_color, false);
  } else {
    text_render_simple((vec2s)
      {state.pos_ptr.x + padding + ((width != -1) ? (width - text_props.width) / 2.0f : 0),
        state.pos_ptr.y + padding + ((height != -1) ? (height - text_props.height) / 2.0f : 0)}, (const char*)text, font, text_color, false);
  }
  lf_unset_cull_end_x();

  // Advancing the position pointer by the width of the button
  state.pos_ptr.x += render_width + margin_right + padding * 2.0f;
  state.pos_ptr.y -= margin_top;
  return ret;

}
LfClickableItemState checkbox_element_loc(void* text, bool* val, LfColor tick_color, LfColor tex_color, const char* file, int32_t line, bool wide) {
  // Retrieving the property values of the checkbox
  LfFont font = get_current_font();
  LfUIElementProps props = get_props_for(state.theme.checkbox_props);
  float margin_left = props.margin_left;
  float margin_right = props.margin_right;
  float margin_top = props.margin_top;
  float margin_bottom = props.margin_bottom;

  float checkbox_size; 
  if(wide)
    checkbox_size = lf_text_dimension_wide((const wchar_t*)text).y;
  else 
    checkbox_size = lf_text_dimension((const char*)text).y;

  // Advance to next line if the object does not fit on the div
  next_line_on_overflow((vec2s){checkbox_size + margin_left + margin_right + props.padding * 2.0f, checkbox_size + margin_top + margin_bottom + props.padding * 2.0f}, 
                        state.div_props.border_width);

  state.pos_ptr.x += margin_left; 
  state.pos_ptr.y += margin_top;

  // Render the box
  LfColor checkbox_color = (*val) ? ((tick_color.a == 0) ? props.color : tick_color) : props.color;
  LfClickableItemState checkbox = button(file, line, state.pos_ptr, (vec2s){checkbox_size + props.padding * 2.0f, checkbox_size + props.padding * 2.0f}, 
                                         props, checkbox_color, props.border_width, false, false);


  if(wide)
    text_render_simple_wide((vec2s){state.pos_ptr.x + checkbox_size + props.padding * 2.0f + margin_right, state.pos_ptr.y + props.padding}, (const wchar_t*)text, font, props.text_color, false);
  else 
    text_render_simple((vec2s){state.pos_ptr.x + checkbox_size + props.padding * 2.0f + margin_right, state.pos_ptr.y + props.padding}, (const char*)text, font, props.text_color, false);

  // Change the value if the checkbox is clicked
  if(checkbox == LF_CLICKED) {
    *val = !*val;
  }
  if(*val) {
    // Render the image
    lf_image_render(
      (vec2s){state.pos_ptr.x + props.padding,
        state.pos_ptr.y + props.padding}, 
      tex_color, 
      (LfTexture){
        .id = state.tex_tick.id, 
        .width = (uint32_t)(checkbox_size), 
        .height = (uint32_t)(checkbox_size)},
      (LfColor){0.0f, 0.0f, 0.0f, 0.0f}, 0, props.corner_radius);
  }
  state.pos_ptr.x += checkbox_size + props.padding * 2.0f + margin_right + 
    ((wide) ? lf_text_dimension_wide((const wchar_t*)text).x : lf_text_dimension((const char*)text).x) + margin_right;
  state.pos_ptr.y -= margin_top;

  return checkbox;

}
void dropdown_menu_item_loc(void** items, void* placeholder, uint32_t item_count, float width, float height, int32_t* selected_index, bool* opened, const char* file, int32_t line, bool wide) {
  LfUIElementProps props = get_props_for(state.theme.button_props);
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom = props.margin_bottom;
  float padding = props.padding;
  LfFont font = get_current_font();

  int32_t placeholder_strlen = (wide) ? wcslen((const wchar_t*)placeholder) : strlen((const char*)placeholder);
  void* button_text = (void*)((*selected_index != -1) ? items[*selected_index] : (placeholder_strlen != 0) ? placeholder : "Select");

  LfTextProps text_props;  
  if(wide)
    text_props = text_render_simple_wide((vec2s){state.pos_ptr.x + padding, state.pos_ptr.y + padding}, (const wchar_t*)button_text, font, props.text_color, true);
  else
    text_props = text_render_simple((vec2s){state.pos_ptr.x + padding, state.pos_ptr.y + padding}, (const char*)button_text, font, props.text_color, true);

  float item_height = get_max_char_height_font(font) + ((*opened) ? height + padding * 4.0f + margin_top : padding * 2.0f); 
  next_line_on_overflow(
    (vec2s){width + padding * 2.0f + margin_right,
      item_height + margin_top + margin_bottom}, 0.0f);

  state.pos_ptr.x += margin_left;
  state.pos_ptr.y += margin_top;

  vec2s button_pos = state.pos_ptr;
  LfClickableItemState dropdown_button = button(file, line, state.pos_ptr, (vec2s){(float)width + padding * 2.0f, (float)text_props.height + padding * 2.0f},  props, props.color, props.border_width, false, true);

  if(wide)
    text_render_simple_wide((vec2s){state.pos_ptr.x + padding, state.pos_ptr.y + padding}, (const wchar_t*)button_text, font, props.text_color, false);
  else 
    text_render_simple((vec2s){state.pos_ptr.x + padding, state.pos_ptr.y + padding}, (const char*)button_text, font, props.text_color, false);

  // Render dropdown arrow
  {
    vec2s image_size = (vec2s){20, 10};
    lf_image_render((vec2s){state.pos_ptr.x + width + padding - image_size.x, state.pos_ptr.y + ((text_props.height + padding * 2) - image_size.y) / 2.0f}, props.text_color,
                    (LfTexture){.id = state.tex_arrow_down.id, .width = (uint32_t)image_size.x, .height = (uint32_t)image_size.y}, LF_NO_COLOR, 0.0f, 0.0f);
  }

  if(dropdown_button == LF_CLICKED) {
    *opened = !*opened;
  }

  if(*opened) {
    if((lf_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT) && dropdown_button != LF_CLICKED) || 
    (!lf_input_grabbed() && lf_key_went_down(GLFW_KEY_ESCAPE))) {
      *opened = false;
    }

    LfUIElementProps div_props = lf_get_theme().div_props;
    div_props.corner_radius = props.corner_radius;
    div_props.border_color = props.border_color;
    div_props.border_width = props.border_width;
    div_props.color = props.color;
    lf_push_style_props(div_props);
    lf_div_begin(((vec2s){state.pos_ptr.x, state.pos_ptr.y + text_props.height + padding * 2.0f}),
                 ((vec2s){width + props.padding * 2.0f, height + props.padding * 2.0f}), false);
    lf_pop_style_props();

    for(uint32_t i = 0; i < item_count; i++) {
      LfUIElementProps text_props = lf_get_theme().text_props;
      text_props.text_color = props.text_color;
      bool hovered = lf_hovered((vec2s){state.pos_ptr.x + text_props.margin_left, state.pos_ptr.y + text_props.margin_top},
                                (vec2s){width + props.padding * 2.0f, lf_get_theme().font.font_size});
      if(hovered) {
        lf_rect_render(((vec2s){state.pos_ptr.x, state.pos_ptr.y}), 
                       (vec2s){width + props.padding * 2.0f, lf_get_theme().font.font_size + props.margin_top}, lf_color_brightness(div_props.color, 1.2f), 
                       LF_NO_COLOR, 0.0f, 0.0f);
      }

      if(hovered && lf_mouse_button_is_released(GLFW_MOUSE_BUTTON_LEFT)) {
        *selected_index = i;
      }
      lf_push_style_props(text_props);
      lf_text(items[i]);
      lf_pop_style_props();
      lf_next_line();
    }
    lf_div_end();
  }

  state.pos_ptr.x += width + padding * 2.0f + margin_right;
  state.pos_ptr.y -= margin_top;

  lf_push_style_props(props);

}

int32_t menu_item_list_item_loc(void** items, uint32_t item_count, int32_t selected_index, LfMenuItemCallback per_cb, bool vertical, const char* file, int32_t line, bool wide) {
  LfUIElementProps props = get_props_for(state.theme.button_props);
  float padding = props.padding;
  float margin_left = props.margin_left, margin_right = props.margin_right,
  margin_top = props.margin_top, margin_bottom = props.margin_bottom; 
  LfColor color = props.color;
  LfFont font = get_current_font();

  LfTextProps text_props[item_count];
  float width = 0;
  for(uint32_t i  = 0; i < item_count; i++) {
    if(wide)
      text_props[i] = text_render_simple_wide((vec2s){state.pos_ptr.x, state.pos_ptr.y + margin_top}, (const wchar_t*)items[i], font, props.text_color, true);
    else 
      text_props[i] = text_render_simple((vec2s){state.pos_ptr.x, state.pos_ptr.y + margin_top}, (const char*)items[i], font, props.text_color, true);
    width += text_props[i].width + padding * 2.0f;
  }
  next_line_on_overflow(
    (vec2s){width + padding * 2.0f + margin_right + margin_left, 
      font.font_size + padding * 2.0f + margin_bottom + margin_top}, 
    state.div_props.border_width);


  state.pos_ptr.y += margin_top; 
  state.pos_ptr.x += margin_left;

  uint32_t element_width = 0;
  uint32_t clicked_item = -1;
  for(uint32_t i = 0; i < item_count; i++) {
    LfUIElementProps props = state.theme.button_props;
    props.margin_left = 0;
    props.margin_right = 0;
    LfUIElementProps button_props = state.theme.button_props;
    lf_push_style_props(props);
    if(i == selected_index) {
      props.color = lf_color_brightness(props.color, 1.2); 
    }
    lf_push_style_props(props);
    if(wide) {
      if(_lf_button_wide_loc((const wchar_t*)items[i], file, line) == LF_CLICKED) {
        clicked_item = i;  
      } 
    } else {
      if(_lf_button_loc((const char*)items[i], file, line) == LF_CLICKED) {
        clicked_item = i;  
      } 
    }
    lf_pop_style_props();
    per_cb(&i);
  } 
  next_line_on_overflow((vec2s){element_width + margin_right, font.font_size + margin_top + margin_bottom}, state.div_props.border_width);


  state.pos_ptr.y -= margin_top;
  return clicked_item;
}
