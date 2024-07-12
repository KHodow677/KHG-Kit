#define GLFW_INCLUDE_NONE

#include "khg_env/game_scripting.h"
#include "khg_2d/utils.h"
#include "khg_env/error_report.h"
#include "khg_env/input.h"
#include "GLFW/glfw3.h"
#include "khg_env/other.h"
#include <stdio.h>

bool current_full_screen = 0;
bool full_screen = 0;
int window_focus = 1;
int mouse_moved_flag = 0;
GLFWwindow *wind = 0;

static uint64_t get_time_count(void) {
  uint64_t value = 0;
  #if defined(_WIN32)
    QueryPerformanceCounter((unsigned long long int *) &value);
  #elif defined(__linux__)
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    value = (uint64_t)now.tv_sec * (uint64_t) 1000000000 + (uint64_t)now.tv_nsec;
  #elif defined(__APPLE__)
    value = mach_absolute_time();
  #elif defined(EMSCRIPTEN)
    value = emscripten_get_now();
  #endif
  return value;
}

static double get_current_time(void) {
  return (double)(get_time_count() - base_time) / frequency * 1000;
}

static void init_timer(void) {
  srand(time(NULL));
  #if defined(_WIN32) || defined(_WIN64)
    QueryPerformanceFrequency((unsigned long long int *) &frequency);
  #elif defined(__linux__)
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) == 0) {
      frequency = 1000000000;
    }
  #elif defined(__APPLE__)
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    frequency = (timebase.denom * 1e9) / timebase.numer;
  #elif defined(EMSCRIPTEN)
    frequency = 1000;
  #endif
  base_time = get_time_count();
  start_time = get_current_time();
}

int run_game() {
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
  init_timer();
  float s_time = get_current_time();
  float delta_time, augmented_delta_time;
  while (!glfwWindowShouldClose(wind)) {
    float current_time = get_current_time();
    delta_time = current_time - s_time;
    s_time = current_time;
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

void set_rel_mouse_position(int x, int y) {
  glfwSetCursorPos(wind, x, y);
}

bool is_full_screen() {
  return full_screen;
}

void setFullscreen(bool f) {
  full_screen = f;
}

vec2 get_window_size() {
  int x = 0, y = 0;
  glfwGetWindowSize(wind, &x, &y);
  return (vec2){ x, y };
}

int get_window_size_x() {
  return get_window_size().x;
}

int get_window_size_y() {
  return get_window_size().y;
}

vec2 get_framebuffer_size() {
  int x = 0, y = 0;
  glfwGetFramebufferSize(wind, &x, &y);
  return (vec2){ x, y };
}

int get_framebuffer_size_x() {
  return get_framebuffer_size().x;
}

int get_framebuffer_size_y() {
  return get_framebuffer_size().y;
}

vec2 get_rel_mouse_position() {
  double x = 0, y = 0;
  glfwGetCursorPos(wind, &x, &y);
  return (vec2){ x, y };
}

void show_mouse(bool show) {
  if(show) {
    glfwSetInputMode(wind, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
  else {
    glfwSetInputMode(wind, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  }
}

bool is_focused() {
  return window_focus;
}

bool mouse_moved() {
  return mouse_moved_flag;
}

bool write_entire_file(const char *name, void *buffer, size_t size) {
  FILE *f = fopen(name, "wb");
  if (f == NULL) {
    return 0;
  }
  size_t written = fwrite(buffer, 1, size, f);
  fclose(f);
  return written == size;
}


bool read_entire_file(const char *name, void *buffer, size_t size) {
  FILE *f = fopen(name, "rb");
  if (f == NULL) {
      return 0;
  }
  size_t read = fread(buffer, 1, size, f);
  fclose(f);
  return read == size;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if ((action == GLFW_REPEAT || action == GLFW_PRESS) && key == GLFW_KEY_BACKSPACE) {
    add_to_typed_input(8);
	}
  bool state = 0;
  if(action == GLFW_PRESS) {
		state = 1;
	}
  else if(action == GLFW_RELEASE) {
		state = 0;
	}
  else {
		return;
	}
  if(key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
		int index = key - GLFW_KEY_A;
		set_button_state(key_A + index, state);
	}
  else if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
		int index = key - GLFW_KEY_0;
		set_button_state(key_0 + index, state);
	}
  else {
    if (key == GLFW_KEY_SPACE) {
      set_button_state(key_space, state);
    }
    else if (key == GLFW_KEY_ENTER) {
      set_button_state(key_enter, state);
    }
    else if (key == GLFW_KEY_ESCAPE) {
      set_button_state(key_escape, state);
    }
    else if (key == GLFW_KEY_UP) {
      set_button_state(key_up, state);
    }
    else if (key == GLFW_KEY_DOWN) {
      set_button_state(key_down, state);
    }
    else if (key == GLFW_KEY_LEFT) {
      set_button_state(key_left, state);
    }
    else if (key == GLFW_KEY_RIGHT) {
      set_button_state(key_right, state);
    }
    else if (key == GLFW_KEY_LEFT_CONTROL) {
      set_button_state(key_left_ctrl, state);
    }
    else if (key == GLFW_KEY_TAB) {
      set_button_state(key_tab, state);
    }
  }
}

void mouse_callback(GLFWwindow *window, int key, int action, int mods) {
  bool state = 0;
  if (action == GLFW_PRESS) {
    state = 1;
  }
  else if (action == GLFW_RELEASE) {
    state = 0;
  }
  else {
    return;
  }
  if (key == GLFW_MOUSE_BUTTON_LEFT) {
    set_left_mouse_state(state);
  }
  else if (key == GLFW_MOUSE_BUTTON_RIGHT) {
    set_right_mouse_state(state);
  }
}

void window_focus_callback(GLFWwindow *window, int focused) {
  if (focused) {
    window_focus = 1;
  }
  else {
    window_focus = 0;
    reset_inputs_to_zero();
  }
}

void window_size_callback(GLFWwindow *window, int x, int y) {
  reset_inputs_to_zero();
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
	mouse_moved_flag = 1;
}

void character_callback(GLFWwindow *window, unsigned int codepoint) {
	if (codepoint < 127) {
		add_to_typed_input(codepoint);
	}
}

