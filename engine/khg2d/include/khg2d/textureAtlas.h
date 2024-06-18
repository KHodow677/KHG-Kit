#pragma once

#include "khgmath/vec4.h"
#include "khgutils/bool.h"

typedef struct {
  int xCount;
	int yCount;
} textureAtlas;

vec4 computeTextureAtlas(int xCount, int yCount, int x, int y, bool flip);
vec4 computeTextureAtlasWithPadding(int mapXsize, int mapYsize, int xCount, int yCount, int x, int y, bool flip);
vec4 getTextureAtlas(textureAtlas *ta, int x, int y, bool flip);
