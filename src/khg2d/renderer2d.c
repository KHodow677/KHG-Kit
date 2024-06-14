#include "renderer2d.h"
#include <math.h>
#include "framebuffer.h"
#include "texture.h"
#include "utils.h"
#include "../math/math.h"
#include "../utils/vec.h"

void internalFlush(renderer2d *r2d, bool shouldClear) {
  const int size = vectorSize(r2d->spriteTextures);
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
  if (vectorSize(r2d->spriteTextures) == 0) {
    return; 
  }
  glViewport(0, 0, r2d->windowW, r2d->windowH);
  glBindVertexArray(r2d->vao);
  glUseProgram(r2d->currentShader.id);
  glUniform1i(r2d->currentShader.u_sampler, 0);
  glBindBuffer(GL_ARRAY_BUFFER, r2d->buffers[quadPositions]);
  glBufferData(GL_ARRAY_BUFFER, vectorSize(r2d->spritePositions) * sizeof(vec2), r2d->spritePositions, GL_STREAM_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, r2d->buffers[quadColors]);
  glBufferData(GL_ARRAY_BUFFER, vectorSize(r2d->spriteColors) * sizeof(vec4), r2d->spriteColors, GL_STREAM_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, r2d->buffers[texturePositions]);
  glBufferData(GL_ARRAY_BUFFER, vectorSize(r2d->texturePositions) * sizeof(vec2), r2d->texturePositions, GL_STREAM_DRAW);
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
  vectorReserve(&r2d->spritePositions, vec2, quadCount * 6);
  vectorReserve(&r2d->spriteColors, vec4, quadCount * 6);
  vectorReserve(&r2d->texturePositions, vec2, quadCount * 6);
  vectorReserve(&r2d->spriteTextures, texture, quadCount);
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

void clearDrawData(renderer2d *r2d) {
  vectorClear(r2d->spritePositions);
  vectorClear(r2d->spriteColors);
  vectorClear(r2d->texturePositions);
  vectorClear(r2d->spriteTextures);
}

void resetShaderAndCamera(renderer2d *r2d) {

}

void renderPostProcess(renderer2d *r2d, shader shader, texture input, framebuffer result) {

}

void flush(renderer2d *r2d, bool clearDrawData) {
  glBindFramebuffer(GL_FRAMEBUFFER, r2d->defaultFbo);
  internalFlush(r2d, clearDrawData);
}

void flushFbo(renderer2d *r2d, framebuffer frameBuffer, short clearDrawData) {
  if (frameBuffer.fbo == 0) {
    errorFunc("Framebuffer not initialized", userDefinedData);
    return;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.fbo);
  glBindTexture(GL_TEXTURE_2D, 0);
  internalFlush(r2d, clearDrawData);
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

void flushPostProcess(renderer2d *r2d, const shader *postProcess, framebuffer frameBuffer, bool shouldClear) {
  if (vectorSize(&postProcess) == 0) {
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

void postProcessOverTexture(renderer2d *r2d, const shader *postProcess, texture in, framebuffer fb) {
  int i;
  if (vectorSize(&postProcess)) {
    return;
  }
  if (!r2d->postProcessFbo1.fbo) { 
    createFramebuffer(&r2d->postProcessFbo1, 0, 0);
  }
  if (!r2d->postProcessFbo2.fbo && vectorSize(&postProcess) > 1) {
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
  for (i = 0; i < vectorSize(&postProcess); i++) {
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
    if (i == vectorSize(&postProcess) - 1) {
      output = fb;
    }
    clearFramebuffer(&output);
    renderPostProcess(r2d, postProcess[i], input, output);
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
  vectorAdd(&r2d->spritePositions, vec2, v1);
  vectorAdd(&r2d->spritePositions, vec2, v2);
  vectorAdd(&r2d->spritePositions, vec2, v4);
  vectorAdd(&r2d->spritePositions, vec2, v2);
  vectorAdd(&r2d->spritePositions, vec2, v3);
  vectorAdd(&r2d->spritePositions, vec2, v4);
  vectorAdd(&r2d->spriteColors, vec4, colors[0]);
  vectorAdd(&r2d->spriteColors, vec4, colors[1]);
  vectorAdd(&r2d->spriteColors, vec4, colors[3]);
  vectorAdd(&r2d->spriteColors, vec4, colors[1]);
  vectorAdd(&r2d->spriteColors, vec4, colors[2]);
  vectorAdd(&r2d->spriteColors, vec4, colors[3]);
  tPos.x = textureCoords.x;
  tPos.y = textureCoords.y;
  vectorAdd(&r2d->texturePositions, vec2, tPos);
  tPos.x = textureCoords.x; 
  tPos.y = textureCoords.w;
  vectorAdd(&r2d->texturePositions, vec2, tPos);
  tPos.x = textureCoords.z;
  tPos.y = textureCoords.y;
  vectorAdd(&r2d->texturePositions, vec2, tPos);
  tPos.x = textureCoords.x; 
  tPos.y = textureCoords.w;
  vectorAdd(&r2d->texturePositions, vec2, tPos);
  tPos.x = textureCoords.z;
  tPos.y = textureCoords.w;
  vectorAdd(&r2d->texturePositions, vec2, tPos);
  tPos.x = textureCoords.z; 
  tPos.y = textureCoords.y;
  vectorAdd(&r2d->texturePositions, vec2, tPos);
  vectorAdd(&r2d->spriteTextures, texture, textureCopy);
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
