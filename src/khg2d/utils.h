#pragma once

#include <X11/X.h>

#define KHG2D_SIMD 0

#include "glad/glad.h"
#include "camera.h"
#include "shader.h"
#include "texture.h"
#include "../math/vec2.h"
#include "../math/vec4.h"

#define KHG2D_USE_OPENGL_130 false

#define KHG2D_DEFAULT_TEXTURE_LOAD_MODE_PIXELATED false
#define KHG2D_DEFAULT_TEXTURE_LOAD_MODE_USE_MIPMAPS true

#define KHG2D_OPNEGL_SHADER_VERSION "#version 330"
#define KHG2D_OPNEGL_SHADER_PRECISION "precision highp float;"

static vec4 colorRed = { 1, 0, 0, 1 };
static vec4 colorGreen = { 0, 1, 0, 1 };
static vec4 colorBlue = { 0, 0, 1, 1 };
static vec4 colorBlack = { 0, 0, 0, 1 };
static vec4 colorWhite = { 1, 1, 1, 1 };
static vec4 colorYellow = { 1, 1, 0, 1 };
static vec4 colorMagenta = { 1, 0, 1, 1 };
static vec4 colorTurqoise = { 0, 1, 1, 1 };
static vec4 colorOrange = { 1, (float)0x7F / 255.0f, 0, 1 };
static vec4 colorPurple = { 101.0f / 255.0f, 29.0f / 255.0f, 173.0f / 255.0f, 1 };
static vec4 colorGray = { (float)0x7F / 255.0f, (float)0x7F / 255.0f, (float)0x7F / 255.0f, 1 };
static vec4 colorTransparent = { 0, 0, 0, 0 };

extern bool hasInitialized;
extern void *userDefinedData;
extern shader defaultShader;
extern camera defaultCamera;
extern texture white1pxSquareTexture;

static vec4 defaultTextureCoords = { 0, 1, 1, 0 };

extern char *defaultVertexShader;
extern char *defaultFragmentShader;
extern char *defaultVertexPostProcessShader;

void defaultErrorFunc(const char *msg, void *userDefinedData);
void setUserDefinedData(void *data);
typedef void (*errorFuncType)(const char *, void *);
errorFuncType setErrorFuncCallback(errorFuncType newFunc);
static errorFuncType errorFunc = defaultErrorFunc;
float positionToScreenCoordsX(const float position, float w);
float positionToScreenCoordsY(const float position, float h);
vec2 convertPoint(const camera *c, const vec2 *p, float windowW, float windowH);
GLuint loadShader(const char *source, GLenum shaderType);
void init(void);
void cleanup(void);
vec2 rotateAroundPoint(vec2 vec, vec2 point, const float degrees);
vec2 scaleAroundPoint(vec2 vec, vec2 point, float scale);
camera createCamera(void);
shader createShader(const char *vertex, const char *fragment);
shader createShaderFromFile(const char *filePath);
shader createShaderDefaultVertex(const char *fragment);
shader createPostProcessShaderFromFile(const char *filePath);
shader createPostProcessShader(const char *fragment);
char *loadFileContents(char const *path);
