#include "renderer2d.h"
#include <math.h>
#include <string.h>
#include "utils.h"
#include "../math/minmax.h"
#include "../math/math.h"
#include "../math/mat3.h"

void internalFlush(renderer2d *r2d, bool shouldClear) {
  const int size = cvector_size(r2d->spriteTextures);
  int pos = 0, i;
  unsigned int id = r2d->spriteTextures[0].id;
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
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
  for (i = 1; i < size; i++) {
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
  vec2 offset = { 1.0f, 1.0f };
  vec4 rect, colors[4];
  vec2 origin = { 0.0f, 0.0f };
  rect.x = pos.x = offset.x;
  rect.y = pos.y - offset.y;
  rect.z = 2.0f;
  rect.w = 2.0f;
  colors[0] = colorBlack;
  colors[1] = colorBlack;
  colors[2] = colorBlack;
  colors[3] = colorBlack;
  renderRectangle(r2d, rect, colors, origin, 0);
}

vec2 calcPos(int p, float size, int segments, vec2 position) {
  vec2 circle;
  float a = 3.1415926 * 2 * ((float)p / segments);
  float c = cos(a);
  float s = sin(a);
  circle.x = size;
  circle.y = 0.0f;
  circle.x = c * circle.x - s * circle.y;
  circle.y = s * circle.x - c * circle.y;
  return vec2Add(&circle, &position);
}

void createRenderer2d(renderer2d *r2d, GLuint fbo, size_t quadCount) {
  if (!hasInitialized) {
		errorFunc("Library not initialized. Have you forgotten to call gl2d::init() ?", userDefinedData);
  }
  r2d->defaultFbo = fbo;
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
  vec4 rect;
  mat3 mat;
  vec3 pos1, pos2;
  vec2 point, scalePoint1, scalePoint2, pos2d1, pos2d2;
  rect.x = 0;
  rect.y = 0;
  rect.z = r2d->windowW;
  rect.w = r2d->windowH;
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
  pos1.x = rect.x;
  pos1.y = rect.y;
  pos1.z = 1.0f;
  pos2.x = rect.z + rect.x;
  pos2.y = rect.w + rect.y;
  pos2.z = 1.0f;
  pos1 = mat3TransformVec3(&mat, &pos1);
  pos2 = mat3TransformVec3(&mat, &pos2);
  point.x = (pos1.x + pos2.y) / 2.0f;
  point.y = (pos1.y + pos2.y) / 2.0f;
  pos2d1.x = pos1.x;
  pos2d1.y = pos1.y;
  pos2d2.x = pos2.x;
  pos2d2.y = pos2.y;
  scalePoint1 = scaleAroundPoint(pos2d1, point, 1.0f / r2d->currentCamera.zoom);
  scalePoint2 = scaleAroundPoint(pos2d2, point, 1.0f / r2d->currentCamera.zoom);
  pos1.x = scalePoint1.x;
  pos1.y = scalePoint1.y;
  pos1.z = 1.0f;
  pos2.x = scalePoint2.x;
  pos2.x = scalePoint2.y;
  pos2.z = 1.0f;
  rect.x = pos1.x;
  rect.y = pos1.y;
  rect.z = pos2.x - pos1.x;
  rect.w = pos2.y - pos1.y;
  return rect;
}

void updateWindowMetrics(renderer2d *r2d, int w, int h) {
  r2d->windowW = w;
  r2d->windowH = h;
}

vec4 pixToScreen(renderer2d *r2d, const vec4 *transform) {
  const float transformY = transform->y * -1.0f;
  vec2 v1, v2, v3, v4, cameraCenter;
  vec4 result;
  v1.x = transform->x;
  v1.y = transformY;
  v2.x = transform->x;
  v2.y = transformY - transform->w;
  v3.x = transform->x + transform->z;
  v3.y = transformY - transform->w;
  v4.x = transform->x + transform->z;
  v4.y = transformY;
  v1.x -= r2d->currentCamera.position.x;
  v1.y += r2d->currentCamera.position.y;
  v2.x -= r2d->currentCamera.position.x;
  v2.y += r2d->currentCamera.position.y;
  v3.x -= r2d->currentCamera.position.x;
  v3.y += r2d->currentCamera.position.y;
  v4.x -= r2d->currentCamera.position.x;
  v4.y += r2d->currentCamera.position.y;
  cameraCenter.x = r2d->windowW / 2.0f;
  cameraCenter.y = r2d->windowH / 2.0f;
  v1 = scaleAroundPoint(v1, cameraCenter, r2d->currentCamera.zoom);
  v3 = scaleAroundPoint(v3, cameraCenter, r2d->currentCamera.zoom);
  v1.x = positionToScreenCoordsX(v1.x, r2d->windowW);
  v3.x = positionToScreenCoordsX(v3.x, r2d->windowW);
  v1.y = positionToScreenCoordsY(v1.y, r2d->windowH);
  v3.y = positionToScreenCoordsY(v3.y, r2d->windowH);
  result.x = v1.x;
  result.y = v1.y;
  result.z = v3.x;
  result.w = v3.y;
  return result;
}

void clearDrawData(renderer2d *r2d) {
  cvector_clear(r2d->spritePositions);
  cvector_clear(r2d->spriteColors);
  cvector_clear(r2d->texturePositions);
  cvector_clear(r2d->spriteTextures);
}

vec2 getTextSize(renderer2d *r2d, const char *text, const font font, const float size, const float spacing, const float line_space) {
  vec2 position, result;
  const int textLength = (int)strlen(text);
  vec4 rectangle;
  float linePositionY = position.y, maxPos = 0, maxPosY = 0, bonusY = 0, paddX, paddY;
  int i;
  if (font.texture.id == 0) {
    errorFunc("Missing font", userDefinedData);
    return position;
  }
  rectangle.x = position.x;
  for (i = 0; i< textLength; i++) {
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
  paddX = maxPos;
  paddY = maxPosY;
  paddY += font.maxHeight * size + bonusY;
  result.x = paddX;
  result.y = paddY;
  return result;
}

void renderText(renderer2d *r2d, vec2 position, const char *text, const font f, const vec4 color, const float size, const float spacing, const float line_space, short showInCenter, const vec4 shadowColor, const vec4 lightColor) {
  const int text_length = (int)strlen(text);
  vec4 rectangle;
  float linePositionY = position.y;
  int i;
  if (f.texture.id == 0) {
    errorFunc("Missing font", userDefinedData);
    return;
  }
  rectangle.x = position.x;
  if (showInCenter) {
    vec2 textSize = getTextSize(r2d, text, f, size, spacing, line_space);
    position.x -= textSize.x / 2.0f;
    position.y -= textSize.y / 2.0f;
  }
  if (0) {
    float maxPos = 0, maxPosY = 0, padd, paddY;
    for (i = 0; i < text_length; i++) {
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
        const stbtt_aligned_quad quad = fontGetGlyphQuad(f, ' ');
        float x = quad.x1 - quad.x0;
        rectangle.x += x * size + spacing * size;
      }
      else if (text[i] >= ' ' && text[i] <= '~') {
        const stbtt_aligned_quad quad = fontGetGlyphQuad(f, text[i]);
        rectangle.z = quad.x1 - quad.x0;
        rectangle.w = quad.y1 - quad.y0;
        rectangle.z *= size;
        rectangle.w *= size;
        rectangle.y = linePositionY + quad.y0 * size;
        rectangle.x += rectangle.z + spacing * size;
        maxPos = max(maxPos, rectangle.x);
        maxPosY = max(maxPosY, rectangle.y);
      }
    }
    padd = maxPos - position.x;
    padd /= 2;
    position.x -= padd;
    paddY = maxPosY - position.y;
    position.y -= paddY;
  }
  rectangle.x= 0.0f;
  rectangle.y = 0.0f;
  rectangle.z = 0.0f;
  rectangle.w = 0.0f;
  rectangle.x = position.x;
  linePositionY = position.y;
  for (i = 0; i < text_length; i++) {
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
      vec4 colorData[4], textureCoords;
      vec2 origin = { 0.0f, 0.0f };
      rectangle.z = quad.x1 - quad.x0;
      rectangle.w = quad.y1 - quad.y0;
      rectangle.z *= size;
      rectangle.y = linePositionY + quad.y0 * size;
      colorData[0] = color;
      colorData[1] = color;
      colorData[2] = color;
      colorData[3] = color;
      textureCoords.x = quad.s0;
      textureCoords.y = quad.t0;
      textureCoords.z = quad.s1;
      textureCoords.w = quad.t1;
      if (shadowColor.w) {
        vec2 pos = { -5.0f, 3.0f };
        vec4 transform, shadowColorData[4];
        pos = vec2MultiplyNumOnVec2(size, &pos);
        transform.x = rectangle.x + pos.x;
        transform.y = rectangle.y + pos.y;
        transform.z = rectangle.z;
        transform.w = rectangle.w;
        shadowColorData[0] = shadowColor;
        shadowColorData[1] = shadowColor;
        shadowColorData[2] = shadowColor;
        shadowColorData[3] = shadowColor;
        renderRectangleTexture(r2d, transform, f.texture, shadowColorData, origin, 0, textureCoords);
      }
      renderRectangleTexture(r2d, rectangle, f.texture, colorData, origin, 0, textureCoords);
      if (lightColor.w) {
        vec2 pos = { -2.0f, 1.0f };
        vec4 transform, lightColorData[4];
        pos = vec2MultiplyNumOnVec2(size, &pos);
        transform.x = rectangle.x + pos.x;
        transform.y = rectangle.y + pos.y;
        transform.z = rectangle.z;
        transform.w = rectangle.w;
        lightColorData[0] = lightColor;
        lightColorData[1] = lightColor;
        lightColorData[2] = lightColor;
        lightColorData[3] = lightColor;
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
  int i;
  if (outRez) {
    *outRez = "";
    *outRez = (char *)malloc(strlen(*in) + 10);
  }
  currentLine = (char *)malloc(strlen(*in) + 10);
  for (i = 0; i < strlen(*in); i++) {
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
  vec2 textPosition;
  textPosition.x = textPos.x;
  textPosition.y = textPos.y;
  wrap(r2d, &text, &f, baseSize, textPos.z, &newText);
  renderText(r2d, textPosition, newText, f, color, baseSize, spacing, lineSpacing, showInCenter, shadowColor, lightColor);
}

vec2 getTextSizeWrapped(renderer2d *r2d, const char *text, font f, float maxTextLength, float baseSize, float spacing, float lineSpacing) {
  char *newText;
  vec2 rez;
  wrap(r2d, &text, &f, baseSize, maxTextLength, &newText);
  rez = getTextSize(r2d, newText, f, baseSize, spacing, lineSpacing);
  return rez;
}

void renderRectangleTexture(renderer2d *r2d, const vec4 transform, const texture t, const vec4 colors[4], const vec2 origin, const float rotationDegrees, const vec4 textureCoords) {
  vec2 newOrigin;
  newOrigin.x = origin.x + transform.x + (transform.z / 2);
  newOrigin.y = origin.y + transform.y + (transform.w / 2);
  renderRectangleTextureAbsRotation(r2d, transform, t, colors, newOrigin, rotationDegrees, textureCoords);
}

void renderRectangleTextureAbsRotation(renderer2d *r2d, const vec4 transform, const texture t, const vec4 colors[4], const vec2 origin, const float rotationDegrees, const vec4 textureCoords) {
  texture textureCopy = t;
  const float transformY = transform.y * -1;
  vec2 v1, v2, v3, v4;
  vec2 cameraCenter;
  vec2 tPos;
  v1.x = transform.x;
  v1.y = transformY;
  v2.x = transform.x;
  v2.y = transformY - transform.w;
  v3.x = transform.x + transform.z;
  v3.y = transformY - transform.w;
  v4.x = transform.x + transform.z;
  v4.y = transformY;
  if (textureCopy.id == 0) {
    errorFunc("Invalid texture", userDefinedData);
    textureCopy = white1pxSquareTexture;
  }
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
  cameraCenter.x = r2d->windowW / 2.0f;
  cameraCenter.y = -r2d->windowH / 2.0f;
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
  tPos.x = textureCoords.x;
  tPos.y = textureCoords.y;
  cvector_push_back(r2d->texturePositions, tPos);
  tPos.x = textureCoords.x; 
  tPos.y = textureCoords.w;
  cvector_push_back(r2d->texturePositions, tPos);
  tPos.x = textureCoords.z;
  tPos.y = textureCoords.y;
  cvector_push_back(r2d->texturePositions, tPos);
  tPos.x = textureCoords.x; 
  tPos.y = textureCoords.w;
  cvector_push_back(r2d->texturePositions, tPos);
  tPos.x = textureCoords.z;
  tPos.y = textureCoords.w;
  cvector_push_back(r2d->texturePositions, tPos);
  tPos.x = textureCoords.z; 
  tPos.y = textureCoords.y;
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
  vec2 halfWidth, difference, origin;
  vec4 transform, colors[4];
  halfWidth.x = 0;
  halfWidth.y = width / 2.0f;
  difference = vec2Subtract(&position, &halfWidth);
  transform.x = difference.x;
  transform.y = difference.y;
  transform.z = length;
  transform.w = width;
  colors[0] = color;
  colors[1] = color;
  colors[2] = color;
  colors[3] = color;
  origin.x = -length / 2.0f;
  origin.y = 0;
  renderRectangle(r2d, transform, colors, origin, angleDegrees);
}

void renderLineStartEnd(renderer2d *r2d, vec2 start, vec2 end, const vec4 color, const float width) {
  vec2 distance = vec2Subtract(&end, &start);
  float length = sqrt(pow(distance.x, 2) + pow(distance.y, 2));
  float angle = atan2(distance.y, distance.x);
  renderLineAngle(r2d, start, -degrees(angle), length, color, width);
}

void renderRectangleOutline(renderer2d *r2d, const vec4 position, const vec4 color, const float width, const vec2 origin, const float rotationDegrees) {
  vec2 topLeft, topRight, bottomRight, bottomLeft;
  vec2 p1, p2, p3, p4, p5, p6, p7, p8;
  topLeft.x = position.x;
  topLeft.y = position.y;
  topRight.x = position.x + position.z;
  topRight.y = position.y;
  bottomRight.x = position.x;
  bottomRight.y = position.y + position.w;
  bottomLeft.x = position.x + position.z;
  bottomLeft.y = position.y + position.w;
  p1.x = topLeft.x - (width / 2.0f);
  p1.y = topLeft.y;
  p2.x = topRight.x + (width / 2.0f);
  p2.y = topRight.y;
  p3.x = topRight.x;
  p3.y = topRight.y + (width / 2.0f);
  p4.x = bottomRight.x;
  p4.y = bottomRight.y - (width / 2.0f);
  p5.x = bottomRight.x + (width / 2.0f);
  p5.y = bottomRight.y;
  p6.x = bottomLeft.x - (width / 2.0f);
  p6.y = bottomLeft.y;
  p7.x = bottomLeft.x;
  p7.y = bottomLeft.y - (width / 2.0f);
  p8.x = topLeft.x;
  p8.y = topLeft.y + (width / 2.0f);
  if (rotationDegrees != 0) {
    vec2 o;
    o.x = origin.x + position.x + position.z / 2.0f;
    o.y = origin.y - position.y - position.w / 2.0f;
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
  int i;
  renderLineStartEnd(r2d, calcPos(0, size, segments, position), lastPos, color, width);
  for (i = 1; i < segments; i++) {
    vec2 pos1 = lastPos;
    vec2 pos2 = calcPos(i + 1, size, segments, position);
    renderLineStartEnd(r2d, pos1, pos2, color, width);
    lastPos = pos2;
  }
}

void renderNinePatch(renderer2d *r2d, const vec4 position, const vec4 color, const vec2 origin, const float rotationDegrees, const texture texture, const vec4 textureCoords, const vec4 inner_texture_coords) {
  vec4 colorData[4];
  int w = 0;
  int h = 0;
  float textureSpaceW = textureCoords.z - textureCoords.x;
  float textureSpaceH = textureCoords.y - textureCoords.w;
  float topBorder = (textureCoords.y - inner_texture_coords.y) / textureSpaceH * position.w;
  float bottomBorder = (inner_texture_coords.w - textureCoords.w) / textureSpaceH * position.w;
  float leftBorder = (inner_texture_coords.x - textureCoords.x) / textureSpaceW * position.z;
  float rightBorder = (textureCoords.z - inner_texture_coords.z) / textureSpaceW * position.z;
  float newAspectRatio = position.z / position.w;
  vec2 pos2d = { 0, 0 };
  vec4 innerPos = position;
  vec4 topPos = position;
  vec4 bottom = position;
  vec4 left = position;
  vec4 right = position;
  vec4 topleft = position;
  vec4 topright = position;
  vec4 bottomleft = position;
  vec4 bottomright = position;
  vec4 upperTexPos;
  vec4 bottomTexPos;
  vec4 leftTexPos;
  vec4 rightTexPos;
  vec4 topleftTexPos;
  vec4 toprightTexPos;
  vec4 bottomleftTexPos;
  vec4 bottomrightTexPos;
  colorData[0] = color;
  colorData[1] = color;
  colorData[2] = color; 
  colorData[3] = color;
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
  upperTexPos.x = inner_texture_coords.x;
  upperTexPos.y = textureCoords.y;
  upperTexPos.z = inner_texture_coords.z;
  upperTexPos.w = inner_texture_coords.y;
  renderRectangleTexture(r2d, topPos, texture, colorData, pos2d, 0, upperTexPos);
  bottom.x += leftBorder;
  bottom.y += (float)position.w - bottomBorder;
  bottom.z -= leftBorder + rightBorder;
  bottom.w = bottomBorder;
  bottomTexPos.x = inner_texture_coords.x;
  bottomTexPos.y = inner_texture_coords.w;
  bottomTexPos.z = inner_texture_coords.z;
  bottomTexPos.w = textureCoords.w;
  renderRectangleTexture(r2d, bottom, texture, colorData, pos2d, 0, bottomTexPos);
  left.y += topBorder;
  left.z = leftBorder;
  left.w -= topBorder + bottomBorder;
  leftTexPos.x = textureCoords.x;
  leftTexPos.y = inner_texture_coords.y;
  leftTexPos.z = inner_texture_coords.x;
  leftTexPos.w = inner_texture_coords.w;
  renderRectangleTexture(r2d, left, texture, colorData, pos2d, 0, leftTexPos);
  right.x += position.z - rightBorder;
  right.y += topBorder;
  right.z = rightBorder;
  right.w -= topBorder + bottomBorder;
  rightTexPos.x = inner_texture_coords.z;
  rightTexPos.y = inner_texture_coords.y;
  rightTexPos.z = textureCoords.z;
  rightTexPos.w = inner_texture_coords.w;
  renderRectangleTexture(r2d, right, texture, colorData, pos2d, 0, rightTexPos);
  topleft.z = leftBorder;
  topleft.w = topBorder;
  topleftTexPos.x = textureCoords.x;
  topleftTexPos.y = textureCoords.y;
  topleftTexPos.z = inner_texture_coords.x;
  topleftTexPos.w = inner_texture_coords.y;
  renderRectangleTexture(r2d, topleft, texture, colorData, pos2d, 0, topleftTexPos);
  topright.x += position.z - rightBorder;
  topright.z = rightBorder;
  topright.w = topBorder;
  toprightTexPos.x = inner_texture_coords.z;
  toprightTexPos.y = textureCoords.y;
  toprightTexPos.z = textureCoords.z;
  toprightTexPos.w = inner_texture_coords.y;
  renderRectangleTexture(r2d, topright, texture, colorData, pos2d, 0, toprightTexPos);
  bottomleft.y += position.w - bottomBorder;
  bottomleft.z = leftBorder;
  bottomleft.w = bottomBorder;
  bottomleftTexPos.x = textureCoords.x;
  bottomleftTexPos.y = inner_texture_coords.w;
  bottomleftTexPos.z = inner_texture_coords.x;
  bottomleftTexPos.w = textureCoords.w;
  renderRectangleTexture(r2d, bottomleft, texture, colorData, pos2d, 0, bottomleftTexPos);
  bottomright.y += position.w - bottomBorder;
  bottomright.x += position.z - rightBorder;
  bottomright.z = rightBorder;
  bottomright.w = bottomBorder;
  bottomrightTexPos.x = inner_texture_coords.z;
  bottomrightTexPos.y = inner_texture_coords.w;
  bottomrightTexPos.z = textureCoords.z;
  bottomrightTexPos.w = textureCoords.w;
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

void setshader(renderer2d *r2d, const shader s) {
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
  vec2 size;
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
  size = getTextureSize(&input);
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

void flush(renderer2d *r2d, bool clearDrawData) {
  glBindFramebuffer(GL_FRAMEBUFFER, r2d->defaultFbo);
  internalFlush(r2d, clearDrawData);
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
  vec2 size;
  size.x = r2d->windowW;
  size.y = r2d->windowH;
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
