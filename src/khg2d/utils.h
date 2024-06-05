#pragma once

#include <X11/X.h>
#ifdef _WIN32
#define KHG2D_SIMD 1
#else
#define KHG2D_SIMD 0
#endif

#include "glad/glad.h"
#include "camera.h"
#include "shader.h"
#include "texture.h"
#include "../math/vec2.h"

#define KHG2D_USE_OPENGL_130 false

#define KHG2D_DEFAULT_TEXTURE_LOAD_MODE_PIXELATED false
#define KHG2D_DEFAULT_TEXTURE_LOAD_MODE_USE_MIPMAPS true

#define KHG2D_OPNEGL_SHADER_VERSION "#version 330"
#define KHG2D_OPNEGL_SHADER_PRECISION "precision highp float;"

#define KHG2D_DefaulttextureCoords (glm::vec4{ 0, 1, 1, 0 })

#define ColorsRed vec4 { 1, 0, 0, 1 }
#define ColorsGreen vec4 { 0, 1, 0, 1 }
#define ColorsBlue vec4 { 0, 0, 1, 1 }
#define ColorsBlack vec4 { 0, 0, 0, 1 }
#define ColorsWhite vec4 { 1, 1, 1, 1 }
#define ColorsYellow vec4 { 1, 1, 0, 1 }
#define ColorsMagenta vec4 { 1, 0, 1, 1 }
#define ColorsTurqoise vec4 { 0, 1, 1, 1 }
#define ColorsOrange vec4 { 1, (float)0x7F / 255.0f, 0, 1 }
#define ColorsPurple vec4 { 101.0f / 255.0f, 29.0f / 255.0f, 173.0f / 255.0f, 1 }
#define ColorsGray vec4 { (float)0x7F / 255.0f, (float)0x7F / 255.0f, (float)0x7F / 255.0f, 1 }
#define ColorsTransparent vec4 { 0, 0, 0, 0 }

bool hasInitialized = 0;
void *userDefinedData = 0;
static shader defaultShader = { 0 };
static camera defaultCamera = { 0 };
static texture white1pxSquareTexture = { 0 };

static const char *defaultVertexShader =
  KHG2D_OPNEGL_SHADER_VERSION "\n"
  KHG2D_OPNEGL_SHADER_PRECISION "\n"
  "in vec2 quad_positions;\n"
  "in vec4 quad_colors;\n"
  "in vec2 texturePositions;\n"
  "out vec4 v_color;\n"
  "out vec2 v_texture;\n"
  "out vec2 v_positions;\n"
  "void main()\n"
  "{\n"
  "	gl_Position = vec4(quad_positions, 0, 1);\n"
  "	v_color = quad_colors;\n"
  "	v_texture = texturePositions;\n"
  "	v_positions = gl_Position.xy;\n"
  "}\n";

static const char *defaultFragmentShader =
  KHG2D_OPNEGL_SHADER_VERSION "\n"
  KHG2D_OPNEGL_SHADER_PRECISION "\n"
  "out vec4 color;\n"
  "in vec4 v_color;\n"
  "in vec2 v_texture;\n"
  "uniform sampler2D u_sampler;\n"
  "void main()\n"
  "{\n"
  "    color = v_color * texture2D(u_sampler, v_texture);\n"
  "}\n";

static const char *defaultVertexPostProcessShader =
  KHG2D_OPNEGL_SHADER_VERSION "\n"
  KHG2D_OPNEGL_SHADER_PRECISION "\n"
  "in vec2 quad_positions;\n"
  "out vec2 v_positions;\n"
  "out vec2 v_texture;\n"
  "out vec4 v_color;\n"
  "void main()\n"
  "{\n"
  "	gl_Position = vec4(quad_positions, 0, 1);\n"
  "	v_positions = gl_Position.xy;\n"
  "	v_color = vec4(1,1,1,1);\n"
  "	v_texture = (gl_Position.xy + vec2(1))/2.f;\n"
  "}\n";

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
bool setVsync(bool b);
void enableKHGNecessaryFeatures(void);
vec2 rotateAroundPoint(vec2 vec, vec2 point, const float degrees);
vec2 scaleAroundPoint(vec2 vec, vec2 point, float scale);
shader createShader(const char *vertex, const char *fragment);
shader createShaderFromFile(const char *filePath);
shader createShaderDefaultVertex(const char *fragment);
shader createPostProcessShaderFromFile(const char *filePath);
shader createPostProcessShader(const char *fragment);
