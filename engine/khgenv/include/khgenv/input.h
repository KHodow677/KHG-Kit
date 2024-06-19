#pragma once

#include "khgmath/vec2.h"
#include "khgutils/bool.h"
#include "GLFW/glfw3.h"

typedef struct {
  char pressed;
  char held;
  char released;
  char newState;
  char typed;
  float typedTime;
  enum {
    KeyA = 0,
    KeyB, KeyC, KeyD, KeyE, KeyF, KeyG, KeyH, KeyI, KeyJ, 
    KeyK, KeyL, KeyM, KeyN, KeyO, KeyP, KeyQ, KeyR, KeyS, 
    KeyT, KeyU, KeyV, KeyW, KeyX, KeyY, KeyZ,
    KeyNR0, KeyNR1, KeyNR2, KeyNR3, KeyNR4, KeyNR5, KeyNR6, KeyNR7, KeyNR8, KeyNR9,
    KeySpace,
    KeyEnter,
    KeyEscape,
    KeyUp,
    KeyDown,
    KeyLeft,
    KeyRight,
    KeyLeftCtrl,
    KeyTab,
    BUTTONS_COUNT,
  };
} button;

void merge(button *b, const button *bOther);
void resetButtonToZero(button *b);

typedef struct {
  enum {
    ConA = GLFW_GAMEPAD_BUTTON_A,           
    ConB = GLFW_GAMEPAD_BUTTON_B,           
    ConX = GLFW_GAMEPAD_BUTTON_X,           
    ConY = GLFW_GAMEPAD_BUTTON_Y,           
    ConLBumper = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER, 
    ConRBumper = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
    ConBack = GLFW_GAMEPAD_BUTTON_BACK,
    ConStart = GLFW_GAMEPAD_BUTTON_START,       
    ConGuide = GLFW_GAMEPAD_BUTTON_GUIDE,      
    ConLThumb = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,  
    ConRthumb = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB, 
    ConUp = GLFW_GAMEPAD_BUTTON_DPAD_UP,   
    ConRight = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,  
    ConDown = GLFW_GAMEPAD_BUTTON_DPAD_DOWN, 
    ConLeft = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,  
  };
  button ConButtons[GLFW_GAMEPAD_BUTTON_LAST + 1];
  float ConLT;
  float ConRT;
  vec2 ConLStick, ConRStick;
} controllerButtons;

void setAllToZero(controllerButtons *c);

int isButtonHeld(int key);
int isButtonPressedOn(int key);
int isButtonReleased(int key);
int isButtonTyped(int key);
int isLMousePressed();
int isRMousePressed();
int isLMouseReleased();
int isRMouseReleased();
int isLMouseHeld();
int isRMouseHeld();
controllerButtons getControllerButtons();
char **getTypedInput();
void setButtonState(int button, int newState);
void setLeftMouseState(int newState);
void setRightMouseState(int newState);
void processEventButton(button *b, bool newState);
void updateButton(button *b, float deltaTime);
void updateAllButtons(float deltaTime);
void resetInputsToZero();
void addToTypedInput(char c);
void resetTypedInput();

