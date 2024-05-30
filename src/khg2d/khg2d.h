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

#define KHG2D_DefaultTextureCoords (glm::vec4{ 0, 1, 1, 0 })

#include <stdbool.h>
#include <stddef.h>
#include "glad/glad.h"
#include "stb/stb_truetype.h"
#include "../math/vec2.h"
#include "../math/vec4.h"

void init(void);
void cleanup(void);
void defaultErrorFunc(const char *msg, void *userDefinedData);
void setUserDefinedData(void *data);
typedef void (*errorFuncType)(const char*, void*);
errorFuncType setErrorFuncCallback(errorFuncType newFunc);
bool setVsync(bool b);
void enableKHGNecessaryFeatures(void);

// SHADERS

typedef struct ShaderProgram {
  GLuint id;
  int u_sampler;
  void (*bind)(void);
  void (*clear)(void);
} ShaderProgram;

ShaderProgram createShaderProgram(const char *vertex, const char *fragment);
ShaderProgram createShaderFromFile(const char *filePath);
ShaderProgram createShader(const char *fragment);
ShaderProgram createPostProcessShaderFromFile(const char *filePath);
ShaderProgram createPostProcessShader(const char *fragment);

// COLORS

#define Colors_Red vec4 { 1, 0, 0, 1 }
#define Colors_Green vec4 { 0, 1, 0, 1 }
#define Colors_Blue vec4 { 0, 0, 1, 1 }
#define Colors_Black vec4 { 0, 0, 0, 1 }
#define Colors_White vec4 { 1, 1, 1, 1 }
#define Colors_Yellow vec4 { 1, 1, 0, 1 }
#define Colors_Magenta vec4 { 1, 0, 1, 1 }
#define Colors_Turqoise vec4 { 0, 1, 1, 1 }
#define Colors_Orange vec4 { 1, (float)0x7F / 255.0f, 0, 1 }
#define Colors_Purple vec4 { 101.0f / 255.0f, 29.0f / 255.0f, 173.0f / 255.0f, 1 }
#define Colors_Gray vec4 { (float)0x7F / 255.0f, (float)0x7F / 255.0f, (float)0x7F / 255.0f, 1 }
#define Colors_Transparent vec4 { 0, 0, 0, 0 }

// MATH

vec2 rotateAroundPoint(vec2 vec, vec2 point, const float degrees);
vec2 scaleAroundPoint(vec2 vec, vec2 point, float scale);

typedef struct Texture {
  GLuint id;
  vec2 (*getSize)(void);
  void (*createFromBuffer)(const char *image_data, const int width, const int height, 
    bool pixelated = KHG2D_DEFAULT_TEXTURE_LOAD_MODE_PIXELATED, 
    bool useMipMaps = KHG2D_DEFAULT_TEXTURE_LOAD_MODE_USE_MIPMAPS);
  void (*create1PxSquare)(const char *b = 0);
  void (*createFromFileData)(const unsigned char *image_file_data, const size_t image_file_size, 
		bool pixelated = KHG2D_DEFAULT_TEXTURE_LOAD_MODE_PIXELATED, bool useMipMaps = GL2D_DEFAULT_TEXTURE_LOAD_MODE_USE_MIPMAPS);
  void (*createFromFileDataWithPixelPadding)(const unsigned char *image_file_data, const size_t image_file_size, int blockSize,
		bool pixelated = KHG2D_DEFAULT_TEXTURE_LOAD_MODE_PIXELATED, bool useMipMaps = GL2D_DEFAULT_TEXTURE_LOAD_MODE_USE_MIPMAPS);
  void (*loadFromFile)(const char *fileName,
	  bool pixelated = KHG2D_DEFAULT_TEXTURE_LOAD_MODE_PIXELATED, bool useMipMaps = GL2D_DEFAULT_TEXTURE_LOAD_MODE_USE_MIPMAPS);
  void (*loadFromFileWithPixelPadding)(const char *fileName, int blockSize,
		bool pixelated = KHG2D_DEFAULT_TEXTURE_LOAD_MODE_PIXELATED, bool useMipMaps = GL2D_DEFAULT_TEXTURE_LOAD_MODE_USE_MIPMAPS);
  size_t (*getMemorySize)(int mipLevel = 0, vec2 *outSize = 0);
  void (*readTextureData)(void *buffer, int mipLevel = 0);
  unsigned char *(*readTextureDataToCharArray)(int mipLevel = 0, vec2 *outSize = 0);
  void (*bind)(const unsigned int sample = 0);
  void (*unbind)(void);
  void (*cleanup)(void);
} Texture;

// TEXTURE ATLAS

vec4 computeTextureAtlas(int xCount, int yCount, int x, int y, bool flip);
vec4 computeTextureAtlasWithPadding(int mapXsize, int mapYsize, int xCount, int yCount, int x, int y, bool flip);

typedef struct TextureAtlas {
  int xCount;
	int yCount;
  vec4 (*get)(int x, int y, bool flip = 0);
} TextureAtlas;

typedef struct TextureAtlasPadding {
		int xCount;
		int yCount;
		int xSize;
		int ySize;
    vec4 (*get)(int x, int y, bool flip = 0);
} TextureAtlasPadding;

// FONT

typedef struct FontTexture {
  Texture texture;
  vec2 size;
  stbtt_packedchar *packedCharsBuffer;
  int packedCharsBufferSize;
  float maxHeight;
  void (*createFromTTF)(const unsigned char *ttf_data, const size_t ttf_data_size);
  void (*createFromFile)(const char *file);
  void (*cleanup)(void);
} FontTexture;

// CAMERA

typedef struct Camera {
  vec2 position;
  float rotation;
  float zoom;
  void (*setDefault)(void);
  void (*follow)(vec2 pos, float speed, float min, float max, float w, float h);
} Camera;

// RENDERER2D

typedef struct FrameBuffer {
  unsigned int fbo;
  Texture texture;
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
  Texture *spriteTextures;
  ShaderProgram currentShader;
  ShaderProgram *shaderPushPop;
  Camera currentCamera;
  Camera *cameraPushPop;
  int windowW;
  int windowH;
  FrameBuffer postProcessFBO1;
  FrameBuffer postProcessFBO2;
  bool internalPostProcessFlip;
  void (*create)(GLuint fbo = 0, size_t quadCount = 1000);
  void (*cleanup)(void);
  void (*pushShader)(ShaderProgram s = {});
  void (*popShader)(void);
  void (*pushCamera)(Camera c = {});
  void (*popCamera)(void);
  vec4 (*getViewRect)(void);
  void (*updateWindowMetrics)(int w, int h);
  vec4 (*toScreen)(const vec4 *transform);
  void (*clearDrawData)(void);
  vec2 (*getTextSize)(const char *text, const FontTexture font, const float size = 1.5f, const float spacing = 4, const float line_space = 3);
  void (*renderText)(vec2 position, const char *text, const FontTexture font, const vec4 color, const float size = 1.5f, const float spacing = 4, 
    const float line_space = 3, bool showInCenter = 1, const vec4 ShadowColor = {0.1,0.1,0.1,1}, const vec4 LightColor = {});
  float (*determineTextRescaleFitSmaller)(const char *str, FontTexture *f, vec4 transform, float maxSize);
  float (*determineTextRescaleFit)(const char *str, FontTexture *f, vec4 transform);
  int (*wrap)(const char *text, FontTexture *f, float baseSize, float maxDimension, char  *outRez);
  void (*renderTextWrapped)(const char *text, FontTexture f, vec4 textPos, vec4 color, float baseSize, float spacing = 4, float lineSpacing = 3,
		bool showInCenter = true, vec4 shadowColor = {0.1,0.1,0.1,1}, vec4 lightColor = {});
  vec2 (*getTextSizeWrapped)(const char *text, FontTexture f, float maxTextLenght, float baseSize, float spacing = 4, float lineSpacing = 3);
  float (*determineTextRescaleFitBigger)(const char *str, FontTexture *f, vec4 transform, float minSize);
  void (*renderRectangleTexture)(const vec4 transforms, const Texture texture, const vec4 colors[4], const vec2 origin = {}, 
    const float rotationDegrees = 0.f, const vec4 textureCoords = KHG2D_DefaultTextureCoords);
  void (*renderRectangleTextureColor)(const vec4 transforms, const Texture texture, const vec4 colors = {1,1,1,1}, const vec2 origin = {}, 
    const float rotationDegrees = 0, const vec4 textureCoords = KHG2D_DefaultTextureCoords);
  void (*renderRectangleTextureAbsRotation)(const vec4 transforms, const Texture texture, const vec4 colors[4], const vec2 origin = {}, 
    const float rotationDegrees = 0.f, const vec4 textureCoords = KHG2D_DefaultTextureCoords);
  void (*renderRectangleTextureAbsRotationColor)(const vec4 transforms, const Texture texture, const vec4 colors = {1,1,1,1}, const vec2 origin = {}, 
    const float rotationDegrees = 0.f, const vec4 textureCoords = KHG2D_DefaultTextureCoords);
  void (*renderRectangle)(const vec4 transforms, const vec4 colors[4], const vec2 origin = { 0,0 }, const float rotationDegrees = 0);
  void (*renderRectangleColor)(const vec4 transforms, const vec4 colors = {1,1,1,1}, const vec2 origin = {0,0}, const float rotationDegrees = 0);
  void (*renderRectangleAbsRotation)(const vec4 transforms, const vec4 colors[4], const vec2 origin = { 0,0 }, const float rotationDegrees = 0);
  void (*renderRectangleAbsRotationColor)(const vec4 transforms, const vec4 colors = {1,1,1,1}, const vec2 origin = { 0,0 }, const float rotationDegrees = 0);
  void (*renderLineAngle)(const vec2 position, const float angleDegrees, const float length, const vec4 color, const float width = 2.f);
  void (*renderLineStartEnd)(const vec2 start, const vec2 end, const vec4 color, const float width = 2.f);
  void (*renderRectangleOutline)(const vec4 position, const vec4 color, const float width = 2.f, const vec2 origin = {}, const float rotationDegrees = 0);
  void (*renderCircleOutline)(const vec2 position, const vec4 color, const float size, const float width = 2.f, const unsigned int segments = 16);
  void (*renderNinePatch)(const vec4 position, const vec4 color, const vec2 origin, const float rotationDegrees, const Texture texture, 
    const vec4 textureCoords, const vec4 inner_texture_coords);
  void (*clearScreen)(const vec4 color = Color4f{ 0, 0, 0, 0 });
  void (*setShaderProgram)(const ShaderProgram shader);
  void (*setCamera)(const Camera camera);
  void (*resetShaderAndCamera)(void);
  void (*renderPostProcess)(ShaderProgram shader, Texture input, FrameBuffer result = {});
  void (*flush)(bool clearDrawData = true);
  void (*flushFBO)(FrameBuffer frameBuffer, bool clearDrawData = true);
  void (*renderFrameBufferToEntireScreen)(FrameBuffer fbo, FrameBuffer screen = {});
  void (*renderTextureToEntireScreen)(Texture t, FrameBuffer screen = {});
  void (*flushPostProcess)(const ShaderProgram *postProcess, FrameBuffer frameBuffer = {}, bool clearDrawData = true);
  void (*postProcesssOverTexture)(const ShaderProgram *postProcess, Texture in, FrameBuffer framebuffer = {});
} Renderer2D;

// UTILS

float positiontoscreencoordsx(const float position, float w);
float positiontoscreencoordsy(const float position, float h);
stbtt_aligned_quad fontgetglyphquad(const FontTexture font, const char c);
vec4 fontgetglyphtexturecoords(const FontTexture font, const char c);
vec2 convertpoint(const Camera *c, const vec2 *p, float windoww, float windowh);
GLuint loadShader(const char *source, GLenum shaderType);
