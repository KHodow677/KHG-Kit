#pragma once

#include "khgmath/vec2.h"
#include "khgmath/vec4.h"
#include "khgutils/cvector.h"
#include "camera.h"
#include "font.h"
#include "framebuffer.h"
#include "renderer2dBufferType.h"
#include "shader.h"
#include "texture.h"

typedef struct {
  GLuint defaultFbo;
  GLuint buffers[bufferSize];
  GLuint vao;
  cvector(vec2) spritePositions;
  cvector(vec4) spriteColors;
  cvector(vec2) texturePositions;
  cvector(texture) spriteTextures;
  shader currentShader;
  cvector(shader) shaderPushPop;
  camera currentCamera;
  cvector(camera) cameraPushPop;
  int windowW;
  int windowH;
  framebuffer postProcessFbo1;
  framebuffer postProcessFbo2;
  bool internalPostProcessFlip;
} renderer2d;

void createRenderer2d(renderer2d *r2d, GLuint fbo, size_t quadCount);
void cleanupRenderer2d(renderer2d *r2d);
void pushShader(renderer2d *r2d, shader s);
void popShader(renderer2d *r2d);
void pushCamera(renderer2d *r2d, camera c);
void popCamera(renderer2d *r2d);
vec4 getViewRect(renderer2d *r2d);
void updateWindowMetrics(renderer2d *r2d, int w, int h);
vec4 pixToScreen(renderer2d *r2d, const vec4 *transform);
void clearDrawData(renderer2d *r2d);
vec2 getTextSize(renderer2d *r2d, const char *text, const font font, const float size, const float spacing, const float line_space);
void renderText(renderer2d *r2d, vec2 position, const char *text, const font f, const vec4 color, const float size, const float spacing, const float line_space, bool showInCenter, const vec4 shadowColor, const vec4 lightColor);
float determineTextRescaleFitSmaller(renderer2d *r2d, const char **str, font *f, vec4 transform, float maxSize);
float determineTextRescaleFitBigger(renderer2d *r2d, const char **str, font *f, vec4 transform, float minSize);
float determineTextRescaleFit(renderer2d *r2d, const char **str, font *f, vec4 transform);
int wrap(renderer2d *r2d, const char **in, font *f, float baseSize, float maxDimension, char **outRez);
void renderTextWrapped(renderer2d *r2d, const char *text, font f, vec4 textPos, vec4 color, float baseSize, float spacing, float lineSpacing, bool showInCenter, vec4 shadowColor, vec4 lightColor);
vec2 getTextSizeWrapped(renderer2d *r2d, const char *text, font f, float maxTextLength, float baseSize, float spacing, float lineSpacing);
void renderRectangleTexture(renderer2d *r2d, const vec4 transforms, const texture texture, const vec4 colors[4], const vec2 origin, const float rotationDegrees, const vec4 textureCoords);
void renderRectangleTextureAbsRotation(renderer2d *r2d, const vec4 transforms, const texture texture, const vec4 colors[4], const vec2 origin, const float rotationDegrees, const vec4 textureCoords);
void renderRectangle(renderer2d *r2d, const vec4 transforms, const vec4 colors[4], const vec2 origin, const float rotationDegrees);
void renderRectangleAbsRotation(renderer2d *r2d, const vec4 transforms, const vec4 colors[4], const vec2 origin, const float rotationDegrees);
void renderLineAngle(renderer2d *r2d, vec2 position, const float angleDegrees, const float length, const vec4 color, const float width);
void renderLineStartEnd(renderer2d *r2d, const vec2 start, const vec2 end, const vec4 color, const float width);
void renderRectangleOutline(renderer2d *r2d, const vec4 position, const vec4 color, const float width, const vec2 origin, const float rotationDegrees);
void renderCircleOutline(renderer2d *r2d, const vec2 position, const vec4 color, const float size, const float width, const unsigned int segments);
void renderNinePatch(renderer2d *r2d, const vec4 position, const vec4 color, const vec2 origin, const float rotationDegrees, const texture texture, const vec4 textureCoords, const vec4 inner_texture_coords);
void clearScreen(renderer2d *r2d, const vec4 color);
void setShader(renderer2d *r2d, const shader s);
void setCamera(renderer2d *r2d, const camera c);
void resetShaderAndCamera(renderer2d *r2d);
void renderPostProcess(renderer2d *r2d, shader shader, texture input, framebuffer result);
void flush(renderer2d *r2d, bool shouldClear);
void flushFbo(renderer2d *r2d, framebuffer frameBuffer, bool shouldClear);
void renderFrameBufferToEntireScreen(renderer2d *r2d, framebuffer fbo, framebuffer screen);
void renderTextureToEntireScreen(renderer2d *r2d, texture t, framebuffer screen);
void flushPostProcess(renderer2d *r2d, const cvector(shader) *postProcess, framebuffer frameBuffer, bool clearDrawData);
void postProcessOverTexture(renderer2d *r2d, const cvector(shader) *postProcess, texture in, framebuffer framebuffer);
void enableGLNecessaryFeatures(void);