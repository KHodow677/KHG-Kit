#include "camera.h"
#include "font.h"
#include "shader.h"
#include "texture.h"
#include "renderer2dBufferType.h"
#include "../math/vec2.h"
#include "../math/vec4.h"

typedef struct {
  GLuint defaultFbo;
  GLuint *buffers;
  GLuint vao;
  vec2 *spritePosition;
  vec4 *spriteColors;
  vec2 *texturePositions;
  texture *spriteTextures;
  shader currentShader;
  shader *shaderPushPop;
  camera currentCamera;
  camera *cameraPushPop;
  int windowW;
  int windowH;
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
void getTextSize(renderer2d *r2d, const char *text, const font font, const float size, const float spacing, const float line_space);
void renderText(renderer2d *r2d, vec2 position, const char *text, const font f, const vec4 color, const float size, const float spacing, const float line_space, bool showInCenter, const vec4 shadowColor, const vec4 lightColor);
float determineTextRescaleFitSmaller(renderer2d *r2d, const char *str, font *f, vec4 transform, float maxSize);
float determineTextRescaleFit(renderer2d *r2d, const char *str, font *f, vec4 transform);
