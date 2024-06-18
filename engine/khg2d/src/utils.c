#include "khgmath/math.h"
#include "khgmath/vec4.h"
#include "khg2d/renderer2d.h"
#include "khg2d/shader.h"
#include "khg2d/utils.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#undef max

bool hasInitialized = false;
void *userDefinedData = 0;
shader defaultShader = { 0 };
camera defaultCamera = { 0 };
texture white1pxSquareTexture = { 0 };

char *defaultVertexShader = "";
char *defaultFragmentShader = "";
char *defaultVertexPostProcessShader = "";

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

vec2 convertPoint(const camera *c, const vec2 *p, float windowW, float windowH) {
  vec2 r = *p;
  vec2 cameraCenter1, cameraCenter2;
  cameraCenter1.x = c->position.x + windowW / 2;
  cameraCenter1.y = c->position.y - windowH / 2;
  cameraCenter2.x = c->position.x + windowW / 2;
  cameraCenter2.y = c->position.y + windowH / 2;
  r.x = c->position.x;
  r.y = c->position.y;
  r = rotateAroundPoint(r, cameraCenter1, c->rotation);
  r = scaleAroundPoint(r, cameraCenter2, 1 / c->zoom);
  return r;
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
  defaultVertexShader = loadFileContents("./res/shaders/defaultVertexShader.vert");
  defaultFragmentShader = loadFileContents("./res/shaders/defaultFragmentShader.frag");
  defaultVertexPostProcessShader = loadFileContents("./res/shaders/defaultVertexPostProcessShader.vert");
  defaultShader = createShader(defaultVertexShader, defaultFragmentShader);
  defaultCamera = createCamera();
  create1PxSquare(&white1pxSquareTexture, 0);
  enableGLNecessaryFeatures();
}

void cleanup(void) {
  cleanupTexture(&white1pxSquareTexture);
  clearShader(&defaultShader);
  hasInitialized = false;
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

camera createCamera(void) {
  vec2 initPos = { 0.0f, 0.0f };
  camera c = { 0 };
  c.zoom = 1;
  c.position = initPos;
  c.rotation = 0.0f;
  return c;
}

shader createShader(const char *vertex, const char *fragment) {
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
  rez = createShaderDefaultVertex(fileData);
  free(fileData);
  return rez;
}

shader createShaderDefaultVertex(const char *fragment) {
  return createShader(defaultVertexShader, fragment);
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
  return createShader(defaultVertexPostProcessShader, fragment);
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

char *loadFileContents(char const *path) {
    char *buffer = 0;
    long length;
    FILE *f = fopen (path, "rb");
    fseek (f, 0, SEEK_END);
    length = ftell (f);
    fseek (f, 0, SEEK_SET);
    buffer = (char*)malloc ((length+1)*sizeof(char));
    if (buffer) {
      fread (buffer, sizeof(char), length, f);
    }
    fclose (f);
    buffer[length] = '\0';
    return buffer;
}
