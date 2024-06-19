#include "khg2d/renderer2d.h"
#include "khg2d/utils.h"
#include "khgenv/errorReport.h"
#include "khgenv/gameScripting.h"
#include "khgenv/input.h"
#include "khgenv/other.h"
#include "khgmath/vec2.h"
#include "GLFW/glfw3.h"
#include <time.h>

struct gameData {
  vec2 rectPos;
} gData;
renderer2d renderer;

bool initGame() {
  init();
  createRenderer2d(&renderer, 0, 1000);
  readEntireFile("./res/gData.data", &gData, sizeof(gData));
  return true;
}

bool gameLogic(float deltaTime) {
  int w = 0; int h = 0;
  w = getFrameBufferSizeX();
  h = getFrameBufferSizeY();
  glViewport(0, 0, w, h);
  glClear(GL_COLOR_BUFFER_BIT);
  updateWindowMetrics(&renderer, w, h);
  if (isButtonHeld(KeyLeft)) {
    gData.rectPos.x -= deltaTime * 100;
  }
  if (isButtonHeld(KeyRight)) {
    gData.rectPos.x += deltaTime * 100;
  }
  if (isButtonHeld(KeyUp)) {
    gData.rectPos.y -= deltaTime * 100;
  }
  if (isButtonHeld(KeyDown)) {
    gData.rectPos.y += deltaTime * 100;
  }
  if (isButtonPressedOn(KeyEscape)) {
    return false;
  }
  vec2 min = { 0.0f, 0.0f };
  vec2 max = { w - 100.0f, h - 100.0f };
  gData.rectPos = vec2Clamp(&gData.rectPos, &min, &max);
  vec4 blueColor[] = { colorBlue, colorBlue, colorBlue, colorBlue };
  renderRectangle(&renderer, (vec4){ gData.rectPos.x, gData.rectPos.y, 100.0f, 100.0f }, blueColor, (vec2){ 0.0f, 0.0f }, 0);
  flush(&renderer, true);
  return true;
}

void closeGame() {
  writeEntireFile("./res/gData.data", &gData, sizeof(gData));
  cleanupRenderer2d(&renderer);
}

int main(int argc, char **argv) {
  glfwInit();
  glfwWindowHint(GLFW_SAMPLES, 4);
	int w = 500;
	int h = 500;
	wind = glfwCreateWindow(w, h, "Window", NULL, NULL);
	glfwMakeContextCurrent(wind);
	glfwSwapInterval(1);
	glfwSetKeyCallback(wind, keyCallback);
	glfwSetMouseButtonCallback(wind, mouseCallback);
	glfwSetWindowFocusCallback(wind, windowFocusCallback);
	glfwSetWindowSizeCallback(wind, windowSizeCallback);
	glfwSetCursorPosCallback(wind, cursorPositionCallback);
	glfwSetCharCallback(wind, characterCallback);
  gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress));
  enableReportGLErrors();
  init();
  if (!initGame()) {
    return 0;
  }
  struct timespec start, stop;
  float deltaTime, augmentedDeltaTime;
  clock_gettime(CLOCK_REALTIME, &stop);
  while (!glfwWindowShouldClose(wind)) {
    clock_gettime(CLOCK_REALTIME, &start);
    deltaTime = (start.tv_sec - stop.tv_sec) + (start.tv_nsec - stop.tv_nsec) / 1000000000.0;
    clock_gettime(CLOCK_REALTIME, &stop);
    augmentedDeltaTime = deltaTime;
    if (augmentedDeltaTime > 1.0f / 10.0f) {
      augmentedDeltaTime = 1.0f / 10.0f;
    }
    if (!gameLogic(augmentedDeltaTime)) {
      closeGame();
			return 0;
		}
		if (isFocused() && currentFullScreen != fullScreen) {
			static int lastW;
			static int lastH;
			static int lastPosX = 0;
			static int lastPosY = 0;
      lastW = w;
      lastH = h;
			if (fullScreen) {
				lastW = w;
				lastH = h;
				glfwGetWindowPos(wind, &lastPosX, &lastPosY);
				GLFWmonitor *monitor = getCurrentMonitor(wind);
				const GLFWvidmode *mode = glfwGetVideoMode(monitor);
				glfwSetWindowMonitor(wind, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
				currentFullScreen = 1;
			}
			else {
				glfwSetWindowMonitor(wind, NULL, lastPosX, lastPosY, lastW, lastH, 0);
				currentFullScreen = 0;
			}
		}
    mouseMovedFlag = 0;
    updateAllButtons(deltaTime);
    resetTypedInput();
		glfwSwapBuffers(wind);
		glfwPollEvents();
  }
  closeGame();
  return 0;
}
