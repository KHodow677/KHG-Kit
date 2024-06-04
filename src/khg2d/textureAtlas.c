#include "khg2d.h"

vec4 getTextureAtlas(textureAtlas *ta, int x, int y, bool flip) {
  return computetextureAtlas(ta->xCount, ta->yCount, x, y, flip);
}

