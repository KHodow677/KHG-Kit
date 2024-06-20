#include "khg2d/textureAtlas.h"

vec4 computeTextureAtlas(int xCount, int yCount, int x, int y, bool flip) {
  float xSize = 1.0f / xCount;
  float ySize = 1.0f / yCount;
  vec4 unflippedVec = { x * xSize, 1 - (y * ySize), (x + 1) * xSize, 1.f - ((y + 1) * ySize) }; 
  vec4 flippedVec = { unflippedVec.z, unflippedVec.y, unflippedVec.x, unflippedVec.w };
  if (flip) {
    return flippedVec;
  }
  else {
    return unflippedVec;
  }
}

vec4 computeTextureAtlasWithPadding(int mapXsize, int mapYsize, int xCount, int yCount, int x, int y, bool flip) {
  float xSize = 1.0f / xCount;
  float ySize = 1.0f / yCount;
  float Xpadding = 1.0f / mapXsize;
  float Ypadding = 1.0f / mapYsize;
  vec4 unflippedVec = { x * xSize + Xpadding, 1 - (y * ySize) - Ypadding, (x + 1) * xSize - Xpadding, 1.0f - ((y + 1) * ySize) + Ypadding };
  vec4 flippedVec = { unflippedVec.z, unflippedVec.y, unflippedVec.x, unflippedVec.w };
  if (flip) {
    return flippedVec;
  }
  else {
    return unflippedVec;
  }
}

vec4 getTextureAtlas(textureAtlas *ta, int x, int y, bool flip) {
  return computeTextureAtlas(ta->xCount, ta->yCount, x, y, flip);
}
