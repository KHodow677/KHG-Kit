#include "khg2d/renderer2d.h"
#include "khg2d/utils.h"
#include "khgenv/error_report.h"
#include "khgenv/game_scripting.h"
#include "khgenv/input.h"
#include "khgenv/other.h"
#include "khgmath/vec2.h"
#include "GLFW/glfw3.h"
#include <time.h>

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
  glfwInit();
  glfwWindowHint(GLFW_SAMPLES, 4);
	int w = 500;
	int h = 500;
	wind = glfwCreateWindow(w, h, "Window", NULL, NULL);
	glfwMakeContextCurrent(wind);
	glfwSwapInterval(1);
	glfwSetKeyCallback(wind, key_callback);
	glfwSetMouseButtonCallback(wind, mouse_callback);
	glfwSetWindowFocusCallback(wind, window_focus_callback);
	glfwSetWindowSizeCallback(wind, window_size_callback);
	glfwSetCursorPosCallback(wind, cursor_position_callback);
	glfwSetCharCallback(wind, character_callback);
  gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress));
  enable_report_GL_errors();
  init();
  if (!init_game()) {
    return 0;
  }
  struct timespec start, stop;
  float delta_time, augmented_delta_time;
  clock_gettime(CLOCK_REALTIME, &stop);
  while (!glfwWindowShouldClose(wind)) {
    clock_gettime(CLOCK_REALTIME, &start);
    delta_time = (start.tv_sec - stop.tv_sec) + (start.tv_nsec - stop.tv_nsec) / 1000000000.0;
    clock_gettime(CLOCK_REALTIME, &stop);
    augmented_delta_time = delta_time;
    if (augmented_delta_time > 1.0f / 10.0f) {
      augmented_delta_time = 1.0f / 10.0f;
    }
    if (!game_logic(augmented_delta_time)) {
      close_game();
			return 0;
		}
		if (is_focused() && current_full_screen != full_screen) {
			static int lastW;
			static int lastH;
			static int lastPosX = 0;
			static int lastPosY = 0;
      lastW = w;
      lastH = h;
			if (full_screen) {
				lastW = w;
				lastH = h;
				glfwGetWindowPos(wind, &lastPosX, &lastPosY);
				GLFWmonitor *monitor = get_current_monitor(wind);
				const GLFWvidmode *mode = glfwGetVideoMode(monitor);
				glfwSetWindowMonitor(wind, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
				current_full_screen = 1;
			}
			else {
				glfwSetWindowMonitor(wind, NULL, lastPosX, lastPosY, lastW, lastH, 0);
				current_full_screen = 0;
			}
		}
    mouse_moved_flag = 0;
    update_all_buttons(delta_time);
    reset_typed_input();
		glfwSwapBuffers(wind);
		glfwPollEvents();
  }
  close_game();
  return 0;
}
