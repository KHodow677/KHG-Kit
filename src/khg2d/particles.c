#include "particles.h"
#include "utils.h"
#include <stdlib.h>

shader defaultParticleShader = { 0 };
char *defaultParticleVertexShader = "";
char *defaultParticleFragmentShader = "";

void swap(float *a, float *b) {
  float temp = *a;
  *a = *b;
  *b = temp;
}

float rand_float_range(float min, float max) {
  return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

float randParticleSystem(vec2 v) {
  if (v.x > v.y) {
    swap(&v.x, &v.y);
  }
  return rand_float_range(v.x, v.y);
}

float interpolate(float a, float b, float perc) {
  return a * perc + b * (1 - perc);
}

void initKhg2dParticleSystem(void) {
  defaultParticleVertexShader = loadFileContents("./data/shaders/defaultParticleVertexShader.vert");
  defaultParticleFragmentShader = loadFileContents("./data/shaders/defaultParticleFragmentShader.frag");
  defaultParticleShader = createShader(defaultParticleVertexShader, defaultParticleFragmentShader);
}

void cleanupKhg2dParticleSystem(void){
  glDeleteShader(defaultParticleShader.id);
}

void initParticleSystem(particleSystem *ps, int size) {
  int size32Aligned, i;
	cleanupParticleSystem(ps);
	size += 4 - (size % 4);
	ps->size = size;
	size32Aligned = size + (4 - (size % 4));
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
	for (i = 0; i < size; i++) {
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

}

void applyMovement(particleSystem *ps, float deltaTime) {

}

void draw(particleSystem *ps, renderer2d *r) {

}



