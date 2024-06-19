#pragma once

#include "khgmath/vec2.h"
#include "khgutils/bool.h"
#include "GLFW/glfw3.h"

extern bool currentFullScreen;
extern bool fullScreen;
extern int windowFocus;
extern int mouseMovedFlag;
extern GLFWwindow *wind;

bool initGame();
bool gameLogic(float deltaTime);
void closeGame();
void setRelMousePosition(int x, int y);
bool isFullScreen();
bool setFullScreen(bool f);
vec2 getWindowSize();
int getWindowSizeX();
int getWindowSizeY();
vec2 getFramebufferSize();
int getFrameBufferSizeX();
int getFrameBufferSizeY();
vec2 getRelMousePosition();
void showMouse(bool show);
bool isFocused();
bool mouseMoved();
bool writeEntireFile(const char *name, void *buffer, size_t size);
bool readEntireFile(const char *name, void *buffer, size_t size);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouseCallback(GLFWwindow *window, int key, int action, int mods);
void windowFocusCallback(GLFWwindow *window, int focused);
void windowSizeCallback(GLFWwindow *window, int x, int y);
void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos);
void characterCallback(GLFWwindow *window, unsigned int codepoint);

