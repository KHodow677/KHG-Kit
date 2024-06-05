#include "../utils/bool.h"
#include "../math/vec4.h"

typedef struct textureAtlas {
  int xCount;
	int yCount;
} textureAtlas;

vec4 computeTextureAtlas(int xCount, int yCount, int x, int y, bool flip);
vec4 computeTextureAtlasWithPadding(int mapXsize, int mapYsize, int xCount, int yCount, int x, int y, bool flip);
vec4 getTextureAtlas(textureAtlas *ta, int x, int y, bool flip);
