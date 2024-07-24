#pragma once

#include "khg_phy/base.h"
#include "khg_phy/collision.h"
#include "khg_phy/id.h"
#include "khg_phy/math_functions.h"
#include <stdbool.h>
#include <stdint.h>

typedef void b2TaskCallback(int32_t startIndex, int32_t endIndex, uint32_t workerIndex, void *taskContext);
typedef void *b2EnqueueTaskCallback(b2TaskCallback *task, int32_t itemCount, int32_t minRange, void *taskContext, void *userContext);
typedef void b2FinishTaskCallback(void *userTask, void *userContext);

typedef struct b2RayResult {
	b2ShapeId shapeId;
	b2Vec2 point;
	b2Vec2 normal;
	float fraction;
	bool hit;
} b2RayResult;

typedef struct b2WorldDef {
	b2Vec2 gravity;
	float restitutionThreshold;
	float contactPushoutVelocity;
	float hitEventThreshold;
	float contactHertz;
	float contactDampingRatio;
	float jointHertz;
	float jointDampingRatio;
	bool enableSleep;
	bool enableContinous;
	int32_t workerCount;
	b2EnqueueTaskCallback *enqueueTask;
	b2FinishTaskCallback *finishTask;
	void *userTaskContext;
	int32_t internalValue;
} b2WorldDef;

B2_API b2WorldDef b2DefaultWorldDef();
typedef enum b2BodyType {
	b2_staticBody = 0,
	b2_kinematicBody = 1,
	b2_dynamicBody = 2,
	b2_bodyTypeCount,
} b2BodyType;

typedef struct b2BodyDef {
	b2BodyType type;
	b2Vec2 position;
	float angle;
	b2Vec2 linearVelocity;
	float angularVelocity;
	float linearDamping;
	float angularDamping;
	float gravityScale;
	float sleepThreshold;
	void *userData;
	bool enableSleep;
	bool isAwake;
	bool fixedRotation;
	bool isBullet;
	bool isEnabled;
	bool automaticMass;
	int32_t internalValue;
} b2BodyDef;

B2_API b2BodyDef b2DefaultBodyDef();

typedef struct b2Filter {
	uint32_t categoryBits;
	uint32_t maskBits;
	int32_t groupIndex;
} b2Filter;

B2_API b2Filter b2DefaultFilter();

typedef struct b2QueryFilter {
	uint32_t categoryBits;
	uint32_t maskBits;
} b2QueryFilter;

B2_API b2QueryFilter b2DefaultQueryFilter();

typedef enum b2ShapeType {
	b2_circleShape,
	b2_capsuleShape,
	b2_segmentShape,
	b2_polygonShape,
	b2_smoothSegmentShape,
	b2_shapeTypeCount
} b2ShapeType;

typedef struct b2ShapeDef {
	void *userData;
	float friction;
	float restitution;
	float density;
	b2Filter filter;
	bool isSensor;
	bool enableSensorEvents;
	bool enableContactEvents;
	bool enableHitEvents;
	bool enablePreSolveEvents;
	bool forceContactCreation;
	int32_t internalValue;
} b2ShapeDef;

B2_API b2ShapeDef b2DefaultShapeDef();

typedef struct b2ChainDef {
	void *userData;
	const b2Vec2 *points;
	int32_t count;
	float friction;
	float restitution;
	b2Filter filter;
	bool isLoop;
	int32_t internalValue;
} b2ChainDef;

B2_API b2ChainDef b2DefaultChainDef();

typedef struct b2Profile {
	float step;
	float pairs;
	float collide;
	float solve;
	float buildIslands;
	float solveConstraints;
	float prepareTasks;
	float solverTasks;
	float prepareConstraints;
	float integrateVelocities;
	float warmStart;
	float solveVelocities;
	float integratePositions;
	float relaxVelocities;
	float applyRestitution;
	float storeImpulses;
	float finalizeBodies;
	float splitIslands;
	float sleepIslands;
	float hitEvents;
	float broadphase;
	float continuous;
} b2Profile;

typedef struct b2Counters {
	int32_t staticBodyCount;
	int32_t bodyCount;
	int32_t shapeCount;
	int32_t contactCount;
	int32_t jointCount;
	int32_t islandCount;
	int32_t stackUsed;
	int32_t staticTreeHeight;
	int32_t treeHeight;
	int32_t byteCount;
	int32_t taskCount;
	int32_t colorCounts[12];
} b2Counters;

typedef enum b2JointType {
	b2_distanceJoint,
	b2_motorJoint,
	b2_mouseJoint,
	b2_prismaticJoint,
	b2_revoluteJoint,
	b2_weldJoint,
	b2_wheelJoint,
} b2JointType;

typedef struct b2DistanceJointDef {
	b2BodyId bodyIdA;
	b2BodyId bodyIdB;
	b2Vec2 localAnchorA;
	b2Vec2 localAnchorB;
	float length;
	bool enableSpring;
	float hertz;
	float dampingRatio;
	bool enableLimit;
	float minLength;
	float maxLength;
	bool enableMotor;
	float maxMotorForce;
	float motorSpeed;
	bool collideConnected;
	void *userData;
	int32_t internalValue;
} b2DistanceJointDef;

B2_API b2DistanceJointDef b2DefaultDistanceJointDef();

typedef struct b2MotorJointDef {
	b2BodyId bodyIdA;
	b2BodyId bodyIdB;
	b2Vec2 linearOffset;
	float angularOffset;
	float maxForce;
	float maxTorque;
	float correctionFactor;
	bool collideConnected;
	void *userData;
	int32_t internalValue;
} b2MotorJointDef;

B2_API b2MotorJointDef b2DefaultMotorJointDef();

typedef struct b2MouseJointDef {
	b2BodyId bodyIdA;
	b2BodyId bodyIdB;
	b2Vec2 target;
	float hertz;
	float dampingRatio;
	float maxForce;
	bool collideConnected;
	void *userData;
	int32_t internalValue;
} b2MouseJointDef;

B2_API b2MouseJointDef b2DefaultMouseJointDef();

typedef struct b2PrismaticJointDef {
	b2BodyId bodyIdA;
	b2BodyId bodyIdB;
	b2Vec2 localAnchorA;
	b2Vec2 localAnchorB;
	b2Vec2 localAxisA;
	float referenceAngle;
	bool enableSpring;
	float hertz;
	float dampingRatio;
	bool enableLimit;
	float lowerTranslation;
	float upperTranslation;
	bool enableMotor;
	float maxMotorForce;
	float motorSpeed;
	bool collideConnected;
	void *userData;
	int32_t internalValue;
} b2PrismaticJointDef;

B2_API b2PrismaticJointDef b2DefaultPrismaticJointDef();

typedef struct b2RevoluteJointDef {
	b2BodyId bodyIdA;
	b2BodyId bodyIdB;
	b2Vec2 localAnchorA;
	b2Vec2 localAnchorB;
	float referenceAngle;
	bool enableSpring;
	float hertz;
	float dampingRatio;
	bool enableLimit;
	float lowerAngle;
	float upperAngle;
	bool enableMotor;
	float maxMotorTorque;
	float motorSpeed;
	float drawSize;
	bool collideConnected;
	void *userData;
	int32_t internalValue;
} b2RevoluteJointDef;

B2_API b2RevoluteJointDef b2DefaultRevoluteJointDef();

typedef struct b2WeldJointDef {
	b2BodyId bodyIdA;
	b2BodyId bodyIdB;
	b2Vec2 localAnchorA;
	b2Vec2 localAnchorB;
	float referenceAngle;
	float linearHertz;
	float angularHertz;
	float linearDampingRatio;
	float angularDampingRatio;
	bool collideConnected;
	void *userData;
	int32_t internalValue;
} b2WeldJointDef;

B2_API b2WeldJointDef b2DefaultWeldJointDef();

typedef struct b2WheelJointDef {
	b2BodyId bodyIdA;
	b2BodyId bodyIdB;
	b2Vec2 localAnchorA;
	b2Vec2 localAnchorB;
	b2Vec2 localAxisA;
	bool enableSpring;
	float hertz;
	float dampingRatio;
	bool enableLimit;
	float lowerTranslation;
	float upperTranslation;
	bool enableMotor;
	float maxMotorTorque;
	float motorSpeed;
	bool collideConnected;
	void *userData;
	int32_t internalValue;
} b2WheelJointDef;

B2_API b2WheelJointDef b2DefaultWheelJointDef();

typedef struct b2SensorBeginTouchEvent {
	b2ShapeId sensorShapeId;
	b2ShapeId visitorShapeId;
} b2SensorBeginTouchEvent;

typedef struct b2SensorEndTouchEvent {
	b2ShapeId sensorShapeId;
	b2ShapeId visitorShapeId;
} b2SensorEndTouchEvent;

typedef struct b2SensorEvents {
	b2SensorBeginTouchEvent *beginEvents;
	b2SensorEndTouchEvent *endEvents;
	int32_t beginCount;
	int32_t endCount;
} b2SensorEvents;

typedef struct b2ContactBeginTouchEvent {
	b2ShapeId shapeIdA;
	b2ShapeId shapeIdB;
} b2ContactBeginTouchEvent;

typedef struct b2ContactEndTouchEvent {
	b2ShapeId shapeIdA;
	b2ShapeId shapeIdB;
} b2ContactEndTouchEvent;

typedef struct b2ContactHitEvent {
	b2ShapeId shapeIdA;
	b2ShapeId shapeIdB;
	b2Vec2 point;
	b2Vec2 normal;
	float approachSpeed;
} b2ContactHitEvent;

typedef struct b2ContactEvents {
	b2ContactBeginTouchEvent *beginEvents;
	b2ContactEndTouchEvent *endEvents;
	b2ContactHitEvent *hitEvents;
	int32_t beginCount;
	int32_t endCount;
	int32_t hitCount;
} b2ContactEvents;

typedef struct b2BodyMoveEvent {
	b2Transform transform;
	b2BodyId bodyId;
	void *userData;
	bool fellAsleep;
} b2BodyMoveEvent;

typedef struct b2BodyEvents {
	b2BodyMoveEvent *moveEvents;
	int32_t moveCount;
} b2BodyEvents;

typedef struct b2ContactData {
	b2ShapeId shapeIdA;
	b2ShapeId shapeIdB;
	b2Manifold manifold;
} b2ContactData;

typedef bool b2CustomFilterFcn(b2ShapeId shapeIdA, b2ShapeId shapeIdB, void *context);
typedef bool b2PreSolveFcn(b2ShapeId shapeIdA, b2ShapeId shapeIdB, b2Manifold *manifold, void *context);
typedef bool b2OverlapResultFcn(b2ShapeId shapeId, void *context);
typedef float b2CastResultFcn(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void *context);

typedef enum b2HexColor {
	b2_colorAliceBlue = 0xf0f8ff,
	b2_colorAntiqueWhite = 0xfaebd7,
	b2_colorAqua = 0x00ffff,
	b2_colorAquamarine = 0x7fffd4,
	b2_colorAzure = 0xf0ffff,
	b2_colorBeige = 0xf5f5dc,
	b2_colorBisque = 0xffe4c4,
	b2_colorBlack = 0x000000,
	b2_colorBlanchedAlmond = 0xffebcd,
	b2_colorBlue = 0x0000ff,
	b2_colorBlueViolet = 0x8a2be2,
	b2_colorBrown = 0xa52a2a,
	b2_colorBurlywood = 0xdeb887,
	b2_colorCadetBlue = 0x5f9ea0,
	b2_colorChartreuse = 0x7fff00,
	b2_colorChocolate = 0xd2691e,
	b2_colorCoral = 0xff7f50,
	b2_colorCornflowerBlue = 0x6495ed,
	b2_colorCornsilk = 0xfff8dc,
	b2_colorCrimson = 0xdc143c,
	b2_colorCyan = 0x00ffff,
	b2_colorDarkBlue = 0x00008b,
	b2_colorDarkCyan = 0x008b8b,
	b2_colorDarkGoldenrod = 0xb8860b,
	b2_colorDarkGray = 0xa9a9a9,
	b2_colorDarkGreen = 0x006400,
	b2_colorDarkKhaki = 0xbdb76b,
	b2_colorDarkMagenta = 0x8b008b,
	b2_colorDarkOliveGreen = 0x556b2f,
	b2_colorDarkOrange = 0xff8c00,
	b2_colorDarkOrchid = 0x9932cc,
	b2_colorDarkRed = 0x8b0000,
	b2_colorDarkSalmon = 0xe9967a,
	b2_colorDarkSeaGreen = 0x8fbc8f,
	b2_colorDarkSlateBlue = 0x483d8b,
	b2_colorDarkSlateGray = 0x2f4f4f,
	b2_colorDarkTurquoise = 0x00ced1,
	b2_colorDarkViolet = 0x9400d3,
	b2_colorDeepPink = 0xff1493,
	b2_colorDeepSkyBlue = 0x00bfff,
	b2_colorDimGray = 0x696969,
	b2_colorDodgerBlue = 0x1e90ff,
	b2_colorFirebrick = 0xb22222,
	b2_colorFloralWhite = 0xfffaf0,
	b2_colorForestGreen = 0x228b22,
	b2_colorFuchsia = 0xff00ff,
	b2_colorGainsboro = 0xdcdcdc,
	b2_colorGhostWhite = 0xf8f8ff,
	b2_colorGold = 0xffd700,
	b2_colorGoldenrod = 0xdaa520,
	b2_colorGray = 0xbebebe,
	b2_colorGray1 = 0x1a1a1a,
	b2_colorGray2 = 0x333333,
	b2_colorGray3 = 0x4d4d4d,
	b2_colorGray4 = 0x666666,
	b2_colorGray5 = 0x7f7f7f,
	b2_colorGray6 = 0x999999,
	b2_colorGray7 = 0xb3b3b3,
	b2_colorGray8 = 0xcccccc,
	b2_colorGray9 = 0xe5e5e5,
	b2_colorGreen = 0x00ff00,
	b2_colorGreenYellow = 0xadff2f,
	b2_colorHoneydew = 0xf0fff0,
	b2_colorHotPink = 0xff69b4,
	b2_colorIndianRed = 0xcd5c5c,
	b2_colorIndigo = 0x4b0082,
	b2_colorIvory = 0xfffff0,
	b2_colorKhaki = 0xf0e68c,
	b2_colorLavender = 0xe6e6fa,
	b2_colorLavenderBlush = 0xfff0f5,
	b2_colorLawnGreen = 0x7cfc00,
	b2_colorLemonChiffon = 0xfffacd,
	b2_colorLightBlue = 0xadd8e6,
	b2_colorLightCoral = 0xf08080,
	b2_colorLightCyan = 0xe0ffff,
	b2_colorLightGoldenrod = 0xeedd82,
	b2_colorLightGoldenrodYellow = 0xfafad2,
	b2_colorLightGray = 0xd3d3d3,
	b2_colorLightGreen = 0x90ee90,
	b2_colorLightPink = 0xffb6c1,
	b2_colorLightSalmon = 0xffa07a,
	b2_colorLightSeaGreen = 0x20b2aa,
	b2_colorLightSkyBlue = 0x87cefa,
	b2_colorLightSlateBlue = 0x8470ff,
	b2_colorLightSlateGray = 0x778899,
	b2_colorLightSteelBlue = 0xb0c4de,
	b2_colorLightYellow = 0xffffe0,
	b2_colorLime = 0x00ff00,
	b2_colorLimeGreen = 0x32cd32,
	b2_colorLinen = 0xfaf0e6,
	b2_colorMagenta = 0xff00ff,
	b2_colorMaroon = 0xb03060,
	b2_colorMediumAquamarine = 0x66cdaa,
	b2_colorMediumBlue = 0x0000cd,
	b2_colorMediumOrchid = 0xba55d3,
	b2_colorMediumPurple = 0x9370db,
	b2_colorMediumSeaGreen = 0x3cb371,
	b2_colorMediumSlateBlue = 0x7b68ee,
	b2_colorMediumSpringGreen = 0x00fa9a,
	b2_colorMediumTurquoise = 0x48d1cc,
	b2_colorMediumVioletRed = 0xc71585,
	b2_colorMidnightBlue = 0x191970,
	b2_colorMintCream = 0xf5fffa,
	b2_colorMistyRose = 0xffe4e1,
	b2_colorMoccasin = 0xffe4b5,
	b2_colorNavajoWhite = 0xffdead,
	b2_colorNavy = 0x000080,
	b2_colorNavyBlue = 0x000080,
	b2_colorOldLace = 0xfdf5e6,
	b2_colorOlive = 0x808000,
	b2_colorOliveDrab = 0x6b8e23,
	b2_colorOrange = 0xffa500,
	b2_colorOrangeRed = 0xff4500,
	b2_colorOrchid = 0xda70d6,
	b2_colorPaleGoldenrod = 0xeee8aa,
	b2_colorPaleGreen = 0x98fb98,
	b2_colorPaleTurquoise = 0xafeeee,
	b2_colorPaleVioletRed = 0xdb7093,
	b2_colorPapayaWhip = 0xffefd5,
	b2_colorPeachPuff = 0xffdab9,
	b2_colorPeru = 0xcd853f,
	b2_colorPink = 0xffc0cb,
	b2_colorPlum = 0xdda0dd,
	b2_colorPowderBlue = 0xb0e0e6,
	b2_colorPurple = 0xa020f0,
	b2_colorRebeccaPurple = 0x663399,
	b2_colorRed = 0xff0000,
	b2_colorRosyBrown = 0xbc8f8f,
	b2_colorRoyalBlue = 0x4169e1,
	b2_colorSaddleBrown = 0x8b4513,
	b2_colorSalmon = 0xfa8072,
	b2_colorSandyBrown = 0xf4a460,
	b2_colorSeaGreen = 0x2e8b57,
	b2_colorSeashell = 0xfff5ee,
	b2_colorSienna = 0xa0522d,
	b2_colorSilver = 0xc0c0c0,
	b2_colorSkyBlue = 0x87ceeb,
	b2_colorSlateBlue = 0x6a5acd,
	b2_colorSlateGray = 0x708090,
	b2_colorSnow = 0xfffafa,
	b2_colorSpringGreen = 0x00ff7f,
	b2_colorSteelBlue = 0x4682b4,
	b2_colorTan = 0xd2b48c,
	b2_colorTeal = 0x008080,
	b2_colorThistle = 0xd8bfd8,
	b2_colorTomato = 0xff6347,
	b2_colorTurquoise = 0x40e0d0,
	b2_colorViolet = 0xee82ee,
	b2_colorVioletRed = 0xd02090,
	b2_colorWheat = 0xf5deb3,
	b2_colorWhite = 0xffffff,
	b2_colorWhiteSmoke = 0xf5f5f5,
	b2_colorYellow = 0xffff00,
	b2_colorYellowGreen = 0x9acd32,
} b2HexColor;

typedef struct b2DebugDraw {
	void (*DrawPolygon)(const b2Vec2 *vertices, int vertexCount, b2HexColor color, void *context);
	void (*DrawSolidPolygon)(b2Transform transform, const b2Vec2 *vertices, int vertexCount, float radius, b2HexColor color, void *context);
	void (*DrawCircle)(b2Vec2 center, float radius, b2HexColor color, void *context);
	void (*DrawSolidCircle)(b2Transform transform, float radius, b2HexColor color, void *context);
	void (*DrawCapsule)(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void *context);
	void (*DrawSolidCapsule)(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void *context);
	void (*DrawSegment)(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void *context);
	void (*DrawTransform)(b2Transform transform, void *context);
	void (*DrawPoint)(b2Vec2 p, float size, b2HexColor color, void *context);
	void (*DrawString)(b2Vec2 p, const char *s, void *context);
	b2AABB drawingBounds;
	bool useDrawingBounds;
	bool drawShapes;
	bool drawJoints;
	bool drawJointExtras;
	bool drawAABBs;
	bool drawMass;
	bool drawContacts;
	bool drawGraphColors;
	bool drawContactNormals;
	bool drawContactImpulses;
	bool drawFrictionImpulses;
	void *context;
} b2DebugDraw;