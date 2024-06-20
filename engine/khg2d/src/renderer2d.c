#include "khgmath/minmax.h"
#include "khgmath/math.h"
#include "khgmath/mat3.h"
#include "khg2d/renderer2d.h"
#include "khg2d/utils.h"
#include "glad/glad.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

void internalFlush(renderer2d *r2d, bool shouldClear) {
  const int size = cvector_size(r2d->spriteTextures);
  unsigned int id = r2d->spriteTextures[0].id;
  enableGLNecessaryFeatures();
  if (!hasInitialized) {
    errorFunc("Library not initialized. Have you forgotten to call gl2d::init() ?", userDefinedData);
    clearDrawData(r2d);
    return;
  }
  if (!r2d->vao) {
    clearDrawData(r2d);
    errorFunc("Renderer not initialized. Have you forgotten to call gl2d::Renderer2D::create() ?", userDefinedData);
    return;
  }
  if (r2d->windowH <= 0 || r2d->windowW <= 0) {
    if (shouldClear) {
      errorFunc("Negative windowW or windowH, have you forgotten to call updateWindowMetrics(w, h)?", userDefinedData);
      clearDrawData(r2d);
    }
    return;
  }
  if (cvector_empty(r2d->spriteTextures)) {
    return; 
  }
  glViewport(0, 0, r2d->windowW, r2d->windowH);
  glBindVertexArray(r2d->vao);
  glUseProgram(r2d->currentShader.id);
  glUniform1i(r2d->currentShader.u_sampler, 0);
  glBindBuffer(GL_ARRAY_BUFFER, r2d->buffers[quadPositions]);
  glBufferData(GL_ARRAY_BUFFER, cvector_size(r2d->spritePositions) * sizeof(vec2), r2d->spritePositions, GL_STREAM_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, r2d->buffers[quadColors]);
  glBufferData(GL_ARRAY_BUFFER, cvector_size(r2d->spriteColors) * sizeof(vec4), r2d->spriteColors, GL_STREAM_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, r2d->buffers[texturePositions]);
  glBufferData(GL_ARRAY_BUFFER, cvector_size(r2d->texturePositions) * sizeof(vec2), r2d->texturePositions, GL_STREAM_DRAW);
  bindTexture(&r2d->spriteTextures[0], 0);
  int pos = 0;
  for (int i = 1; i < size; i++) {
    if (r2d->spriteTextures[i].id != id) {
      glDrawArrays(GL_TRIANGLES, pos * 6, 6 * (i - pos));
      pos = i;
      id = r2d->spriteTextures[i].id;
      bindTexture(&r2d->spriteTextures[i], 0);
    }
  }
  glDrawArrays(GL_TRIANGLES, pos * 6, 6 * (size - pos));
  glBindVertexArray(0);
  if (shouldClear) {
    clearDrawData(r2d);
  }
}

void renderQuadToScreenInternal(renderer2d *r2d) {
  static float positions[12] = { -1, 1, -1, -1, 1, 1, 1, 1, -1, -1, 1, -1 };
  static float colors[6 * 4] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
  static float texCoords[12] = { 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0 };
  glBindVertexArray(r2d->vao);
  glBindBuffer(GL_ARRAY_BUFFER, r2d->buffers[quadPositions]);
  glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(vec2), positions, GL_STREAM_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, r2d->buffers[quadColors]);
  glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(vec4), colors, GL_STREAM_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, r2d->buffers[texturePositions]);
  glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(vec2), texCoords, GL_STREAM_DRAW);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void renderPoint(renderer2d *r2d, vec2 pos) {
  vec4 colors[4] = { colorBlack, colorBlack, colorBlack, colorBlack };
  renderRectangle(r2d, (vec4){ pos.x - 1.0f, pos.y - 1.0f, 2.0f, 2.0f }, colors, (vec2){ 0.0f, 0.0f }, 0);
}

vec2 calcPos(int p, float size, int segments, vec2 position) {
  vec2 circle = { size, 0.0f };
  float a = 3.1415926 * 2 * ((float)p / segments);
  float c = cos(a);
  float s = sin(a);
  circle = (vec2){ c * circle.x - s * circle.y, s * circle.x - c * circle.y };
  return vec2Add(&circle, &position);
}

void createRenderer2d(renderer2d *r2d, GLuint fbo, size_t quadCount) {
  if (!hasInitialized) {
		errorFunc("Library not initialized. Have you forgotten to call gl2d::init() ?", userDefinedData);
  }
  r2d->defaultFbo = fbo;
  r2d->spritePositions = NULL;
  r2d->spriteColors = NULL;
  r2d->texturePositions = NULL;
  r2d->spriteTextures = NULL;
  clearDrawData(r2d);
  cvector_reserve(r2d->spritePositions, quadCount * 6);
  cvector_reserve(r2d->spriteColors, quadCount * 6);
  cvector_reserve(r2d->texturePositions, quadCount * 6);
  cvector_reserve(r2d->spriteTextures, quadCount);
  resetShaderAndCamera(r2d);
  glGenVertexArrays(1, &r2d->vao);
  glBindVertexArray(r2d->vao);
  glGenBuffers(bufferSize, r2d->buffers);
  glBindBuffer(GL_ARRAY_BUFFER, r2d->buffers[quadPositions]);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glBindBuffer(GL_ARRAY_BUFFER, r2d->buffers[quadColors]);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glBindBuffer(GL_ARRAY_BUFFER, r2d->buffers[texturePositions]);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glBindVertexArray(0);
}

void cleanupRenderer2d(renderer2d *r2d) {
  glDeleteVertexArrays(1, &r2d->vao);
  glDeleteBuffers(bufferSize, r2d->buffers);
  cleanupFramebuffer(&r2d->postProcessFbo1);
  cleanupFramebuffer(&r2d->postProcessFbo2);
  r2d->internalPostProcessFlip = 0;
  cvector_free(r2d->spritePositions);
  cvector_free(r2d->spriteColors);
  cvector_free(r2d->texturePositions);
  cvector_free(r2d->spriteTextures);
}

void pushShader(renderer2d *r2d, shader s) {
  cvector_push_back(r2d->shaderPushPop, s);
  r2d->currentShader = s;
}

void popShader(renderer2d *r2d) {
  if (cvector_empty(r2d->shaderPushPop)) {
    errorFunc("Pop on an empty stack on popShader", userDefinedData);
  }
  else {
    r2d->currentShader = *cvector_back(r2d->shaderPushPop);
    cvector_pop_back(r2d->shaderPushPop);
  }
}

void pushCamera(renderer2d *r2d, camera c) {
  cvector_push_back(r2d->cameraPushPop, c);
  r2d->currentCamera = c;
}

void popCamera(renderer2d *r2d) {
  if (cvector_empty(r2d->cameraPushPop)) {
    errorFunc("Pop on an empty stack on popCamera", userDefinedData);
  }
  else {
    r2d->currentCamera = *cvector_back(r2d->cameraPushPop);
    cvector_pop_back(r2d->cameraPushPop);
  }
}

vec4 getViewRect(renderer2d *r2d) {
  vec4 rect = { 0.0f, 0.0f, r2d->windowW, r2d->windowH };
  mat3 mat;
  mat.values[0] = 1.0f;
  mat.values[1] = 0.0f;
  mat.values[2] = r2d->currentCamera.position.x;
  mat.values[3] = 0.0f;
  mat.values[4] = 1.0f;
  mat.values[5] = r2d->currentCamera.position.y;
  mat.values[6] = 0.0f;
  mat.values[7] = 0.0f;
  mat.values[8] = 1.0f;
  mat = mat3Transpose(&mat);
  vec3 pos1 = { rect.x, rect.y, 1.0f };
  vec3 pos2 = { rect.z + rect.x, rect.w + rect.y, 1.0f };
  pos1 = mat3TransformVec3(&mat, &pos1);
  pos2 = mat3TransformVec3(&mat, &pos2);
  vec2 point = { (pos1.x + pos2.y) / 2.0f, (pos1.y + pos2.y) / 2.0f };
  vec2 scalePoint1 = scaleAroundPoint((vec2){ pos1.x, pos1.y }, point, 1.0f / r2d->currentCamera.zoom);
  vec2 scalePoint2 = scaleAroundPoint((vec2){ pos2.x, pos2.y }, point, 1.0f / r2d->currentCamera.zoom);
  pos1 = (vec3){ scalePoint1.x, scalePoint1.y, 1.0f };
  pos2 = (vec3){ scalePoint2.x, scalePoint2.y, 1.0f };
  rect = (vec4){ pos1.x, pos1.y, pos2.x - pos1.x, pos2.y - pos1.y };
  return rect;
}

void updateWindowMetrics(renderer2d *r2d, int w, int h) {
  r2d->windowW = w;
  r2d->windowH = h;
}

vec4 pixToScreen(renderer2d *r2d, const vec4 *transform) {
  const float transformY = transform->y * -1.0f;
  vec2 v1 = { transform->x, transformY };
  vec2 v3 = { transform->x + transform->z, transformY - transform->w };
  v1.x -= r2d->currentCamera.position.x;
  v1.y += r2d->currentCamera.position.y;
  v3.x -= r2d->currentCamera.position.x;
  v3.y += r2d->currentCamera.position.y;
  vec2 cameraCenter = { r2d->windowW / 2.0f, r2d->windowH / 2.0f };
  v1 = scaleAroundPoint(v1, cameraCenter, r2d->currentCamera.zoom);
  v3 = scaleAroundPoint(v3, cameraCenter, r2d->currentCamera.zoom);
  v1.x = positionToScreenCoordsX(v1.x, r2d->windowW);
  v3.x = positionToScreenCoordsX(v3.x, r2d->windowW);
  v1.y = positionToScreenCoordsY(v1.y, r2d->windowH);
  v3.y = positionToScreenCoordsY(v3.y, r2d->windowH);
  vec4 result = { v1.x, v1.y, v3.x, v3.y };
  return result;
}

void clearDrawData(renderer2d *r2d) {
  cvector_clear(r2d->spritePositions);
  cvector_clear(r2d->spriteColors);
  cvector_clear(r2d->texturePositions);
  cvector_clear(r2d->spriteTextures);
}

vec2 getTextSize(renderer2d *r2d, const char *text, const font font, const float size, const float spacing, const float line_space) {
  if (font.texture.id == 0) {
    errorFunc("Missing font", userDefinedData);
    return (vec2){ 0.0f, 0.0f };
  }
  vec2 position = { 0.0f, 0.0f };
  const int textLength = (int)strlen(text);
  vec4 rectangle;
  rectangle.x = position.x;
  float linePositionY = position.y, maxPos = 0, maxPosY = 0, bonusY = 0;
  for (int i = 0; i< textLength; i++) {
    if (text[i] == '\n') {
      rectangle.x = position.x;
      linePositionY += (font.maxHeight + line_space) * size;
      bonusY += (font.maxHeight + line_space) * size;
      maxPosY = 0;
    }
    else if (text[i] == '\t') {
      const stbtt_aligned_quad quad = fontGetGlyphQuad(font, '_');
      float x = quad.x1 - quad.x0;
      rectangle.x += x * size * 3 + spacing * size;
    }
    else if (text[i] == ' ') {
      const stbtt_aligned_quad quad = fontGetGlyphQuad(font, ' ');
      float x = quad.x1 - quad.x0;
      rectangle.x += x * size + spacing * size;
    }
    else if (text[i] >= ' ' && text[i] <= '~') {
      const stbtt_aligned_quad quad = fontGetGlyphQuad(font, text[i]);
      rectangle.z = quad.x1 - quad.x0;
      rectangle.w = quad.y1 - quad.y0;
      rectangle.z *= size;
      rectangle.w *= size;
      rectangle.y = linePositionY + quad.y0 * size;
      rectangle.x += rectangle.z + spacing * size;
      maxPosY = max(maxPosY, rectangle.y);
      maxPos = max(maxPos, rectangle.x);
    }
  }
  maxPos = max(maxPos, rectangle.x);
  maxPos = max(maxPosY, rectangle.y);
  float paddX = maxPos;
  float paddY = maxPosY;
  paddY += font.maxHeight * size + bonusY;
  vec2 result = { paddX, paddY };
  return result;
}

void renderText(renderer2d *r2d, vec2 position, const char *text, const font f, const vec4 color, const float size, const float spacing, const float line_space, short showInCenter, const vec4 shadowColor, const vec4 lightColor) {
  if (f.texture.id == 0) {
    errorFunc("Missing font", userDefinedData);
    return;
  } 
  const int text_length = (int)strlen(text);
  vec4 rectangle = { 0.0f, 0.0f, 0.0f, 0.0f };
  if (showInCenter) {
    vec2 textSize = getTextSize(r2d, text, f, size, spacing, line_space);
    position.x -= textSize.x / 2.0f;
    position.y -= textSize.y / 2.0f;
  }
  rectangle.x = position.x;
  float linePositionY = position.y;
  for (int i = 0; i < text_length; i++) {
    if (text[i] == '\n') {
      rectangle.x = position.x;
      linePositionY += (f.maxHeight + line_space) * size;
    }
    else if (text[i] == '\t') {
      const stbtt_aligned_quad quad = fontGetGlyphQuad(f, '_');
      float x = quad.x1 - quad.x0;
      rectangle.x += x * size * 3 + spacing * size;
    }
    else if (text[i] == ' ') {
      const stbtt_aligned_quad quad = fontGetGlyphQuad(f, '_');
      float x = quad.x1 - quad.x0;
      rectangle.x += x * size + spacing * size;
    }
    else if (text[i] >= ' ' && text[i] <= '~') {
      const stbtt_aligned_quad quad = fontGetGlyphQuad(f, text[i]);
      vec4 colorData[4] = { color, color, color, color };
      vec4 textureCoords = { quad.s0, quad.t0, quad.s1, quad.t1 };
      vec2 origin = { 0.0f, 0.0f };
      rectangle.z = quad.x1 - quad.x0;
      rectangle.w = quad.y1 - quad.y0;
      rectangle.z *= size;
      rectangle.y = linePositionY + quad.y0 * size;
      if (shadowColor.w) {
        vec2 pos = { -5.0f, 3.0f };
        vec4 shadowColorData[4] = { shadowColor, shadowColor, shadowColor, shadowColor };
        pos = vec2MultiplyNumOnVec2(size, &pos);
        vec4 transform = { rectangle.x + pos.x, rectangle.y + pos.y, rectangle.z, rectangle.w };
        renderRectangleTexture(r2d, transform, f.texture, shadowColorData, origin, 0, textureCoords);
      }
      renderRectangleTexture(r2d, rectangle, f.texture, colorData, origin, 0, textureCoords);
      if (lightColor.w) {
        vec2 pos = { -2.0f, 1.0f };
        vec4 lightColorData[4] = { lightColor, lightColor, lightColor, lightColor };
        pos = vec2MultiplyNumOnVec2(size, &pos);
        vec4 transform = { rectangle.x + pos.x, rectangle.y + pos.y, rectangle.z, rectangle.w };
        renderRectangleTexture(r2d, transform, f.texture, lightColorData, origin, 0, textureCoords);
      }
      rectangle.x += rectangle.z + spacing * size;
    }
  }
}

float determineTextRescaleFitSmaller(renderer2d *r2d, const char **str, font *f, vec4 transform, float maxSize) {
  vec2 s = getTextSize(r2d, *str, *f, maxSize, 4, 3);
  float ratioX = transform.z / s.x;
  float ratioY = transform.w / s.y;
  if (ratioX > 1 && ratioY > 1) {
    return maxSize;
  }
  else {
    if (ratioX < ratioY) {
      return maxSize * ratioX;
    }
    else {
      return maxSize * ratioY;
    }
  }
}

float determineTextRescaleFitBigger(renderer2d *r2d, const char **str, font *f, vec4 transform, float minSize) {
  vec2 s = getTextSize(r2d, *str, *f, minSize, 4, 3);
  float ratioX = transform.z / s.x;
  float ratioY = transform.w / s.y;
  if (ratioX > 1 && ratioY > 1) {
    if (ratioX > ratioY) {
      return minSize * ratioY;
    }
    else {
      return minSize * ratioX;
    }
  }
  return minSize; 
}

float determineTextRescaleFit(renderer2d *r2d, const char **str, font *f, vec4 transform) {
  float ret = 1.0f;
  vec2 s = getTextSize(r2d, *str, *f, ret, 4, 3);
  float ratioX = transform.z / s.x;
  float ratioY = transform.w / s.y;
  if (ratioX > 1 && ratioY > 1) {
    if (ratioX > ratioY) {
      return ret * ratioY;
    }
    else {
      return ret * ratioX;
    }
  }
  else {
    if (ratioX < ratioY) {
      return ret * ratioX;
    }
    else {
      return ret * ratioY;
    }
  }
  return ret;
}

int wrap(renderer2d *r2d, const char **in, font *f, float baseSize, float maxDimension, char **outRez) {
  char *word = "";
  char *currentLine = "";
  bool wrap = 0;
  bool newLine = 1;
  int newLineCounter = 0;
  if (outRez) {
    *outRez = "";
    *outRez = (char *)malloc(strlen(*in) + 10);
  }
  currentLine = (char *)malloc(strlen(*in) + 10);
  for (int i = 0; i < strlen(*in); i++) {
    strcat(word, in[i]);
    strcat(currentLine, in[i]);
    if (*in[i] == ' ') {
      if (wrap) {
        if (outRez) {
          strcat(*outRez, "\n");
        }
        newLineCounter++;
      }
      currentLine = "";
      if (outRez) {
        strcat(*outRez, word);
      }
      word = "";
      wrap = 0;
      newLine = true;
    }
    else {
      if (!wrap && !newLine) {
        float size = baseSize;
        vec2 textSize = getTextSize(r2d, currentLine, *f, size, 4, 3);
        if (textSize.x >= maxDimension && !newLine) {
          wrap = 1;
        }
      }
    }
  }
  if (wrap) {
    if (outRez) {
      strcat(*outRez, "\n");
      newLineCounter++;
    }
  }
  if (outRez) {
    strcat(*outRez, word);
  }
  return newLineCounter + 1;
}

void renderTextWrapped(renderer2d *r2d, const char *text, font f, vec4 textPos, vec4 color, float baseSize, float spacing, float lineSpacing, short showInCenter, vec4 shadowColor, vec4 lightColor) {
  char *newText;
  vec2 textPosition = { textPos.x, textPos.y };
  wrap(r2d, &text, &f, baseSize, textPos.z, &newText);
  renderText(r2d, textPosition, newText, f, color, baseSize, spacing, lineSpacing, showInCenter, shadowColor, lightColor);
}

vec2 getTextSizeWrapped(renderer2d *r2d, const char *text, font f, float maxTextLength, float baseSize, float spacing, float lineSpacing) {
  char *newText;
  wrap(r2d, &text, &f, baseSize, maxTextLength, &newText);
  vec2 rez = getTextSize(r2d, newText, f, baseSize, spacing, lineSpacing);
  return rez;
}

void renderRectangleTexture(renderer2d *r2d, const vec4 transform, const texture t, const vec4 colors[4], const vec2 origin, const float rotationDegrees, const vec4 textureCoords) {
  vec2 newOrigin = { origin.x + transform.x + (transform.z / 2), origin.y + transform.y + (transform.w / 2) };
  renderRectangleTextureAbsRotation(r2d, transform, t, colors, newOrigin, rotationDegrees, textureCoords);
}

void renderRectangleTextureAbsRotation(renderer2d *r2d, const vec4 transform, const texture t, const vec4 colors[4], const vec2 origin, const float rotationDegrees, const vec4 textureCoords) {
  texture textureCopy = t;
  if (textureCopy.id == 0) {
    errorFunc("Invalid texture", userDefinedData);
    textureCopy = white1pxSquareTexture;
  }
  const float transformY = transform.y * -1.0f;
  vec2 v1 = { transform.x, transformY };
  vec2 v2 = { transform.x, transformY - transform.w };
  vec2 v3 = { transform.x + transform.z, transformY - transform.w };
  vec2 v4 = { transform.x + transform.z, transformY };
  if (rotationDegrees != 0) {
    v1 = rotateAroundPoint(v1, origin, rotationDegrees);
    v2 = rotateAroundPoint(v2, origin, rotationDegrees);
    v3 = rotateAroundPoint(v3, origin, rotationDegrees);
    v4 = rotateAroundPoint(v4, origin, rotationDegrees);
  }
  v1.x -= r2d->currentCamera.position.x;
  v1.y += r2d->currentCamera.position.y;
  v2.x -= r2d->currentCamera.position.x;
  v2.y += r2d->currentCamera.position.y;
  v3.x -= r2d->currentCamera.position.x;
  v3.y += r2d->currentCamera.position.y;
  v4.x -= r2d->currentCamera.position.x;
  v4.y += r2d->currentCamera.position.y;
  if (r2d->currentCamera.rotation != 0) {
    vec2 cameraCenter;
    cameraCenter.x = r2d->windowW / 2.0f;
    cameraCenter.y = r2d->windowH / 2.0f;
    v1 = rotateAroundPoint(v1, cameraCenter, r2d->currentCamera.rotation);
    v2 = rotateAroundPoint(v2, cameraCenter, r2d->currentCamera.rotation);
    v3 = rotateAroundPoint(v3, cameraCenter, r2d->currentCamera.rotation);
    v4 = rotateAroundPoint(v4, cameraCenter, r2d->currentCamera.rotation);
  }
  vec2 cameraCenter = { r2d->windowW / 2.0f, -r2d->windowH / 2.0f };
  v1 = scaleAroundPoint(v1, cameraCenter, r2d->currentCamera.zoom);
  v2 = scaleAroundPoint(v2, cameraCenter, r2d->currentCamera.zoom);
  v3 = scaleAroundPoint(v3, cameraCenter, r2d->currentCamera.zoom);
  v4 = scaleAroundPoint(v4, cameraCenter, r2d->currentCamera.zoom);
  v1.x = positionToScreenCoordsX(v1.x, (float)r2d->windowW);
  v2.x = positionToScreenCoordsX(v2.x, (float)r2d->windowW);
  v3.x = positionToScreenCoordsX(v3.x, (float)r2d->windowW);
  v4.x = positionToScreenCoordsX(v4.x, (float)r2d->windowW);
  v1.y = positionToScreenCoordsY(v1.y, (float)r2d->windowH);
  v2.y = positionToScreenCoordsY(v2.y, (float)r2d->windowH);
  v3.y = positionToScreenCoordsY(v3.y, (float)r2d->windowH);
  v4.y = positionToScreenCoordsY(v4.y, (float)r2d->windowH);
  cvector_push_back(r2d->spritePositions, v1);
  cvector_push_back(r2d->spritePositions, v2);
  cvector_push_back(r2d->spritePositions, v4);
  cvector_push_back(r2d->spritePositions, v2);
  cvector_push_back(r2d->spritePositions, v3);
  cvector_push_back(r2d->spritePositions, v4);
  cvector_push_back(r2d->spriteColors, colors[0]);
  cvector_push_back(r2d->spriteColors, colors[1]);
  cvector_push_back(r2d->spriteColors, colors[3]);
  cvector_push_back(r2d->spriteColors, colors[1]);
  cvector_push_back(r2d->spriteColors, colors[2]);
  cvector_push_back(r2d->spriteColors, colors[3]);
  vec2 tPos = { textureCoords.x, textureCoords.y };
  cvector_push_back(r2d->texturePositions, tPos);
  tPos = (vec2){ textureCoords.x, textureCoords.w };
  cvector_push_back(r2d->texturePositions, tPos);
  tPos = (vec2){ textureCoords.z, textureCoords.y };
  cvector_push_back(r2d->texturePositions, tPos);
  tPos = (vec2){ textureCoords.x, textureCoords.w };
  cvector_push_back(r2d->texturePositions, tPos);
  tPos = (vec2){ textureCoords.z, textureCoords.w };
  cvector_push_back(r2d->texturePositions, tPos);
  tPos = (vec2){ textureCoords.z, textureCoords.y };
  cvector_push_back(r2d->texturePositions, tPos);
  cvector_push_back(r2d->spriteTextures, textureCopy);
}

void renderRectangle(renderer2d *r2d, const vec4 transform, const vec4 colors[4], const vec2 origin, const float rotationDegrees) {
  renderRectangleTexture(r2d, transform, white1pxSquareTexture, colors, origin, rotationDegrees, defaultTextureCoords);
}

void renderRectangleAbsRotation(renderer2d *r2d, const vec4 transform, const vec4 colors[4], const vec2 origin, const float rotationDegrees) {
  renderRectangleTextureAbsRotation(r2d, transform, white1pxSquareTexture, colors, origin, rotationDegrees, defaultTextureCoords);
}

void renderLineAngle(renderer2d *r2d, vec2 position, const float angleDegrees, const float length, const vec4 color, const float width) {
  vec4 colors[4] = { color, color, color, color };
  vec2 halfWidth = { 0.0f, width / 2.0f };
  vec2 difference = vec2Subtract(&position, &halfWidth);
  renderRectangle(r2d, (vec4){ difference.x, difference.y, length, width }, colors, (vec2){ -length / 2.0f, 0.0f }, angleDegrees);
}

void renderLineStartEnd(renderer2d *r2d, vec2 start, vec2 end, const vec4 color, const float width) {
  vec2 distance = vec2Subtract(&end, &start);
  float length = sqrt(pow(distance.x, 2) + pow(distance.y, 2));
  float angle = atan2(distance.y, distance.x);
  renderLineAngle(r2d, start, -degrees(angle), length, color, width);
}

void renderRectangleOutline(renderer2d *r2d, const vec4 position, const vec4 color, const float width, const vec2 origin, const float rotationDegrees) {
  vec2 topLeft = { position.x, position.y };
  vec2 topRight = { position.x + position.z, position.y };
  vec2 bottomRight = { position.x, position.y + position.w };
  vec2 bottomLeft = { position.x + position.z, position.y + position.w };
  vec2 p1 = { topLeft.x - (width / 2.0f), topLeft.y };
  vec2 p2 = { topRight.x + (width / 2.0f), topRight.y };
  vec2 p3 = { topRight.x, topRight.y + (width / 2.0f) };
  vec2 p4 = { bottomRight.x, bottomRight.y - (width / 2.0f) };
  vec2 p5 = { bottomRight.x + (width / 2.0f), bottomRight.y };
  vec2 p6 = { bottomLeft.x - (width / 2.0f), bottomLeft.y };
  vec2 p7 = { bottomLeft.x, bottomLeft.y - (width / 2.0f) };
  vec2 p8 = { topLeft.x, topLeft.y + (width / 2.0f) };
  if (rotationDegrees != 0) {
    vec2 o = { origin.x + position.x + position.z / 2.0f, origin.y - position.y - position.w / 2.0f };
    p1 = rotateAroundPoint(p1, o, -rotationDegrees);
    p2 = rotateAroundPoint(p2, o, -rotationDegrees);
    p3 = rotateAroundPoint(p3, o, -rotationDegrees);
    p4 = rotateAroundPoint(p4, o, -rotationDegrees);
    p5 = rotateAroundPoint(p5, o, -rotationDegrees);
    p6 = rotateAroundPoint(p6, o, -rotationDegrees);
    p7 = rotateAroundPoint(p7, o, -rotationDegrees);
    p8 = rotateAroundPoint(p8, o, -rotationDegrees);
  }
  renderPoint(r2d, p1);
  renderPoint(r2d, p2);
  renderPoint(r2d, p3);
  renderPoint(r2d, p4);
  renderPoint(r2d, p5);
  renderPoint(r2d, p6);
  renderPoint(r2d, p7);
  renderPoint(r2d, p8);
  renderLineStartEnd(r2d, p1, p2, color, width);
  renderLineStartEnd(r2d, p3, p4, color, width);
  renderLineStartEnd(r2d, p5, p6, color, width);
  renderLineStartEnd(r2d, p7, p8, color, width);
}

void renderCircleOutline(renderer2d *r2d, const vec2 position, const vec4 color, const float size, const float width, const unsigned int segments) {
  vec2 lastPos = calcPos(1, size, segments, position);
  renderLineStartEnd(r2d, calcPos(0, size, segments, position), lastPos, color, width);
  for (int i = 1; i < segments; i++) {
    vec2 pos1 = lastPos;
    vec2 pos2 = calcPos(i + 1, size, segments, position);
    renderLineStartEnd(r2d, pos1, pos2, color, width);
    lastPos = pos2;
  }
}

void renderNinePatch(renderer2d *r2d, const vec4 position, const vec4 color, const vec2 origin, const float rotationDegrees, const texture texture, const vec4 textureCoords, const vec4 inner_texture_coords) {
  vec4 colorData[4] = { color, color, color, color };
  int w = 0;
  int h = 0;
  float textureSpaceW = textureCoords.z - textureCoords.x;
  float textureSpaceH = textureCoords.y - textureCoords.w;
  float topBorder = (textureCoords.y - inner_texture_coords.y) / textureSpaceH * position.w;
  float bottomBorder = (inner_texture_coords.w - textureCoords.w) / textureSpaceH * position.w;
  float leftBorder = (inner_texture_coords.x - textureCoords.x) / textureSpaceW * position.z;
  float rightBorder = (textureCoords.z - inner_texture_coords.z) / textureSpaceW * position.z;
  float newAspectRatio = position.z / position.w;
  vec2 pos2d = { 0.0f, 0.0f };
  vec4 innerPos = position, topPos = position;
  vec4 bottom = position, left = position, right = position;
  vec4 topleft = position, topright = position, bottomleft = position, bottomright = position;
  glBindTexture(GL_TEXTURE_2D, texture.id);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
  if (newAspectRatio < 1.f) {
    topBorder *= newAspectRatio;
    bottomBorder *= newAspectRatio;
  }
  else {
    leftBorder /= newAspectRatio;
  }
  innerPos.x += leftBorder;
  innerPos.y += topBorder;
  innerPos.z -= leftBorder + rightBorder;
  innerPos.w -= topBorder + bottomBorder;
  renderRectangleTexture(r2d, innerPos, texture, colorData, pos2d, 0, inner_texture_coords);
  topPos.x += leftBorder;
  topPos.z -= leftBorder + rightBorder;
  topPos.w = topBorder;
  vec4 upperTexPos = { inner_texture_coords.x, textureCoords.y, inner_texture_coords.z, inner_texture_coords.y };
  renderRectangleTexture(r2d, topPos, texture, colorData, pos2d, 0, upperTexPos);
  bottom.x += leftBorder;
  bottom.y += (float)position.w - bottomBorder;
  bottom.z -= leftBorder + rightBorder;
  bottom.w = bottomBorder;
  vec4 bottomTexPos = { inner_texture_coords.x, inner_texture_coords.w, inner_texture_coords.z, textureCoords.w };
  renderRectangleTexture(r2d, bottom, texture, colorData, pos2d, 0, bottomTexPos);
  left.y += topBorder;
  left.z = leftBorder;
  left.w -= topBorder + bottomBorder;
  vec4 leftTexPos = { textureCoords.x, inner_texture_coords.y, inner_texture_coords.x, inner_texture_coords.w };
  renderRectangleTexture(r2d, left, texture, colorData, pos2d, 0, leftTexPos);
  right.x += position.z - rightBorder;
  right.y += topBorder;
  right.z = rightBorder;
  right.w -= topBorder + bottomBorder;
  vec4 rightTexPos = { inner_texture_coords.z, inner_texture_coords.y, textureCoords.z, inner_texture_coords.w };
  renderRectangleTexture(r2d, right, texture, colorData, pos2d, 0, rightTexPos);
  topleft.z = leftBorder;
  topleft.w = topBorder;
  vec4 topleftTexPos = { textureCoords.x, textureCoords.y, inner_texture_coords.x, inner_texture_coords.y };
  renderRectangleTexture(r2d, topleft, texture, colorData, pos2d, 0, topleftTexPos);
  topright.x += position.z - rightBorder;
  topright.z = rightBorder;
  topright.w = topBorder;
  vec4 toprightTexPos = { inner_texture_coords.z, textureCoords.y, textureCoords.z, inner_texture_coords.y };
  renderRectangleTexture(r2d, topright, texture, colorData, pos2d, 0, toprightTexPos);
  bottomleft.y += position.w - bottomBorder;
  bottomleft.z = leftBorder;
  bottomleft.w = bottomBorder;
  vec4 bottomleftTexPos = { textureCoords.x, inner_texture_coords.w, inner_texture_coords.x, textureCoords.w };
  renderRectangleTexture(r2d, bottomleft, texture, colorData, pos2d, 0, bottomleftTexPos);
  bottomright.y += position.w - bottomBorder;
  bottomright.x += position.z - rightBorder;
  bottomright.z = rightBorder;
  bottomright.w = bottomBorder;
  vec4 bottomrightTexPos = { inner_texture_coords.z, inner_texture_coords.w, textureCoords.z, textureCoords.w };
  renderRectangleTexture(r2d, bottomright, texture, colorData, pos2d, 0, bottomrightTexPos);
}

void clearScreen(renderer2d *r2d, const vec4 color) {
  glBindFramebuffer(GL_FRAMEBUFFER, r2d->defaultFbo);
  #if KHG2D_USE_OPENGL_130
    GLfloat oldColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, oldColor);
    glClearColor(color.x, color.y, color.z, color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(oldColor[0], oldColor[1], oldColor[2], oldColor[3]);
  #else
    glClearBufferfv(GL_COLOR, 0, &color.x);
  #endif
}

void setShader(renderer2d *r2d, const shader s) {
  r2d->currentShader = s;
}

void setCamera(renderer2d *r2d, const camera c) {
  r2d->currentCamera = c;
}

void resetShaderAndCamera(renderer2d *r2d) {
  r2d->currentCamera = defaultCamera;
  r2d->currentShader = defaultShader;
}

void renderPostProcess(renderer2d *r2d, shader shader, texture input, framebuffer result) {
  glBindFramebuffer(GL_FRAMEBUFFER, result.fbo);
  enableGLNecessaryFeatures();
  if (!hasInitialized) {
    errorFunc("Library not initialized. Have you forgotten to call gl2d::init() ?", userDefinedData);
    return;
  }
  if (!r2d->vao) {
    errorFunc("Renderer not initialized. Have you forgotten to call gl2d::Renderer2D::create() ?", userDefinedData);
    return;
  }
  if (!shader.id) {
    errorFunc("Post Process Shader not created.", userDefinedData);
    return;
  }
  vec2 size = getTextureSize(&input);
  if (size.x == 0 || size.y == 0) {
    return;
  }
  glViewport(0, 0, size.x, size.y);
  glUseProgram(shader.id);
  glUniform1i(shader.u_sampler, 0);
  bindTexture(&input, 0);
  renderQuadToScreenInternal(r2d);
  glBindVertexArray(0);
}

void flush(renderer2d *r2d, bool shouldClear) {
  glBindFramebuffer(GL_FRAMEBUFFER, r2d->defaultFbo);
  internalFlush(r2d, shouldClear);
}

void flushFbo(renderer2d *r2d, framebuffer frameBuffer, short shouldClear) {
  if (frameBuffer.fbo == 0) {
    errorFunc("Framebuffer not initialized", userDefinedData);
    return;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.fbo);
  glBindTexture(GL_TEXTURE_2D, 0);
  internalFlush(r2d, shouldClear);
  glBindFramebuffer(GL_FRAMEBUFFER, r2d->defaultFbo);
}

void renderFrameBufferToEntireScreen(renderer2d *r2d, framebuffer fbo, framebuffer screen) {
  renderTextureToEntireScreen(r2d, fbo.texture, screen);
}

void renderTextureToEntireScreen(renderer2d *r2d, texture t, framebuffer screen) {
  vec2 size = { r2d->windowW, r2d->windowH };
  glBindFramebuffer(GL_FRAMEBUFFER, screen.fbo);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  if (!hasInitialized) {
    errorFunc("Library not initialized. Have you forgotten to call gl2d::init() ?", userDefinedData);
    return;
  }
  if (!r2d->vao) {
    errorFunc("Renderer not initialized. Have you forgotten to call gl2d::Renderer2D::create() ?", userDefinedData);
    return;
  }
  if (!r2d->currentShader.id) {
    errorFunc("Post Process Shader not created.", userDefinedData);
    return;
  }
  if (screen.fbo) {
    size = getTextureSize(&screen.texture);
  }
  if (size.x == 0 || size.y == 0) {
    return;
  }
  glViewport(0, 0, size.x, size.y);
  glUseProgram(r2d->currentShader.id);
  glUniform1i(r2d->currentShader.u_sampler, 0);
  bindTexture(&t, 0);
  renderQuadToScreenInternal(r2d);
  glBindVertexArray(0);
}

void flushPostProcess(renderer2d *r2d, const cvector(shader) *postProcess, framebuffer frameBuffer, bool shouldClear) {
  if (cvector_empty(*postProcess)) {
    if (shouldClear) {
      clearDrawData(r2d);
      return;
    }
  }
  if (!r2d->postProcessFbo1.fbo) {
    createFramebuffer(&r2d->postProcessFbo1, 0, 0);
  }
  resizeFramebuffer(&r2d->postProcessFbo1, r2d->windowW, r2d->windowH);
  clearFramebuffer(&r2d->postProcessFbo1);
  flushFbo(r2d, r2d->postProcessFbo1, shouldClear);
  r2d->internalPostProcessFlip = 1;
  postProcessOverTexture(r2d, postProcess, r2d->postProcessFbo1.texture, frameBuffer);
}

void postProcessOverTexture(renderer2d *r2d, const cvector(shader) *postProcess, texture in, framebuffer fb) {
  int i;
  if (cvector_empty(*postProcess)) {
    return;
  }
  if (!r2d->postProcessFbo1.fbo) { 
    createFramebuffer(&r2d->postProcessFbo1, 0, 0);
  }
  if (!r2d->postProcessFbo2.fbo && cvector_size(*postProcess) > 1) {
    createFramebuffer(&r2d->postProcessFbo2, 0, 0);
  }
  if (r2d->internalPostProcessFlip == 0) {
    resizeFramebuffer(&r2d->postProcessFbo1, r2d->windowW, r2d->windowH);
    clearFramebuffer(&r2d->postProcessFbo1);
    resizeFramebuffer(&r2d->postProcessFbo2, r2d->windowW, r2d->windowH);
    clearFramebuffer(&r2d->postProcessFbo2); 
  }
  else if(r2d->postProcessFbo2.fbo) {
    resizeFramebuffer(&r2d->postProcessFbo2, r2d->windowW, r2d->windowH);
    clearFramebuffer(&r2d->postProcessFbo2);
  }
  for (i = 0; i < cvector_size(*postProcess); i++) {
    framebuffer output;
    texture input;
    if (r2d->internalPostProcessFlip == 0) {
      input = r2d->postProcessFbo2.texture;
      output = r2d->postProcessFbo1;
    }
    else {
      input = r2d->postProcessFbo1.texture;
      output = r2d->postProcessFbo2;
    }
    if (i == 0) {
      input = in;
    }
    if (i == cvector_size(*postProcess) - 1) {
      output = fb;
    }
    clearFramebuffer(&output);
    renderPostProcess(r2d, *postProcess[i], input, output);
    r2d->internalPostProcessFlip = !r2d->internalPostProcessFlip;
  }
  r2d->internalPostProcessFlip = 0;
}

void enableGLNecessaryFeatures(void) {
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}
