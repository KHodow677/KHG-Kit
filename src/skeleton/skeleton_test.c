#include "skeleton/skeleton_test.h"
#include "khg_ani/Animation.h"
#include "skeleton/skeleton_get.h"
#include "khg_ani/AnimationState.h"
#include "khg_ani/AnimationStateData.h"
#include "khg_ani/Atlas.h"
#include "khg_ani/Skeleton.h"
#include "khg_ani/SkeletonData.h"
#include "khg_ani/SkeletonJson.h"
#include "khg_ani/extension.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>

int skeleton_test(void) {
  spAtlas* atlas = get_anim_atlas("spineboy-pma");
  if (!atlas) {
    printf("Error loading atlas\n");
    return -1;
  }
  spSkeletonJson* json = spSkeletonJson_create(atlas);
  spSkeletonData* skeletonData = get_anim_skeleton_data("spineboy-pro", json);
  if (!skeletonData) {
    printf("Error loading skeleton data: %s\n", json->error);
    spSkeletonJson_dispose(json);
    return -1;
  }
  spSkeletonJson_dispose(json);
  spSkeleton *skeleton = spSkeleton_create(skeletonData);
  spSkeleton_setToSetupPose(skeleton);
  spAnimationStateData* stateData = spAnimationStateData_create(skeletonData);
  spAnimationState* state = spAnimationState_create(stateData);
  spAnimationState_setAnimationByName(state, 0, "walk", 1); 
  spSkeletonData_dispose(skeletonData);
  spSkeleton_dispose(skeleton);
  spAnimationState_dispose(state);
  spAnimationStateData_dispose(stateData);
  spAtlas_dispose(atlas);
  return 0;
}

void _spAtlasPage_createTexture(spAtlasPage *self, const char *path) {
	self->rendererObject = 0;
	self->width = 2048;
	self->height = 2048;
}

void _spAtlasPage_disposeTexture(spAtlasPage *self) {
}

char *_spUtil_readFile(const char *path, int *length) {
	return _spReadFile(path, length);
}

