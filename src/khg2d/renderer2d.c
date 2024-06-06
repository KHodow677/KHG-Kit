#include "renderer2d.h"
#include "framebuffer.h"
#include "texture.h"
#include "utils.h"
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

void clearDrawData(renderer2d *r2d) {
  vectorClear(r2d->spritePositions);
  vectorClear(r2d->spriteColors);
  vectorClear(r2d->texturePositions);
  vectorClear(r2d->spriteTextures);
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

void renderRectangleTexture(renderer2d *r2d, const vec4 transforms, const texture t, const vec4 colors[4], const vec2 origin, const float rotationDegrees, const vec4 textureCoords) {
  vec2 newOrigin;
  newOrigin.x = origin.x + transforms.x + (transforms.z / 2);
  newOrigin.y = origin.y + transforms.y + (transforms.w / 2);
  renderRectangleTextureAbsRotation(r2d, transforms, t, colors, newOrigin, rotationDegrees, textureCoords);
}

void renderRectangleTextureAbsRotation(renderer2d *r2d, const vec4 transforms, const texture t, const vec4 colors[4], const vec2 origin, const float rotationDegrees, const vec4 textureCoords) {
  texture textureCopy = t;
  const float transformsY = transforms.y * -1;
  vec2 v1, v2, v3, v4;
  vec2 cameraCenter;
  vec2 tPos;
  v1.x = transforms.x;
  v1.y = transformsY;
  v2.x = transforms.x;
  v2.y = transformsY - transforms.w;
  v3.x = transforms.x + transforms.z;
  v3.y = transformsY - transforms.w;
  v4.x = transforms.x + transforms.z;
  v4.y = transformsY;
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

void renderRectangle(renderer2d *r2d, const vec4 transforms, const vec4 colors[4], const vec2 origin, const float rotationDegrees) {
  renderRectangleTexture(r2d, transforms, white1pxSquareTexture, colors, origin, rotationDegrees, defaultTextureCoords);
}

void renderRectangleAbsRotation(renderer2d *r2d, const vec4 transforms, const vec4 colors[4], const vec2 origin, const float rotationDegrees) {
  renderRectangleTextureAbsRotation(r2d, transforms, white1pxSquareTexture, colors, origin, rotationDegrees, defaultTextureCoords);
}

