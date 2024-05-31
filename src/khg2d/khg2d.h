#pragma once

#include <X11/X.h>
#ifdef _WIN32
#define KHG2D_SIMD 1
#else
#define KHG2D_SIMD 0
#endif

#define KHG2D_USE_OPENGL_130 false

#define KHG2D_DEFAULT_TEXTURE_LOAD_MODE_PIXELATED false
#define KHG2D_DEFAULT_TEXTURE_LOAD_MODE_USE_MIPMAPS true

#define KHG2D_OPNEGL_SHADER_VERSION "#version 330"
#define KHG2D_OPNEGL_SHADER_PRECISION "precision highp float;"

#define KHG2D_DefaulttextureCoords (glm::vec4{ 0, 1, 1, 0 })

#include <stdbool.h>
#include <stddef.h>
#include "glad/glad.h"
#include "stb/stb_image.h"
#include "stb/stb_truetype.h"
#include "../math/vec2.h"
#include "../math/vec4.h"

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

typedef struct shader {
  GLuint id;
  int u_sampler;
} shader;

void bindShader(shader *s);
void clearShader(shader *s);


typedef struct texture {
  GLuint id;
} texture;

vec2 getSize(texture *texture);
void createFromBuffer(texture *texture, const char *image_data, const int width, const int height, bool pixelated, bool useMipMaps);
void create1PxSquare(texture *texture, const char *b);
void createFromFileData(texture *texture, const unsigned char *image_file_data, const size_t image_file_size, bool pixelated, bool useMipMaps);
void createFromFileDataWithPixelPadding(texture *texture, const unsigned char *image_file_data, const size_t image_file_size, int blockSize, bool pixelated, bool useMipMaps);
void loadFromFile(texture *texture, const char *fileName, bool pixelated, bool useMipMaps);
void loadFromFileWithPixelPadding(texture *texture, const char *fileName, int blockSize, bool pixelated, bool useMipMaps);
size_t getMemorySize(texture *texture, int mipLevel, vec2 *outSize);
void readtextureData(texture *texture, void *buffer, int mipLevel);
unsigned char *readtextureDataToCharArray(texture *texture, int mipLevel, vec2 *outSize);
void bindTexture(texture *texture, const unsigned int sample);
void unbindTexture(texture *texture);
void cleanupTexture(texture *texture);

typedef struct textureAtlas {
  int xCount;
	int yCount;
} textureAtlas;

vec4 getTextureAtlas(int x, int y, bool flip);

typedef struct textureAtlasPadding {
		int xCount;
		int yCount;
		int xSize;
		int ySize;
} textureAtlasPadding;

vec4 getTextureAtlasPadding(int x, int y, bool flip);

typedef struct font {
  texture texture;
  vec2 size;
  stbtt_packedchar *packedCharsBuffer;
  int packedCharsBufferSize;
  float maxHeight;
} font;

void createFromTTF(font *f, const unsigned char *ttf_data, const size_t ttf_data_size);
void createFromFile(font *f, const char *file);
void cleanupFont(font *f);

typedef struct camera {
  vec2 position;
  float rotation;
  float zoom;
} camera;

void setDefault(camera *c);
void follow(camera *c, vec2 pos, float speed, float min, float max, float w, float h);

// RENDERER2D

typedef struct FrameBuffer {
  unsigned int fbo;
  texture texture;
  void (*create)(unsigned int w, unsigned int h);
  void (*resize)(unsigned int w, unsigned int h);
  void (*cleanup)(void);
  void (*clear)(void);
} FrameBuffer;

typedef enum Renderer2DBufferType {
  quadPositions,
  quadColors,
  texturePositions,
  bufferSize
} Renderer2DBufferType;

typedef struct Renderer2D {
  GLuint defaultFBO;
  GLuint buffers;
  GLuint vao;
  vec2 *spritePositions;
  vec4 *spriteColors;
  vec2 *texturePositions;
  texture *spritetextures;
  shader currentShader;
  shader *shaderPushPop;
  camera currentCamera;
  camera *cameraPushPop;
  int windowW;
  int windowH;
  FrameBuffer postProcessFBO1;
  FrameBuffer postProcessFBO2;
  bool internalPostProcessFlip;
  void (*create)(GLuint fbo = 0, size_t quadCount = 1000);
  void (*cleanup)(void);
  void (*pushShader)(shader s = {});
  void (*popShader)(void);
  void (*pushCamera)(camera c = {});
  void (*popCamera)(void);
  vec4 (*getViewRect)(void);
  void (*updateWindowMetrics)(int w, int h);
  vec4 (*toScreen)(const vec4 *transform);
  void (*clearDrawData)(void);
  vec2 (*getTextSize)(const char *text, const font f, const float size = 1.5f, const float spacing = 4, const float line_space = 3);
  void (*renderText)(vec2 position, const char *text, const font f, const vec4 color, const float size = 1.5f, const float spacing = 4, 
    const float line_space = 3, bool showInCenter = 1, const vec4 ShadowColor = {0.1,0.1,0.1,1}, const vec4 LightColor = {});
  float (*determineTextRescaleFitSmaller)(const char *str, font *f, vec4 transform, float maxSize);
  float (*determineTextRescaleFit)(const char *str, font *f, vec4 transform);
  int (*wrap)(const char *text, font *f, float baseSize, float maxDimension, char  *outRez);
  void (*renderTextWrapped)(const char *text, font f, vec4 textPos, vec4 color, float baseSize, float spacing = 4, float lineSpacing = 3,
		bool showInCenter = true, vec4 shadowColor = {0.1,0.1,0.1,1}, vec4 lightColor = {});
  vec2 (*getTextSizeWrapped)(const char *text, font f, float maxTextLenght, float baseSize, float spacing = 4, float lineSpacing = 3);
  float (*determineTextRescaleFitBigger)(const char *str, font *f, vec4 transform, float minSize);
  void (*renderRectangletexture)(const vec4 transforms, const texture texture, const vec4 colors[4], const vec2 origin = {}, 
    const float rotationDegrees = 0.f, const vec4 textureCoords = KHG2D_DefaulttextureCoords);
  void (*renderRectangletextureColor)(const vec4 transforms, const texture texture, const vec4 colors = {1,1,1,1}, const vec2 origin = {}, 
    const float rotationDegrees = 0, const vec4 textureCoords = KHG2D_DefaulttextureCoords);
  void (*renderRectangletextureAbsRotation)(const vec4 transforms, const texture texture, const vec4 colors[4], const vec2 origin = {}, 
    const float rotationDegrees = 0.f, const vec4 textureCoords = KHG2D_DefaulttextureCoords);
  void (*renderRectangletextureAbsRotationColor)(const vec4 transforms, const texture texture, const vec4 colors = {1,1,1,1}, const vec2 origin = {}, 
    const float rotationDegrees = 0.f, const vec4 textureCoords = KHG2D_DefaulttextureCoords);
  void (*renderRectangle)(const vec4 transforms, const vec4 colors[4], const vec2 origin = { 0,0 }, const float rotationDegrees = 0);
  void (*renderRectangleColor)(const vec4 transforms, const vec4 colors = {1,1,1,1}, const vec2 origin = {0,0}, const float rotationDegrees = 0);
  void (*renderRectangleAbsRotation)(const vec4 transforms, const vec4 colors[4], const vec2 origin = { 0,0 }, const float rotationDegrees = 0);
  void (*renderRectangleAbsRotationColor)(const vec4 transforms, const vec4 colors = {1,1,1,1}, const vec2 origin = { 0,0 }, const float rotationDegrees = 0);
  void (*renderLineAngle)(const vec2 position, const float angleDegrees, const float length, const vec4 color, const float width = 2.f);
  void (*renderLineStartEnd)(const vec2 start, const vec2 end, const vec4 color, const float width = 2.f);
  void (*renderRectangleOutline)(const vec4 position, const vec4 color, const float width = 2.f, const vec2 origin = {}, const float rotationDegrees = 0);
  void (*renderCircleOutline)(const vec2 position, const vec4 color, const float size, const float width = 2.f, const unsigned int segments = 16);
  void (*renderNinePatch)(const vec4 position, const vec4 color, const vec2 origin, const float rotationDegrees, const texture texture, 
    const vec4 textureCoords, const vec4 inner_texture_coords);
  void (*clearScreen)(const vec4 color = Color4f{ 0, 0, 0, 0 });
  void (*setshader)(const shader shader);
  void (*setCamera)(const camera c);
  void (*resetShaderAndCamera)(void);
  void (*renderPostProcess)(shader shader, texture input, FrameBuffer result = {});
  void (*flush)(bool clearDrawData = true);
  void (*flushFBO)(FrameBuffer frameBuffer, bool clearDrawData = true);
  void (*renderFrameBufferToEntireScreen)(FrameBuffer fbo, FrameBuffer screen = {});
  void (*rendertextureToEntireScreen)(texture t, FrameBuffer screen = {});
  void (*flushPostProcess)(const shader *postProcess, FrameBuffer frameBuffer = {}, bool clearDrawData = true);
  void (*postProcesssOvertexture)(const shader *postProcess, texture in, FrameBuffer framebuffer = {});
} Renderer2D;

// UTILS

void defaultErrorFunc(const char *msg, void *userDefinedData);
void setUserDefinedData(void *data);
typedef void (*errorFuncType)(const char*, void*);
errorFuncType setErrorFuncCallback(errorFuncType newFunc);
float positiontoscreencoordsx(const float position, float w);
float positiontoscreencoordsy(const float position, float h);
stbtt_aligned_quad fontgetglyphquad(const font font, const char c);
vec4 fontgetglyphtexturecoords(const font font, const char c);
vec2 convertpoint(const camera *c, const vec2 *p, float windoww, float windowh);
GLuint loadShader(const char *source, GLenum shaderType);
void init(void);
void cleanup(void);
bool setVsync(bool b);
void enableKHGNecessaryFeatures(void);
vec2 rotateAroundPoint(vec2 vec, vec2 point, const float degrees);
vec2 scaleAroundPoint(vec2 vec, vec2 point, float scale);
shader createshader(const char *vertex, const char *fragment);
shader createShaderFromFile(const char *filePath);
shader createShader(const char *fragment);
shader createPostProcessShaderFromFile(const char *filePath);
shader createPostProcessShader(const char *fragment);
vec4 computetextureAtlas(int xCount, int yCount, int x, int y, bool flip);
vec4 computetextureAtlasWithPadding(int mapXsize, int mapYsize, int xCount, int yCount, int x, int y, bool flip);
