#include "textureAtlas.h"

vec4 computeTextureAtlas(int xCount, int yCount, int x, int y, bool flip) {
  float xSize = 1.0f / xCount;
  float ySize = 1.0f / yCount;
  vec4 flippedVec, unflippedVec;
  unflippedVec.x = x * xSize;
  unflippedVec.y = 1 - (y * ySize);
  unflippedVec.z = (x + 1) * xSize;
  unflippedVec.w = 1.f - ((y + 1) * ySize); 
  flippedVec.x = unflippedVec.z;
  flippedVec.y = unflippedVec.y;
  flippedVec.z = unflippedVec.x;
  flippedVec.w = unflippedVec.w;
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
  vec4 flippedVec, unflippedVec;
  unflippedVec.x = x * xSize + Xpadding;
  unflippedVec.y = 1 - (y * ySize) - Ypadding;
  unflippedVec.z = (x + 1) * xSize - Xpadding;
  unflippedVec.w = 1.0f - ((y + 1) * ySize) + Ypadding;
  flippedVec.x = unflippedVec.z;
  flippedVec.y = unflippedVec.y;
  flippedVec.z = unflippedVec.x;
  flippedVec.w = unflippedVec.w;
  if (flip) {
    return flippedVec;
  }
  else
  {
    return unflippedVec;
  }
}

vec4 getTextureAtlas(textureAtlas *ta, int x, int y, bool flip) {
  return computeTextureAtlas(ta->xCount, ta->yCount, x, y, flip);
}
