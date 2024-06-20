#include "khgmath/math.h"
#include "khg2d/particles.h"
#include "khg2d/utils.h"
#include <math.h>
#include <stdlib.h>

shader defaultParticleShader = { 0 };
char *defaultParticleVertexShader = "";
char *defaultParticleFragmentShader = "";

void swap(float *a, float *b) {
  float temp = *a;
  *a = *b;
  *b = temp;
}

float randFloatRange(float min, float max) {
  return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

float randParticleSystem(vec2 v) {
  if (v.x > v.y) {
    swap(&v.x, &v.y);
  }
  return randFloatRange(v.x, v.y);
}

float interpolate(float a, float b, float perc) {
  return a * perc + b * (1 - perc);
}

void initKhg2dParticleSystem(void) {
  defaultParticleVertexShader = loadFileContents("./res/shaders/defaultParticleVertexShader.vert");
  defaultParticleFragmentShader = loadFileContents("./res/shaders/defaultParticleFragmentShader.frag");
  defaultParticleShader = createShader(defaultParticleVertexShader, defaultParticleFragmentShader);
}

void cleanupKhg2dParticleSystem(void){
  glDeleteShader(defaultParticleShader.id);
}

void initParticleSystem(particleSystem *ps, int size) {
	cleanupParticleSystem(ps);
	size += 4 - (size % 4);
	ps->size = size;
	int size32Aligned = size + (4 - (size % 4));
  ps->posX = (float *)malloc(size32Aligned * sizeof(float));
  ps->posY = (float *)malloc(size32Aligned * sizeof(float));
  ps->directionX = (float *)malloc(size32Aligned * sizeof(float));
  ps->directionY = (float *)malloc(size32Aligned * sizeof(float));
  ps->rotation = (float *)malloc(size32Aligned * sizeof(float));
  ps->sizeXY = (float *)malloc(size32Aligned * sizeof(float));
  ps->dragX = (float *)malloc(size32Aligned * sizeof(float));
  ps->dragY = (float *)malloc(size32Aligned * sizeof(float));
  ps->duration = (float *)malloc(size32Aligned * sizeof(float));
  ps->durationTotal = (float *)malloc(size32Aligned * sizeof(float));
  ps->color = (vec4 *)malloc(size * sizeof(vec4));
  ps->rotationSpeed = (float *)malloc(size32Aligned * sizeof(float));
  ps->rotationDrag = (float *)malloc(size32Aligned * sizeof(float));
  ps->deathRattle = (particleSettings **)malloc(size32Aligned * sizeof(particleSettings *));
  ps->thisParticleSettings = (particleSettings **)malloc(size32Aligned * sizeof(particleSettings *));
  ps->emitParticle = (particleSettings **)malloc(size32Aligned * sizeof(particleSettings *));
  ps->transitionType = (char *)malloc(size32Aligned * sizeof(char));
  ps->textures = (texture **)malloc(size32Aligned * sizeof(texture *));
  ps->emitTime = (float *)malloc(size32Aligned * sizeof(float));
	for (int i = 0; i < size; i++) {
	  ps->duration[i] = 0;
		ps->sizeXY[i] = 0;
		ps->deathRattle[i] = 0;
		ps->textures[i] = NULL;
		ps->thisParticleSettings[i] = NULL;
		ps->emitParticle[i] = NULL;
	}
	createFramebuffer(&ps->fb, 100, 100);
}

void cleanupParticleSystem(particleSystem *ps) {
  free(ps->posX);
  free(ps->posY);
  free(ps->directionX);
  free(ps->directionY);
  free(ps->rotation);
  free(ps->sizeXY);
  free(ps->dragX);
  free(ps->dragY);
  free(ps->duration);
  free(ps->durationTotal);
  free(ps->color);
  free(ps->rotationSpeed);
  free(ps->rotationDrag);
  free(ps->emitTime);
  free(ps->transitionType);
  free(ps->deathRattle);
  free(ps->thisParticleSettings);
  free(ps->emitParticle);
  free(ps->textures);
	ps->posX = 0;
	ps->posY = 0;
	ps->directionX = 0;
	ps->directionY = 0;
	ps->rotation = 0;
	ps->sizeXY = 0;
	ps->dragX = 0;
	ps->dragY = 0;
	ps->duration = 0;
	ps->durationTotal = 0;
	ps->color = 0;
	ps->rotationSpeed = 0;
	ps->rotationDrag = 0;
	ps->emitTime = 0;
	ps->transitionType = 0;
	ps->deathRattle = 0;
	ps->thisParticleSettings = 0;
	ps->emitParticle = 0;
	ps->textures = 0;
	ps->size = 0;
  cleanupFramebuffer(&ps->fb);
}

void emitParticleWave(particleSystem *ps, particleSettings *pSettings, vec2 pos) {
  int recreatedParticlesThisFrame = 0;
  for (int i = 0; i < ps->size; i++) {
    if (recreatedParticlesThisFrame < pSettings->onCreateCount && ps->sizeXY[i] == 0) {
      ps->duration[i] = randParticleSystem(pSettings->particleLifeTime);
      ps->durationTotal[i] = ps->duration[i];
      ps->posX[i] = pos.x + randParticleSystem(pSettings->positionX);
      ps->posY[i] = pos.y + randParticleSystem(pSettings->positionY);
      ps->directionX[i] = randParticleSystem(pSettings->directionX);
      ps->directionY[i] = randParticleSystem(pSettings->directionY);
      ps->rotation[i] = randParticleSystem(pSettings->rotation);
      ps->sizeXY[i] = randParticleSystem(pSettings->createAppearance.size);
      ps->dragX[i] = randParticleSystem(pSettings->dragX);
      ps->dragY[i] = randParticleSystem(pSettings->dragY);
      ps->color[i].x = randParticleSystem((vec2){ pSettings->createAppearance.color1.x, pSettings->createAppearance.color2.x });
      ps->color[i].y = randParticleSystem((vec2){ pSettings->createAppearance.color1.y, pSettings->createAppearance.color2.y });
      ps->color[i].z = randParticleSystem((vec2){ pSettings->createAppearance.color1.z, pSettings->createAppearance.color2.z });
      ps->color[i].w = randParticleSystem((vec2){ pSettings->createAppearance.color1.w, pSettings->createAppearance.color2.w });
      ps->rotationSpeed[i] = randParticleSystem(pSettings->rotationSpeed);
      ps->rotationDrag[i] = randParticleSystem(pSettings->rotationDrag);
      ps->textures[i] = pSettings->texturePtr;
      ps->deathRattle[i] = pSettings->deathRattle;
      ps->transitionType[i] = pSettings->transitionType;
      ps->thisParticleSettings[i] = pSettings;
      ps->emitParticle[i] = pSettings->subEmitParticle;
      ps->emitTime[i] = randParticleSystem(ps->thisParticleSettings[i]->subEmitParticleTime);
      recreatedParticlesThisFrame++;
    }
  }
}

void applyMovement(particleSystem *ps, float deltaTime) {
  int i;
  for (i = 0; i < ps->size; i++) {
    if (ps->duration[i] > 0) {
      ps->duration[i] -= deltaTime;
    }
    if (ps->emitTime[i] > 0 && ps->emitParticle[i]) {
      ps->emitTime[i] -= deltaTime;
    }
    if (ps->duration[i] <= 0) {
      if (ps->deathRattle[i] != NULL && ps->deathRattle[i]->onCreateCount) {
        emitParticleWave(ps, ps->deathRattle[i], (vec2){ ps->posX[i], ps->posY[i] });
      }
      ps->deathRattle[i] = NULL;
      ps->duration[i] = 0;
      ps->sizeXY[i] = 0;
      ps->emitParticle[i] = NULL;
    }
    else if (ps->emitTime[i] <= 0 && ps->emitParticle[i]) {
      ps->emitTime[i] = randParticleSystem(ps->thisParticleSettings[i]->subEmitParticleTime);
      emitParticleWave(ps, ps->emitParticle[i], (vec2){ ps->posX[i], ps->posY[i] });
    }
  }
	for (i = 0; i < ps->size; i++) {
		ps->posX[i] += deltaTime * ps->directionX[i];
	}
	for (i = 0; i < ps->size; i++) {
		ps->posY[i] += deltaTime * ps->directionY[i];
	}
	for (i = 0; i < ps->size; i++) {
		ps->rotation[i] += deltaTime * ps->rotationSpeed[i];
	}
}

void draw(particleSystem *ps, renderer2d *r2d) {
  unsigned int w = r2d->windowW;
  unsigned int h = r2d->windowH;
  camera cam = r2d->currentCamera;
  if (ps->postProcessing) {
    vec2 texSize;
    flush(r2d, true);
    texSize = getTextureSize(&ps->fb.texture);
    if (texSize.x != (w / ps->pixelateFactor) || texSize.y != (w / ps->pixelateFactor)) {
      resizeFramebuffer(&ps->fb, w / ps->pixelateFactor, h / ps->pixelateFactor);
    }
    updateWindowMetrics(r2d, w / ps->pixelateFactor, h / ps->pixelateFactor);
  }
  for (int i = 0; i < ps->size; i++) {
    vec4 pos, c, p;
    if (ps->sizeXY[i] == 0) {
      continue;
    }
    float lifePerc = ps->duration[i] / ps->durationTotal[i];
    switch (ps->transitionType[i]) {
      case none:
        lifePerc = 1;
        break;
      case linear:
        break;
      case curve:
        lifePerc *= lifePerc;
        break;
      case abruptCurve:
        lifePerc *= lifePerc * lifePerc;
        break;
      case wave:
        lifePerc = (cos(lifePerc * 5.0f * PI) * lifePerc + lifePerc) * 2.0f;
        break;
      case wave2:
        lifePerc = cos(lifePerc * 5.0f * PI) * sqrt(lifePerc) * 0.9f * 0.1f;
        break;
  		case delay:
		    lifePerc = (cos(lifePerc * PI * 2) * sin(lifePerc * lifePerc)) / 2.f;
        break;
      case delay2:
        lifePerc = atan(2 * lifePerc * lifePerc * lifePerc * PI) / 2.f;
        break;
		  default:
        break;
    }
    if (ps->thisParticleSettings[i]) {
      pos = (vec4){ 
        ps->posX[i], 
        ps->posY[i], 
        interpolate(ps->sizeXY[i], ps->thisParticleSettings[i]->createAppearance.size.x, lifePerc),
        pos.z 
      };
      c = (vec4){
        interpolate(ps->color[i].x, ps->thisParticleSettings[i]->createAppearance.color1.x, lifePerc),
        interpolate(ps->color[i].y, ps->thisParticleSettings[i]->createAppearance.color1.y, lifePerc),
        interpolate(ps->color[i].z, ps->thisParticleSettings[i]->createAppearance.color1.z, lifePerc),
        interpolate(ps->color[i].w, ps->thisParticleSettings[i]->createAppearance.color1.w, lifePerc)
      };
    }
    else {
      pos = (vec4){ ps->posX[i], ps->posY[i], ps->sizeXY[i], pos.z };
      c = (vec4) { ps->color[i].x, ps->color[i].y, ps->color[i].z, ps->color[i].w };
    }
    if (ps->postProcessing) {
      r2d->currentCamera = cam;
      p = vec4MultiplyNumOnVec4(1.0f / ps->pixelateFactor, &pos);
      p.x -= r2d->currentCamera.position.x / ps->pixelateFactor;
      p.y -= r2d->currentCamera.position.y / ps->pixelateFactor;
      r2d->currentCamera.position = (vec2){ 0.0f, 0.0f };
    }
    else {
      p = pos;
    }
    if (ps->textures[i] != NULL) {
      vec2 origin = { 0.0f, 0.0f };
      vec4 cData[4] = { c, c, c, c };
      renderRectangleTexture(r2d, p, *ps->textures[i], cData, origin, ps->rotation[i], defaultTextureCoords);
    }
    else {
      vec2 origin = { 0.0f, 0.0f };
      vec4 cData[4] = { c, c, c, c };
      renderRectangle(r2d, p, cData, origin, ps->rotation[i]);
    }
    if (ps->postProcessing) {
      vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
      vec4 cData[4] = { color, color, color, color };
      vec2 origin = { 0.0f, 0.0f };
      clearFramebuffer(&ps->fb);
      flushFbo(r2d, ps->fb, true);
      updateWindowMetrics(r2d, w, h);
      setDefault(&r2d->currentCamera);
      shader s = r2d->currentShader;
      vec4 transform = { 0.0f, 0.0f, w, h };
      renderRectangleTexture(r2d, transform, ps->fb.texture, cData, origin, 0.0f, defaultTextureCoords);
      setShader(r2d, defaultParticleShader);
      flush(r2d, true);
      setShader(r2d, s);
    }
    r2d->currentCamera = cam;
  }
}

