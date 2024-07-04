#include "khg_2d/renderer_2d.h"
#include "khg_2d/utils.h"
#include "khg_env/game_scripting.h"
#include "khg_env/input.h"
#include "khg_math/vec2.h"

struct gameData {
  vec2 rectPos;
} g_data;
renderer_2d renderer;

bool init_game() {
  init();
  create_renderer_2d(&renderer, 0, 1000);
  read_entire_file("./res/g_data.txt", &g_data, sizeof(g_data));
  return true;
}

bool game_logic(float delta_time) {
  int w = 0; int h = 0;
  w = get_framebuffer_size_x();
  h = get_framebuffer_size_y();
  glViewport(0, 0, w, h);
  glClear(GL_COLOR_BUFFER_BIT);
  update_window_metrics(&renderer, w, h);
  if (is_button_held(key_left)) {
    g_data.rectPos.x -= delta_time * 100;
  }
  if (is_button_held(key_right)) {
    g_data.rectPos.x += delta_time * 100;
  }
  if (is_button_held(key_up)) {
    g_data.rectPos.y -= delta_time * 100;
  }
  if (is_button_held(key_down)) {
    g_data.rectPos.y += delta_time * 100;
  }
  if (is_button_pressed_on(key_escape)) {
    return false;
  }
  vec2 min = { 0.0f, 0.0f };
  vec2 max = { w - 100.0f, h - 100.0f };
  g_data.rectPos = vec2_clamp(&g_data.rectPos, &min, &max);
  vec4 blueColor[] = { color_blue, color_blue, color_blue, color_blue };
  render_rectangle(&renderer, (vec4){ g_data.rectPos.x, g_data.rectPos.y, 100.0f, 100.0f }, blueColor, (vec2){ 0.0f, 0.0f }, 0);
  flush(&renderer, true);
  return true;
}

void close_game() {
  write_entire_file("./res/g_data.txt", &g_data, sizeof(g_data));
  cleanup_renderer_2d(&renderer);
}

int main(int argc, char **argv) {
  return run_game();
}
