#include "khg2d.h"
#include "../math/math.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4244 4305 4267 4996 4018)
#pragma comment(lib, "Opengl32.lib")
#endif

#ifdef _WIN32
	typedef BOOL(WINAPI *PFNWGLSWAPINTERVALEXTPROC) (int interval);
#else
	typedef bool(*PFNWGLSWAPINTERVALEXTPROC) (int interval);
#endif

struct {
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
} extensions = { 0 };

#undef max

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



static errorFuncType errorFunc = defaultErrorFunc;

void defaultErrorFunc(const char *msg, void *userDefinedData) {
  printf("KHG2D error: %s\n", msg);
}

void setUserDefinedData(void *data) {
  userDefinedData = data;
}

errorFuncType setErrorFuncCallback(errorFuncType newFunc) {
  errorFuncType a = errorFunc;
  errorFunc = newFunc;
  return a;
}

float positionToScreenCoordsX(const float position, float w) {
  return (position / w) * 2 - 1;
}

float positionToScreenCoordsY(const float position, float h) {
  return -((-position / h) * 2 - 1);
}

stbtt_aligned_quad fontGetGlyphQuad(const font f, const char c) {
  stbtt_aligned_quad quad = { 0 };
  float x = 0;
  float y = 0;
  stbtt_GetPackedQuad(f.packedCharsBuffer, f.size.x, f.size.y, c - ' ', &x, &y, &quad, 1);
  return quad;
}

vec4 fontGetGlyphTextureCoords(const font f, const char c) {
  const stbtt_aligned_quad quad = fontGetGlyphQuad(f, c);
  return (vec4){quad.s0, quad.t0, quad.s1, quad.t1};
}

GLuint loadShader(const char *source, GLenum shaderType) {
  GLuint id = glCreateShader(shaderType);
  int result = 0;
  glShaderSource(id, 1, &source, 0);
  glCompileShader(id);
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if (!result) {
    char* message = 0;
    int l = 0;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &l);
    message = malloc(l);
    glGetShaderInfoLog(id, l, &l, message);
    message[l - 1] = 0;
    errorFunc(message, userDefinedData);
    free(message);
  }
  return id;
}

void init(void) {
  if (hasInitialized) { 
    return;
  }
  hasInitialized = true;
  if (!glGenTextures) {
    errorFunc("OpenGL doesn't seem to be initialized, have you forgotten to call gladLoadGL() \
      or gladLoadGLLoader() or glewInit()?", userDefinedData);
  }
  defaultShader = createshader(defaultVertexShader, defaultFragmentShader);
  create1PxSquare(&white1pxSquareTexture, 0);
  enableKHGNecessaryFeatures();
}

void cleanup(void) {
  cleanupTexture(&white1pxSquareTexture);
  clearShader(&defaultShader);
  hasInitialized = false;
}

bool setVsync(bool b) {
  if (extensions.wglSwapIntervalEXT != NULL) {
    bool rezult = extensions.wglSwapIntervalEXT(b);
    return rezult;
  }
  else {
    return false;
  }
}

vec2 rotateAroundPoint(vec2 vector, vec2 point, const float degrees) {
  float newx, newy;
  float a = radians(degrees);
  float s = sinf(a);
  float c = cosf(a);
  point.y = -point.y;
  vector.x -= point.x;
  newx = vector.x * c - vector.y * s;
  newy = vector.x * s + vector.y * c; vector.y -= point.y;
  vector.x = newx + point.x;
  vector.y = newy + point.y;
  return vector;
}

vec2 scaleAroundPoint(vec2 vector, vec2 point, float scale) {
  vec2 pointDif = vec2Subtract(&vector, &point);
  vec2 scaledDif = vec2MultiplyNumOnVec2(scale, &pointDif);
  return vec2Add(&scaledDif, &point);
}

void validateProgram(GLuint id) {
  int info = 0;
  glGetProgramiv(id, GL_LINK_STATUS, &info);
  if (info != GL_TRUE) {
    char *message = 0;
    int l = 0;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &l);
    message = malloc(l);
    glGetProgramInfoLog(id, l, &l, message);
    errorFunc(message, userDefinedData);
    free(message);
  }
  glValidateProgram(id);
}

shader createShaderProgram(const char *vertex, const char *fragment) {
  shader shader = { 0 };
  const GLuint vertexId = loadShader(vertex, GL_VERTEX_SHADER);
  const GLuint fragmentId = loadShader(fragment, GL_FRAGMENT_SHADER);
  shader.id = glCreateProgram();
  glAttachShader(shader.id, vertexId);
  glAttachShader(shader.id, fragmentId);
  glBindAttribLocation(shader.id, 0, "quad_positions");
  glBindAttribLocation(shader.id, 1, "quad_colors");
  glBindAttribLocation(shader.id, 2, "texturePositions");
  glLinkProgram(shader.id);
  glDeleteShader(vertexId);
  glDeleteShader(fragmentId);
  validateProgram(shader.id);
  shader.u_sampler = glGetUniformLocation(shader.id, "u_sampler");
  return shader;
}

shader createShaderFromFile(const char *filePath) {
  FILE *file = fopen(filePath, "rb");
  shader emptyShader, rez;
  long fileSize;
  char *fileData;
  if (!file) {
      char e[256];
      snprintf(e, sizeof(e), "error opening: %s", filePath);
      errorFunc(e, userDefinedData);
      emptyShader.id = 0;
      emptyShader.u_sampler = 0;
      return emptyShader;
  }
  fseek(file, 0, SEEK_END);
  fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);
  fileData = (char *)malloc(fileSize + 1);
  if (fileData == NULL) {
      char e[256];
      snprintf(e, sizeof(e), "memory allocation failed for: %s", filePath);
      errorFunc(e, userDefinedData);
      fclose(file);
      emptyShader.id = 0;
      emptyShader.u_sampler = 0;
      return emptyShader;
  }
  fread(fileData, 1, fileSize, file);
  fclose(file);
  fileData[fileSize] = '\0';
  rez = createShader(fileData);
  free(fileData);
  return rez;
}

shader createShader(const char *fragment) {
  return createshader(defaultVertexShader, fragment);
}

shader createPostProcessShaderFromFile(const char *filePath) {
  FILE *file = fopen(filePath, "rb");
  shader emptyShader, rez;
  long fileSize;
  char *fileData;
  if (!file) {
    char e[256];
    snprintf(e, sizeof(e), "error opening: %s", filePath);
    errorFunc(e, userDefinedData);
    emptyShader.id = 0;
    emptyShader.u_sampler = 0;
    return emptyShader;
  }
  fseek(file, 0, SEEK_END);
  fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);
  fileData = (char *)malloc(fileSize + 1); // null terminated
  if (fileData == NULL) {
    char e[256];
    snprintf(e, sizeof(e), "memory allocation failed for: %s", filePath);
    errorFunc(e, userDefinedData);
    fclose(file);
    emptyShader.id = 0;
    emptyShader.u_sampler = 0;
    return emptyShader;
  }
  fread(fileData, 1, fileSize, file);
  fclose(file);
  fileData[fileSize] = '\0';
  rez = createPostProcessShader(fileData);
  free(fileData);
  return rez;
}

shader createPostProcessShader(const char *fragment) {
  return createshader(defaultVertexPostProcessShader, fragment);
}

void cleanTextureCoordinates(int tSizeX, int tSizeY, int x, int y, int sizeX, int sizeY, int s1, int s2, int s3, int s4, vec4 *outer, vec4 *inner) {
  float newX = (float)tSizeX / (float)x;
  float newY = (float)tSizeY / (float)y;
  float newSizeX = (float)tSizeX / (float)sizeX;
  float newSizeY = (float)tSizeY / (float)sizeY;
  newY = 1 - newY;
  if (outer) {
    outer->x = newX;
    outer->y = newY;
    outer->z = newX + newSizeX;
    outer->w = newY - newSizeY;
  }
  if (inner) {
    inner->x = newX + ((float)s1 / tSizeX);
    inner->y = newY - ((float)s2 / tSizeY);
    inner->z = newX + newSizeX - ((float)s3 / tSizeX);
    inner->w = newY - newSizeY + ((float)s4 / tSizeY);
  }
}

