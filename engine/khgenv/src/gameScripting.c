#include "khgenv/gameScripting.h"
#include "khgenv/input.h"
#include "GLFW/glfw3.h"
#include <stdio.h>

bool currentFullScreen = 0;
bool fullScreen = 0;
int windowFocus = 1;
int mouseMovedFlag = 0;
GLFWwindow *wind = 0;

void setRelMousePosition(int x, int y) {
  glfwSetCursorPos(wind, x, y);
}

bool isFullScreen() {
  return fullScreen;
}

void setFullscreen(bool f) {
  fullScreen = f;
}

vec2 getWindowSize() {
  int x = 0, y = 0;
  glfwGetWindowSize(wind, &x, &y);
  return (vec2){ x, y };
}

int getWindowSizeX() {
  return getWindowSize().x;
}

int getWindowSizeY() {
  return getWindowSize().y;
}

vec2 getFramebufferSize() {
  int x = 0, y = 0;
  glfwGetFramebufferSize(wind, &x, &y);
  return (vec2){ x, y };
}

int getFrameBufferSizeX() {
  return getFramebufferSize().x;
}

int getFrameBufferSizeY() {
  return getFramebufferSize().y;
}

vec2 getRelMousePosition() {
  double x = 0, y = 0;
  glfwGetCursorPos(wind, &x, &y);
  return (vec2){ x, y };
}

void showMouse(bool show) {
  if(show) {
    glfwSetInputMode(wind, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
  else {
    glfwSetInputMode(wind, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  }
}

bool isFocused() {
  return windowFocus;
}

bool mouseMoved() {
  return mouseMovedFlag;
}

bool writeEntireFile(const char *name, void *buffer, size_t size) {
  FILE *f = fopen(name, "wb");
  if (f == NULL) {
    return 0;
  }
  size_t written = fwrite(buffer, 1, size, f);
  fclose(f);
  return written == size;
}


bool readEntireFile(const char *name, void *buffer, size_t size) {
  FILE *f = fopen(name, "rb");
  if (f == NULL) {
      return 0;
  }
  size_t read = fread(buffer, 1, size, f);
  fclose(f);
  return read == size;
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
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

void mouseCallback(GLFWwindow *window, int key, int action, int mods) {
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

void windowFocusCallback(GLFWwindow *window, int focused) {
  if (focused) {
    windowFocus = 1;
  }
  else {
    windowFocus = 0;
    reset_inputs_to_zero();
  }
}

void windowSizeCallback(GLFWwindow *window, int x, int y) {
  reset_inputs_to_zero();
}

void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos) {
	mouseMovedFlag = 1;
}

void characterCallback(GLFWwindow *window, unsigned int codepoint) {
	if (codepoint < 127) {
		add_to_typed_input(codepoint);
	}
}

