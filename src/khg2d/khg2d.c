#include "khg2d.h"
#include "../math/math.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4244 4305 4267 4996 4018)
#pragma comment(lib, "Opengl32.lib")
#endif

#undef max

static ShaderProgram defaultShader = { 0 };
static Camera defaultCamera = { 0 };
static Texture white1pxSquareTexture = { 0 };

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

void *userDefinedData = 0;
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

stbtt_aligned_quad fontGetGlyphQuad(const FontTexture font, const char c) {
  stbtt_aligned_quad quad = {0};
  float x = 0;
  float y = 0;
  stbtt_GetPackedQuad(font.packedCharsBuffer, font.size.x, font.size.y, c - ' ', &x, &y, &quad, 1);
  return quad;
}

vec4 fontGetGlyphTextureCoords(const FontTexture font, const char c) {
  const stbtt_aligned_quad quad = fontGetGlyphQuad(font, c);
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

#ifdef _WIN32
	typedef BOOL(WINAPI *PFNWGLSWAPINTERVALEXTPROC) (int interval);
#else
	typedef bool(*PFNWGLSWAPINTERVALEXTPROC) (int interval);
#endif

struct {
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
} extensions = { 0 };

bool hasInitialized = 0;

void init(void) {
  if (hasInitialized) { 
    return;
  }
  hasInitialized = true;
  if (!glGenTextures) {
    errorFunc("OpenGL doesn't seem to be initialized, have you forgotten to call gladLoadGL() \
      or gladLoadGLLoader() or glewInit()?", userDefinedData);
  }
  defaultShader = createShaderProgram(defaultVertexShader, defaultFragmentShader);
  white1pxSquareTexture.create1PxSquare(0);
  enableKHGNecessaryFeatures();
}

void cleanup(void) {
  white1pxSquareTexture.cleanup();
  defaultShader.clear();
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
  float a = radians(degrees);
  float s = sinf(a);
  float c = cosf(a);
  point.y = -point.y;
  vector.x -= point.x;
  float newx = vector.x * c - vector.y * s;
  float newy = vector.x * s + vector.y * c; vector.y -= point.y;
  vector.x = newx + point.x;
  vector.y = newy + point.y;
  return vector;
}

vec2 scaleAroundPoint(vec2 vector, vec2 point, float scale) {
  vec2 pointDif = vec2Subtract(&vector, &point);
  vec2 scaledDif = vec2MultiplyNumOnVec2(scale, &pointDif);
  return vec2Add(&scaledDif, &point);
}

// SHADER

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

ShaderProgram createShaderProgram(const char *vertex, const char *fragment) {
  ShaderProgram shader = { 0 };
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

ShaderProgram createShaderFromFile(const char *filePath) {
  FILE *file = fopen(filePath, "rb");
  if (!file) {
      char e[256];
      snprintf(e, sizeof(e), "error opening: %s", filePath);
      errorFunc(e, userDefinedData);
      ShaderProgram emptyShader = { 0 };
      return emptyShader;
  }
  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *fileData = (char *)malloc(fileSize + 1); // null terminated
  if (fileData == NULL) {
      char e[256];
      snprintf(e, sizeof(e), "memory allocation failed for: %s", filePath);
      errorFunc(e, userDefinedData);
      fclose(file);
      ShaderProgram emptyShader = {0}; 
      return emptyShader;
  }
  fread(fileData, 1, fileSize, file);
  fclose(file);
  fileData[fileSize] = '\0';
  ShaderProgram rez = createShader(fileData);
  free(fileData);
  return rez;
}

ShaderProgram createShader(const char *fragment) {
  return createShaderProgram(defaultVertexShader, fragment);
}

ShaderProgram createPostProcessShaderFromFile(const char *filePath) {
  FILE *file = fopen(filePath, "rb");
  if (!file) {
      char e[256];
      snprintf(e, sizeof(e), "error opening: %s", filePath);
      errorFunc(e, userDefinedData);
      ShaderProgram emptyShader = {0};
      return emptyShader;
  }
  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *fileData = (char *)malloc(fileSize + 1); // null terminated
  if (fileData == NULL) {
      char e[256];
      snprintf(e, sizeof(e), "memory allocation failed for: %s", filePath);
      errorFunc(e, userDefinedData);
      fclose(file);
      ShaderProgram emptyShader = {0};
      return emptyShader;
  }
  fread(fileData, 1, fileSize, file);
  fclose(file);
  fileData[fileSize] = '\0';
  ShaderProgram rez = createPostProcessShader(fileData);
  free(fileData);
  return rez;
}

ShaderProgram createPostProcessShader(const char *fragment) {
  return createShaderProgram(defaultVertexPostProcessShader, fragment);
}

// TEXTURE

void convertFromRetardedCoordonates(int tSizeX, int tSizeY, int x, int y, int sizeX, int sizeY, int s1, int s2, int s3, int s4, vec4 *outer, vec4 *inner) {
  float newX = (float)tSizeX / (float)x;
  float newY = (float)tSizeY / (float)y;
  newY = 1 - newY;
  float newSizeX = (float)tSizeX / (float)sizeX;
  float newSizeY = (float)tSizeY / (float)sizeY;
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

// FONT

void createFromTTF(FontTexture *font, const unsigned char *ttf_data, const size_t ttf_data_size) {
  font->size.x = 2000,
  font->size.y = 2000,
  font->maxHeight = 0,
  font->packedCharsBufferSize = ('~' - ' ');

  const size_t fontMonochromeBufferSize = font->size.x * font->size.y;
  const size_t fontRgbaBufferSize = font->size.x * font->size.y * 4;

  unsigned char *fontMonochromeBuffer = malloc(fontMonochromeBufferSize);
  unsigned char *fontRgbaBuffer = malloc(fontRgbaBufferSize);

  font->packedCharsBuffer = malloc(font->packedCharsBufferSize);

  stbtt_pack_context stbtt_context;
  stbtt_PackBegin(&stbtt_context, fontMonochromeBuffer, font->size.x, font->size.y, 0, 2, NULL);
  stbtt_PackSetOversampling(&stbtt_context, 2, 2);
  stbtt_PackFontRange(&stbtt_context, ttf_data, 0, 65, ' ', '~' - ' ', font->packedCharsBuffer);
  stbtt_PackEnd(&stbtt_context);
  for (int i = 0; i < fontMonochromeBufferSize; i++) {
    fontRgbaBuffer[(i * 4)] = fontMonochromeBuffer[i];
    fontRgbaBuffer[(i * 4) + 1] = fontMonochromeBuffer[i];
    fontRgbaBuffer[(i * 4) + 2] = fontMonochromeBuffer[i];
    if (fontMonochromeBuffer[i] > 1) {
      fontRgbaBuffer[(i * 4) + 3] = 255;
    }
    else {
      fontRgbaBuffer[(i * 4) + 3] = 0;
    }
  }
  {
    glGenTextures(1, &font->texture.id);
    glBindTexture(GL_TEXTURE_2D, font->texture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, font->size.x, font->size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, fontRgbaBuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
  free(fontMonochromeBuffer);
  free(fontRgbaBuffer);
  for (char c = ' '; c <= '~'; c++) {
    const stbtt_aligned_quad q = fontGetGlyphQuad(*font, c);
    const float m = q.y1 - q.y0;
    if (m > font->maxHeight && m < 1.e+8f) {
      font->maxHeight = m;
    }
  }
}


