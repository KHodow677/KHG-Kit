#pragma once

#include "khg_phy/base.h"
#include <stdint.h>

typedef struct b2WorldId {
	uint16_t index1;
	uint16_t revision;
} b2WorldId;

typedef struct b2BodyId {
	int32_t index1;
	uint16_t world0;
	uint16_t revision;
} b2BodyId;

typedef struct b2ShapeId {
	int32_t index1;
	uint16_t world0;
	uint16_t revision;
} b2ShapeId;

typedef struct b2JointId {
	int32_t index1;
	uint16_t world0;
	uint16_t revision;
} b2JointId;

typedef struct b2ChainId {
	int32_t index1;
	uint16_t world0;
	uint16_t revision;
} b2ChainId;

static const b2WorldId b2_nullWorldId = B2_ZERO_INIT;
static const b2BodyId b2_nullBodyId = B2_ZERO_INIT;
static const b2ShapeId b2_nullShapeId = B2_ZERO_INIT;
static const b2JointId b2_nullJointId = B2_ZERO_INIT;
static const b2ChainId b2_nullChainId = B2_ZERO_INIT;

#define B2_IS_NULL(id) (id.index1 == 0)
#define B2_IS_NON_NULL(id) (id.index1 != 0)
#define B2_ID_EQUALS(id1, id2) (id1.index1 == id2.index1 && id1.world0 == id2.world0 && id1.revision == id2.revision)
