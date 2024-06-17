#pragma once

#include "framebuffer.h"
#include "texture.h"
#include "renderer2d.h"
#include "shader.h"
#include "../math/vec2.h"
#include "../math/vec4.h"

extern shader defaultParticleShader;

extern char *defaultParticleVertexShader;

extern char *defaultParticleFragmentShader; 

void initKhg2dParticleSystem(void);
void cleanupKhg2dParticleSystem(void);

typedef struct {
  vec2 size;
  vec4 color1;
  vec4 color2;
} particleAppearance;

typedef enum {
  none = 0,
  linear,
  curbe,
  abruptCurbe,
  wave,
  wave2,
  delay,
  delay2
} transitionTypes; 

typedef struct {
  struct particleSettings *deathRattle;
  struct particleSettings *subemitParticle;
  int onCreateCount;
  vec2 subemitParticleTime;
  vec2 positionX;
  vec2 positionY;
  vec2 particleLifeTime;
  vec2 directionX;
  vec2 directionY;
  vec2 dragX;
  vec2 dragY;
  vec2 rotation;
  vec2 rotationSpeed;
  vec2 rotationDrag;
  particleAppearance createApearence;
  particleAppearance createEndApearence;
  texture *texturePtr;
  int transitionType;
} particleSettings;

typedef struct {
  bool postProcessing;
	float pixelateFactor;
  int size;
  float *posX;
  float *posY;
  float *directionX;
  float *directionY;
  float *rotation;
  float *sizeXY;
  float *dragX;
  float *dragY;
  float *duration;
  float *durationTotal;
  vec4 *color;
  float *rotationSpeed;
  float *rotationDrag;
  float *emitTime;
  char *transitionType;
  particleSettings **deathRattle;
  particleSettings **thisParticleSettings;
  particleSettings **emitParticle;
  texture **textures;
  framebuffer fb;
} particleSystem;

void initParticleSystem(particleSystem *ps, int size);
void cleanupParticleSystem(particleSystem *ps);
void emitParticleWave(particleSystem *ps, particleSettings *pSettings, vec2 pos);
void applyMovement(particleSystem *ps, float deltaTime);
void draw(particleSystem *ps, renderer2d *r);


