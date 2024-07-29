#include "gfx/inputs.h"
#include "khg_utl/error_func.h"
#include <string.h>

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
  state.key_ev.keycode = key;
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
  state.scr_ev.xoffset = xoffset;
  state.scr_ev.yoffset = yoffset;
  LfDiv *selected_div = &state.selected_div;
  if(!selected_div->scrollable) return;
  if((state.grabbed_div.id != -1 && selected_div->id != state.grabbed_div.id)) {
    return;
  }
  if(yoffset < 0.0f) {
    if(selected_div->total_area.y > (selected_div->aabb.size.y + selected_div->aabb.pos.y)) { 
      if(state.theme.div_smooth_scroll) {
        *state.scroll_velocity_ptr -= state.theme.div_scroll_acceleration;
        state.div_velocity_accelerating = true;
      } 
      else {
        *state.scroll_ptr -= state.theme.div_scroll_amount_px;
      }
    } 
  } 
  else if (yoffset > 0.0f) {
    if(*state.scroll_ptr) {
      if(state.theme.div_smooth_scroll) {
        *state.scroll_velocity_ptr += state.theme.div_scroll_acceleration;
        state.div_velocity_accelerating = false;
      } 
      else {
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
  LfMouse *mouse = &state.input.mouse;
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
  state.ch_ev.charcode = charcode;
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

void props_stack_create(PropsStack *stack) {
  stack->data = (LfUIElementProps *)malloc(LF_STACK_INIT_CAP * sizeof(LfUIElementProps));
  if(!stack->data) {
    error_func("Failed to allocate memory for stack data structure", user_defined_data);
  }
  stack->count = 0;
  stack->cap = LF_STACK_INIT_CAP;
}

void props_stack_resize(PropsStack *stack, uint32_t newcap) {
  LfUIElementProps *newdata = (LfUIElementProps *)realloc(stack->data, newcap * sizeof(LfUIElementProps));
  if(!newdata) {
    error_func("Failed to reallocate memory for stack datastructure", user_defined_data);
  }
  stack->data = newdata;
  stack->cap = newcap;
}

void props_stack_push(PropsStack *stack, LfUIElementProps props) {
  if(stack->count == stack->cap) {
    props_stack_resize(stack, stack->cap * 2);
  }
  stack->data[stack->count++] = props;
}

LfUIElementProps props_stack_pop(PropsStack *stack) {
  LfUIElementProps val = stack->data[--stack->count];
  if(stack->count > 0 && stack->count == stack->cap / 4) {
    props_stack_resize(stack, stack->cap / 2);
  }
  return val;
}

LfUIElementProps props_stack_peak(PropsStack *stack) {
  return stack->data[stack->count - 1];
}

bool props_stack_empty(PropsStack *stack) {
  return stack->count == 0;
}

LfUIElementProps get_props_for(LfUIElementProps props) {
  return (!props_stack_empty(&state.props_stack)) ? props_stack_peak(&state.props_stack) : props; 
}
