#include "khg_2d/font.h"
#include "khg_2d/renderer_2d.h"
#include "khg_2d/texture.h"
#include "khg_2d/utils.h"
#include "khg_env/game_scripting.h"
#include "khg_env/input.h"
#include "khg_math/vec2.h"
#include "khg_ui/renderer_ui.h"

struct game_data {
} g_data;
renderer_2d r2d;
renderer_ui rui;
font f;
texture t;
texture terraria_texture;
texture logo_texture;

bool init_game() {
  init();
  create_renderer_2d(&r2d, 0, 1000);
  create_from_file(&f, "./res/assets/fonts/ANDYB.TTF");
  load_from_file(&t, "./res/assets/mc/beacon_button_default.png", true, KHG2D_DEFAULT_TEXTURE_LOAD_MODE_USE_MIPMAPS);
  load_from_file(&terraria_texture, "./res/assets/terraria.png", KHG2D_DEFAULT_TEXTURE_LOAD_MODE_PIXELATED, KHG2D_DEFAULT_TEXTURE_LOAD_MODE_USE_MIPMAPS);
  load_from_file(&logo_texture, "./res/assets/tick.png", true, KHG2D_DEFAULT_TEXTURE_LOAD_MODE_USE_MIPMAPS);
  if (!read_entire_file("./res/g_data.txt", &g_data, sizeof(g_data))) {
    g_data = (struct game_data){};
  }
  return true;
}

void render_1() {
  begin_ui(&rui, 6996);
  static float value = 0;
  static int value_int = 0;
  slider_float_ui(&rui, "Slider example", &value, -2, 5, color_white, t, (vec4){ 1.0f, 1.0f, 1.0f, 1.0f }, t, (vec4){ 1.0f, 1.0f, 1.0f, 1.0f });
  slider_int_ui(&rui, "Slider example int", &value_int, -2, 5, (vec4){ 1.0f, 1.0f, 1.0f, 1.0f }, t, (vec4){ 1.0f, 1.0f, 1.0f, 1.0f }, t, (vec4){ 1.0f, 1.0f, 1.0f, 1.0f });
  vec4 custom_transform = { 0.0f, 0.0f, 20.0f, 20.0f };
  bool clicked = 0;
  bool hovered = 0;
  if (custom_widget_ui(&rui, 23, &custom_transform, &hovered, &clicked)) {
    if (clicked) {
      vec4 colors[4] = { color_blue, color_blue, color_blue, color_blue };
      render_rectangle(&r2d, custom_transform, colors, (vec2){ 0.0f, 0.0f }, 0.0f);
    }
    else if (hovered) {
      vec4 colors[4] = { color_green, color_green, color_green, color_green };
      render_rectangle(&r2d, custom_transform, colors, (vec2){ 0.0f, 0.0f }, 0.0f);
    }
    else {
      vec4 colors[4] = { color_red, color_red, color_red, color_red };
      render_rectangle(&r2d, custom_transform, colors, (vec2){ 0.0f, 0.0f }, 0.0f);
    }
  }
  end_ui(&rui);
}

void render_2() {

}

bool game_logic(float delta_time) {
  int w = 0, h = 0;
  w = get_window_size_x();
  h = get_window_size_y();
  update_window_metrics(&r2d, w, h);
  clear_screen(&r2d, (vec4){ 0.2f, 0.2f, 0.3f, 1.0f });
  render_1();
  render_frame(&rui, &r2d, &f, get_rel_mouse_position(), is_left_mouse_pressed(), is_left_mouse_held(), is_left_mouse_released(), is_button_released(key_escape), get_typed_input(), delta_time);
  flush(&r2d, true);
  return true;
}

void close_game() {
  write_entire_file("./res/g_data.txt", &g_data, sizeof(g_data));
  cleanup_renderer_2d(&r2d);
}

int main(int argc, char **argv) {
  return run_game();
}
