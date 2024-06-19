#include "khgenv/input.h"
#include "khgenv/gameScripting.h"
#include <string.h>

button keyboard[BUTTONS_COUNT];
button leftMouse;
button rightMouse;

controllerButtons cButtons;
char ** typedInput;

void merge(button *b, const button *bOther) {
  b->pressed |= bOther->pressed;
  b->released |= bOther->released;
  b->held |= bOther->held;
}

void resetButtonToZero(button *b) {
  b->pressed = 0;
  b->held = 0;
  b->released = 0;
}

void setAllToZero(controllerButtons *c) {
  button buttons[GLFW_GAMEPAD_BUTTON_LAST + 1];
  vec2 zeroVec2 = { 0.0f, 0.0f };
  *c->ConButtons = *buttons;
  c->ConLT = 0.0f;
  c->ConRT = 0.0f;
  c->ConLStick = zeroVec2;
  c->ConRStick = zeroVec2;
}

int isButtonHeld(int key) {
  if (key < KeyA || key >= BUTTONS_COUNT) {
    return 0;
  }
  return keyboard[key].held;
}

int isButtonPressedOn(int key) {
  if (key < KeyA || key >= BUTTONS_COUNT) {
    return 0;
  }
  return keyboard[key].pressed;
}

int isButtonReleased(int key) {
  if (key < KeyA || key >= BUTTONS_COUNT) {
    return 0;
  }
  return keyboard[key].released;
}

int isButtonTyped(int key) {
  if (key < KeyA || key >= BUTTONS_COUNT) {
    return 0;
  }
  return keyboard[key].typed;
}

int isLMousePressed() {
  return leftMouse.pressed;
}

int isRMousePressed() {
  return rightMouse.pressed;
}

int isLMouseReleased() {
  return leftMouse.released;
}

int isRMouseReleased() {
  return rightMouse.released;
}

int isLMouseHeld() {
  return leftMouse.held;
}

int isRMouseHeld() {
  return rightMouse.held;
}

controllerButtons getControllerButtons() {
  if (isFocused()) {
    return cButtons;
  }
  controllerButtons c;
  setAllToZero(&c);
  return c;
}

char **getTypedInput() {
  return typedInput;
}

void setButtonState(int button, int newState) {
  processEventButton(&keyboard[button], newState);
}

void setLeftMouseState(int newState) {
  processEventButton(&leftMouse, newState);
}
void setRightMouseState(int newState) {
  processEventButton(&rightMouse, newState);
}

void processEventButton(button *b, bool newState) {
  b->newState = newState;
}

void updateButton(button *b, float deltaTime) {
  if (b->newState == 1) {
    if (b->held) {
      b->pressed = false;
    }
    else {
      b->pressed = true;
    }
    b->held = true;
    b->released = false;
  }
  else if (b->newState == 0) {
    b->held = false;
    b->pressed = false;
    b->released = true;
  }
  else {
    b->pressed = false;
    b->released = false;
  }
  if (b->pressed) {
    b->typed = true;
    b->typedTime = 0.48f;
  }
  else if (b->held) {
    b->typedTime -= deltaTime;
    if (b->typedTime < 0.0f) {
      b->typedTime += 0.07f;
      b->typed = true;
    }
    else {
      b->typed = false;
    }
  }
  else {
    b->typedTime = 0;
    b->typed = false;
  }
  b->newState = -1;
}

void updateAllButtons(float deltaTime) {
	for (int i = 0; i < BUTTONS_COUNT; i++) {
		updateButton(&keyboard[i], deltaTime);
	}
	updateButton(&leftMouse, deltaTime);
	updateButton(&rightMouse, deltaTime);
	for(int i=0; i<=GLFW_JOYSTICK_LAST; i++) {
		if(glfwJoystickPresent(i) && glfwJoystickIsGamepad(i)) {
			GLFWgamepadstate state;
			if (glfwGetGamepadState(i, &state)) {
				for (int b = 0; b <= GLFW_GAMEPAD_BUTTON_LAST; b++) {
          if(state.buttons[b] == GLFW_PRESS) {
						processEventButton(&cButtons.ConButtons[b], 1);
					}
          else if (state.buttons[b] == GLFW_RELEASE) {
						processEventButton(&cButtons.ConButtons[b], 0);
					}
					updateButton(&cButtons.ConButtons[b], deltaTime);
				}
				cButtons.ConLT = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
				cButtons.ConRT = state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
				cButtons.ConLStick.x = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
				cButtons.ConLStick.y = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
				cButtons.ConRStick.x = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
				cButtons.ConRStick.y = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
				break;
			}
		}
	}
}

void resetInputsToZero() {
  resetTypedInput();
  for (int i = 0; i < BUTTONS_COUNT; i++) {
    resetButtonToZero(&keyboard[i]);
  }
  resetButtonToZero(&leftMouse);
  resetButtonToZero(&rightMouse);
  setAllToZero(&cButtons);
}

void addToTypedInput(char c) {
  strcat(*typedInput, &c);
}

void resetTypedInput() {
  memset(&typedInput, 0, sizeof(typedInput));
}
