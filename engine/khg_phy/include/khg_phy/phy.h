#pragma once

#include "khg_phy/base.h"
#include "khg_phy/collision.h"
#include "khg_phy/id.h"
#include "khg_phy/types.h"
#include <stdbool.h>

B2_API b2WorldId b2CreateWorld(const b2WorldDef* def);
B2_API void b2DestroyWorld(b2WorldId worldId);
B2_API bool b2World_IsValid(b2WorldId id);
B2_API void b2World_Step(b2WorldId worldId, float timeStep, int subStepCount);
B2_API void b2World_Draw(b2WorldId worldId, b2DebugDraw* draw);

B2_API b2BodyEvents b2World_GetBodyEvents(b2WorldId worldId);
B2_API b2SensorEvents b2World_GetSensorEvents(b2WorldId worldId);
B2_API b2ContactEvents b2World_GetContactEvents(b2WorldId worldId);

B2_API void b2World_OverlapAABB(b2WorldId worldId, b2AABB aabb, b2QueryFilter filter, b2OverlapResultFcn* fcn, void* context);
B2_API void b2World_OverlapCircle(b2WorldId worldId, const b2Circle* circle, b2Transform transform, b2QueryFilter filter, b2OverlapResultFcn* fcn, void* context);
B2_API void b2World_OverlapCapsule(b2WorldId worldId, const b2Capsule* capsule, b2Transform transform, b2QueryFilter filter, b2OverlapResultFcn* fcn, void* context);
B2_API void b2World_OverlapPolygon(b2WorldId worldId, const b2Polygon* polygon, b2Transform transform, b2QueryFilter filter, b2OverlapResultFcn* fcn, void* context);

B2_API void b2World_CastRay(b2WorldId worldId, b2Vec2 origin, b2Vec2 translation, b2QueryFilter filter, b2CastResultFcn* fcn, void* context);
B2_API b2RayResult b2World_CastRayClosest(b2WorldId worldId, b2Vec2 origin, b2Vec2 translation, b2QueryFilter filter);
B2_API void b2World_CastCircle(b2WorldId worldId, const b2Circle* circle, b2Transform originTransform, b2Vec2 translation, b2QueryFilter filter, b2CastResultFcn* fcn, void* context);
B2_API void b2World_CastCapsule(b2WorldId worldId, const b2Capsule* capsule, b2Transform originTransform, b2Vec2 translation, b2QueryFilter filter, b2CastResultFcn* fcn, void* context);
B2_API void b2World_CastPolygon(b2WorldId worldId, const b2Polygon* polygon, b2Transform originTransform, b2Vec2 translation, b2QueryFilter filter, b2CastResultFcn* fcn, void* context);

B2_API void b2World_EnableSleeping(b2WorldId worldId, bool flag);
B2_API void b2World_EnableContinuous(b2WorldId worldId, bool flag);

B2_API void b2World_SetRestitutionThreshold(b2WorldId worldId, float value);
B2_API void b2World_SetHitEventThreshold(b2WorldId worldId, float value);

B2_API void b2World_SetCustomFilterCallback(b2WorldId worldId, b2CustomFilterFcn* fcn, void* context);
B2_API void b2World_SetPreSolveCallback(b2WorldId worldId, b2PreSolveFcn* fcn, void* context);

B2_API void b2World_SetGravity(b2WorldId worldId, b2Vec2 gravity);
B2_API b2Vec2 b2World_GetGravity(b2WorldId worldId);

B2_API void b2World_Explode(b2WorldId worldId, b2Vec2 position, float radius, float impulse);
B2_API void b2World_SetContactTuning(b2WorldId worldId, float hertz, float dampingRatio, float pushVelocity);
B2_API void b2World_EnableWarmStarting(b2WorldId worldId, bool flag);

B2_API b2Profile b2World_GetProfile(b2WorldId worldId);
B2_API b2Counters b2World_GetCounters(b2WorldId worldId);

B2_API void b2World_DumpMemoryStats(b2WorldId worldId);

B2_API b2BodyId b2CreateBody(b2WorldId worldId, const b2BodyDef* def);
B2_API void b2DestroyBody(b2BodyId bodyId);
B2_API bool b2Body_IsValid(b2BodyId id);
B2_API b2BodyType b2Body_GetType(b2BodyId bodyId);
B2_API void b2Body_SetType(b2BodyId bodyId, b2BodyType type);
B2_API void b2Body_SetUserData(b2BodyId bodyId, void* userData);
B2_API void* b2Body_GetUserData(b2BodyId bodyId);
B2_API b2Vec2 b2Body_GetPosition(b2BodyId bodyId);
B2_API b2Rot b2Body_GetRotation(b2BodyId bodyId);
B2_API float b2Body_GetAngle(b2BodyId bodyId);
B2_API b2Transform b2Body_GetTransform(b2BodyId bodyId);
B2_API void b2Body_SetTransform(b2BodyId bodyId, b2Vec2 position, float angle);
B2_API b2Vec2 b2Body_GetLocalPoint(b2BodyId bodyId, b2Vec2 worldPoint);
B2_API b2Vec2 b2Body_GetWorldPoint(b2BodyId bodyId, b2Vec2 localPoint);
B2_API b2Vec2 b2Body_GetLocalVector(b2BodyId bodyId, b2Vec2 worldVector);
B2_API b2Vec2 b2Body_GetWorldVector(b2BodyId bodyId, b2Vec2 localVector);
B2_API b2Vec2 b2Body_GetLinearVelocity(b2BodyId bodyId);
B2_API float b2Body_GetAngularVelocity(b2BodyId bodyId);
B2_API void b2Body_SetLinearVelocity(b2BodyId bodyId, b2Vec2 linearVelocity);
B2_API void b2Body_SetAngularVelocity(b2BodyId bodyId, float angularVelocity);
B2_API void b2Body_ApplyForce(b2BodyId bodyId, b2Vec2 force, b2Vec2 point, bool wake);
B2_API void b2Body_ApplyForceToCenter(b2BodyId bodyId, b2Vec2 force, bool wake);
B2_API void b2Body_ApplyTorque(b2BodyId bodyId, float torque, bool wake);
B2_API void b2Body_ApplyLinearImpulse(b2BodyId bodyId, b2Vec2 impulse, b2Vec2 point, bool wake);
B2_API void b2Body_ApplyLinearImpulseToCenter(b2BodyId bodyId, b2Vec2 impulse, bool wake);
B2_API void b2Body_ApplyAngularImpulse(b2BodyId bodyId, float impulse, bool wake);
B2_API float b2Body_GetMass(b2BodyId bodyId);
B2_API float b2Body_GetInertiaTensor(b2BodyId bodyId);
B2_API b2Vec2 b2Body_GetLocalCenterOfMass(b2BodyId bodyId);
B2_API b2Vec2 b2Body_GetWorldCenterOfMass(b2BodyId bodyId);
B2_API void b2Body_SetMassData(b2BodyId bodyId, b2MassData massData);
B2_API b2MassData b2Body_GetMassData(b2BodyId bodyId);
B2_API void b2Body_ApplyMassFromShapes(b2BodyId bodyId);
B2_API void b2Body_SetAutomaticMass(b2BodyId bodyId, bool automaticMass);
B2_API bool b2Body_GetAutomaticMass(b2BodyId bodyId);
B2_API void b2Body_SetLinearDamping(b2BodyId bodyId, float linearDamping);
B2_API float b2Body_GetLinearDamping(b2BodyId bodyId);
B2_API void b2Body_SetAngularDamping(b2BodyId bodyId, float angularDamping);
B2_API float b2Body_GetAngularDamping(b2BodyId bodyId);
B2_API void b2Body_SetGravityScale(b2BodyId bodyId, float gravityScale);
B2_API float b2Body_GetGravityScale(b2BodyId bodyId);
B2_API bool b2Body_IsAwake(b2BodyId bodyId);
B2_API void b2Body_SetAwake(b2BodyId bodyId, bool awake);
B2_API void b2Body_EnableSleep(b2BodyId bodyId, bool enableSleep);
B2_API bool b2Body_IsSleepEnabled(b2BodyId bodyId);
B2_API void b2Body_SetSleepThreshold(b2BodyId bodyId, float sleepVelocity);
B2_API float b2Body_GetSleepThreshold(b2BodyId bodyId);
B2_API bool b2Body_IsEnabled(b2BodyId bodyId);
B2_API void b2Body_Disable(b2BodyId bodyId);
B2_API void b2Body_Enable(b2BodyId bodyId);
B2_API void b2Body_SetFixedRotation(b2BodyId bodyId, bool flag);
B2_API bool b2Body_IsFixedRotation(b2BodyId bodyId);
B2_API void b2Body_SetBullet(b2BodyId bodyId, bool flag);
B2_API bool b2Body_IsBullet(b2BodyId bodyId);
B2_API void b2Body_EnableHitEvents(b2BodyId bodyId, bool enableHitEvents);
B2_API int b2Body_GetShapeCount(b2BodyId bodyId);
B2_API int b2Body_GetShapes(b2BodyId bodyId, b2ShapeId* shapeArray, int capacity);
B2_API int b2Body_GetJointCount(b2BodyId bodyId);
B2_API int b2Body_GetJoints(b2BodyId bodyId, b2JointId* jointArray, int capacity);
B2_API int b2Body_GetContactCapacity(b2BodyId bodyId);
B2_API int b2Body_GetContactData(b2BodyId bodyId, b2ContactData* contactData, int capacity);
B2_API b2AABB b2Body_ComputeAABB(b2BodyId bodyId);

B2_API b2ShapeId b2CreateCircleShape(b2BodyId bodyId, const b2ShapeDef* def, const b2Circle* circle);
B2_API b2ShapeId b2CreateSegmentShape(b2BodyId bodyId, const b2ShapeDef* def, const b2Segment* segment);
B2_API b2ShapeId b2CreateCapsuleShape(b2BodyId bodyId, const b2ShapeDef* def, const b2Capsule* capsule);
B2_API b2ShapeId b2CreatePolygonShape(b2BodyId bodyId, const b2ShapeDef* def, const b2Polygon* polygon);
B2_API void b2DestroyShape(b2ShapeId shapeId);
B2_API bool b2Shape_IsValid(b2ShapeId id);
B2_API b2ShapeType b2Shape_GetType(b2ShapeId shapeId);
B2_API b2BodyId b2Shape_GetBody(b2ShapeId shapeId);
B2_API bool b2Shape_IsSensor(b2ShapeId shapeId);
B2_API void b2Shape_SetUserData(b2ShapeId shapeId, void* userData);
B2_API void* b2Shape_GetUserData(b2ShapeId shapeId);
B2_API void b2Shape_SetDensity(b2ShapeId shapeId, float density);
B2_API float b2Shape_GetDensity(b2ShapeId shapeId);
B2_API void b2Shape_SetFriction(b2ShapeId shapeId, float friction);
B2_API float b2Shape_GetFriction(b2ShapeId shapeId);
B2_API void b2Shape_SetRestitution(b2ShapeId shapeId, float restitution);
B2_API float b2Shape_GetRestitution(b2ShapeId shapeId);
B2_API b2Filter b2Shape_GetFilter(b2ShapeId shapeId);
B2_API void b2Shape_SetFilter(b2ShapeId shapeId, b2Filter filter);
B2_API void b2Shape_EnableSensorEvents(b2ShapeId shapeId, bool flag);
B2_API bool b2Shape_AreSensorEventsEnabled(b2ShapeId shapeId);
B2_API void b2Shape_EnableContactEvents(b2ShapeId shapeId, bool flag);
B2_API bool b2Shape_AreContactEventsEnabled(b2ShapeId shapeId);
B2_API void b2Shape_EnablePreSolveEvents(b2ShapeId shapeId, bool flag);
B2_API bool b2Shape_ArePreSolveEventsEnabled(b2ShapeId shapeId);
B2_API void b2Shape_EnableHitEvents(b2ShapeId shapeId, bool flag);
B2_API bool b2Shape_AreHitEventsEnabled(b2ShapeId shapeId);
B2_API bool b2Shape_TestPoint(b2ShapeId shapeId, b2Vec2 point);
B2_API b2CastOutput b2Shape_RayCast(b2ShapeId shapeId, b2Vec2 origin, b2Vec2 translation);
B2_API b2Circle b2Shape_GetCircle(b2ShapeId shapeId);
B2_API b2Segment b2Shape_GetSegment(b2ShapeId shapeId);
B2_API b2SmoothSegment b2Shape_GetSmoothSegment(b2ShapeId shapeId);
B2_API b2Capsule b2Shape_GetCapsule(b2ShapeId shapeId);
B2_API b2Polygon b2Shape_GetPolygon(b2ShapeId shapeId);
B2_API void b2Shape_SetCircle(b2ShapeId shapeId, const b2Circle* circle);
B2_API void b2Shape_SetCapsule(b2ShapeId shapeId, const b2Capsule* capsule);
B2_API void b2Shape_SetSegment(b2ShapeId shapeId, const b2Segment* segment);
B2_API void b2Shape_SetPolygon(b2ShapeId shapeId, const b2Polygon* polygon);
B2_API b2ChainId b2Shape_GetParentChain(b2ShapeId shapeId);
B2_API int b2Shape_GetContactCapacity(b2ShapeId shapeId);
B2_API int b2Shape_GetContactData(b2ShapeId shapeId, b2ContactData* contactData, int capacity);
B2_API b2AABB b2Shape_GetAABB(b2ShapeId shapeId);
B2_API b2Vec2 b2Shape_GetClosestPoint(b2ShapeId shapeId, b2Vec2 target);

B2_API b2ChainId b2CreateChain(b2BodyId bodyId, const b2ChainDef* def);
B2_API void b2DestroyChain(b2ChainId chainId);
B2_API void b2Chain_SetFriction(b2ChainId chainId, float friction);
B2_API void b2Chain_SetRestitution(b2ChainId chainId, float restitution);
B2_API bool b2Chain_IsValid(b2ChainId id);

B2_API void b2DestroyJoint(b2JointId jointId);
B2_API bool b2Joint_IsValid(b2JointId id);
B2_API b2JointType b2Joint_GetType(b2JointId jointId);
B2_API b2BodyId b2Joint_GetBodyA(b2JointId jointId);
B2_API b2BodyId b2Joint_GetBodyB(b2JointId jointId);
B2_API b2Vec2 b2Joint_GetLocalAnchorA(b2JointId jointId);
B2_API b2Vec2 b2Joint_GetLocalAnchorB(b2JointId jointId);
B2_API void b2Joint_SetCollideConnected(b2JointId jointId, bool shouldCollide);
B2_API bool b2Joint_GetCollideConnected(b2JointId jointId);
B2_API void b2Joint_SetUserData(b2JointId jointId, void* userData);
B2_API void* b2Joint_GetUserData(b2JointId jointId);
B2_API void b2Joint_WakeBodies(b2JointId jointId);
B2_API b2Vec2 b2Joint_GetConstraintForce(b2JointId jointId);
B2_API float b2Joint_GetConstraintTorque(b2JointId jointId);

B2_API b2JointId b2CreateDistanceJoint(b2WorldId worldId, const b2DistanceJointDef* def);
B2_API void b2DistanceJoint_SetLength(b2JointId jointId, float length);
B2_API float b2DistanceJoint_GetLength(b2JointId jointId);
B2_API void b2DistanceJoint_EnableSpring(b2JointId jointId, bool enableSpring);
B2_API bool b2DistanceJoint_IsSpringEnabled(b2JointId jointId);
B2_API void b2DistanceJoint_SetSpringHertz(b2JointId jointId, float hertz);
B2_API void b2DistanceJoint_SetSpringDampingRatio(b2JointId jointId, float dampingRatio);
B2_API float b2DistanceJoint_GetHertz(b2JointId jointId);
B2_API float b2DistanceJoint_GetDampingRatio(b2JointId jointId);
B2_API void b2DistanceJoint_EnableLimit(b2JointId jointId, bool enableLimit);
B2_API bool b2DistanceJoint_IsLimitEnabled(b2JointId jointId);
B2_API void b2DistanceJoint_SetLengthRange(b2JointId jointId, float minLength, float maxLength);
B2_API float b2DistanceJoint_GetMinLength(b2JointId jointId);
B2_API float b2DistanceJoint_GetMaxLength(b2JointId jointId);
B2_API float b2DistanceJoint_GetCurrentLength(b2JointId jointId);
B2_API void b2DistanceJoint_EnableMotor(b2JointId jointId, bool enableMotor);
B2_API bool b2DistanceJoint_IsMotorEnabled(b2JointId jointId);
B2_API void b2DistanceJoint_SetMotorSpeed(b2JointId jointId, float motorSpeed);
B2_API float b2DistanceJoint_GetMotorSpeed(b2JointId jointId);
B2_API void b2DistanceJoint_SetMaxMotorForce(b2JointId jointId, float force);
B2_API float b2DistanceJoint_GetMaxMotorForce(b2JointId jointId);
B2_API float b2DistanceJoint_GetMotorForce(b2JointId jointId);

B2_API b2JointId b2CreateMotorJoint(b2WorldId worldId, const b2MotorJointDef* def);
B2_API void b2MotorJoint_SetLinearOffset(b2JointId jointId, b2Vec2 linearOffset);
B2_API b2Vec2 b2MotorJoint_GetLinearOffset(b2JointId jointId);
B2_API void b2MotorJoint_SetAngularOffset(b2JointId jointId, float angularOffset);
B2_API float b2MotorJoint_GetAngularOffset(b2JointId jointId);
B2_API void b2MotorJoint_SetMaxForce(b2JointId jointId, float maxForce);
B2_API float b2MotorJoint_GetMaxForce(b2JointId jointId);
B2_API void b2MotorJoint_SetMaxTorque(b2JointId jointId, float maxTorque);
B2_API float b2MotorJoint_GetMaxTorque(b2JointId jointId);
B2_API void b2MotorJoint_SetCorrectionFactor(b2JointId jointId, float correctionFactor);
B2_API float b2MotorJoint_GetCorrectionFactor(b2JointId jointId);

B2_API b2JointId b2CreateMouseJoint(b2WorldId worldId, const b2MouseJointDef* def);
B2_API void b2MouseJoint_SetTarget(b2JointId jointId, b2Vec2 target);
B2_API b2Vec2 b2MouseJoint_GetTarget(b2JointId jointId);
B2_API void b2MouseJoint_SetSpringHertz(b2JointId jointId, float hertz);
B2_API float b2MouseJoint_GetSpringHertz(b2JointId jointId);
B2_API void b2MouseJoint_SetSpringDampingRatio(b2JointId jointId, float dampingRatio);
B2_API float b2MouseJoint_GetSpringDampingRatio(b2JointId jointId);
B2_API void b2MouseJoint_SetMaxForce(b2JointId jointId, float maxForce);
B2_API float b2MouseJoint_GetMaxForce(b2JointId jointId);

B2_API b2JointId b2CreatePrismaticJoint(b2WorldId worldId, const b2PrismaticJointDef* def);
B2_API void b2PrismaticJoint_EnableSpring(b2JointId jointId, bool enableSpring);
B2_API bool b2PrismaticJoint_IsSpringEnabled(b2JointId jointId);
B2_API void b2PrismaticJoint_SetSpringHertz(b2JointId jointId, float hertz);
B2_API float b2PrismaticJoint_GetSpringHertz(b2JointId jointId);
B2_API void b2PrismaticJoint_SetSpringDampingRatio(b2JointId jointId, float dampingRatio);
B2_API float b2PrismaticJoint_GetSpringDampingRatio(b2JointId jointId);
B2_API void b2PrismaticJoint_EnableLimit(b2JointId jointId, bool enableLimit);
B2_API bool b2PrismaticJoint_IsLimitEnabled(b2JointId jointId);
B2_API float b2PrismaticJoint_GetLowerLimit(b2JointId jointId);
B2_API float b2PrismaticJoint_GetUpperLimit(b2JointId jointId);
B2_API void b2PrismaticJoint_SetLimits(b2JointId jointId, float lower, float upper);
B2_API void b2PrismaticJoint_EnableMotor(b2JointId jointId, bool enableMotor);
B2_API bool b2PrismaticJoint_IsMotorEnabled(b2JointId jointId);
B2_API void b2PrismaticJoint_SetMotorSpeed(b2JointId jointId, float motorSpeed);
B2_API float b2PrismaticJoint_GetMotorSpeed(b2JointId jointId);
B2_API void b2PrismaticJoint_SetMaxMotorForce(b2JointId jointId, float force);
B2_API float b2PrismaticJoint_GetMaxMotorForce(b2JointId jointId);
B2_API float b2PrismaticJoint_GetMotorForce(b2JointId jointId);

B2_API b2JointId b2CreateRevoluteJoint(b2WorldId worldId, const b2RevoluteJointDef* def);
B2_API void b2RevoluteJoint_EnableSpring(b2JointId jointId, bool enableSpring);
B2_API bool b2RevoluteJoint_IsLimitEnabled(b2JointId jointId);
B2_API void b2RevoluteJoint_SetSpringHertz(b2JointId jointId, float hertz);
B2_API float b2RevoluteJoint_GetSpringHertz(b2JointId jointId);
B2_API void b2RevoluteJoint_SetSpringDampingRatio(b2JointId jointId, float dampingRatio);
B2_API float b2RevoluteJoint_GetSpringDampingRatio(b2JointId jointId);
B2_API float b2RevoluteJoint_GetAngle(b2JointId jointId);
B2_API void b2RevoluteJoint_EnableLimit(b2JointId jointId, bool enableLimit);
B2_API bool b2RevoluteJoint_IsLimitEnabled(b2JointId jointId);
B2_API float b2RevoluteJoint_GetLowerLimit(b2JointId jointId);
B2_API float b2RevoluteJoint_GetUpperLimit(b2JointId jointId);
B2_API void b2RevoluteJoint_SetLimits(b2JointId jointId, float lower, float upper);
B2_API void b2RevoluteJoint_EnableMotor(b2JointId jointId, bool enableMotor);
B2_API bool b2RevoluteJoint_IsMotorEnabled(b2JointId jointId);
B2_API void b2RevoluteJoint_SetMotorSpeed(b2JointId jointId, float motorSpeed);
B2_API float b2RevoluteJoint_GetMotorSpeed(b2JointId jointId);
B2_API float b2RevoluteJoint_GetMotorTorque(b2JointId jointId);
B2_API void b2RevoluteJoint_SetMaxMotorTorque(b2JointId jointId, float torque);
B2_API float b2RevoluteJoint_GetMaxMotorTorque(b2JointId jointId);

B2_API b2JointId b2CreateWeldJoint(b2WorldId worldId, const b2WeldJointDef* def);
B2_API void b2WeldJoint_SetLinearHertz(b2JointId jointId, float hertz);
B2_API float b2WeldJoint_GetLinearHertz(b2JointId jointId);
B2_API void b2WeldJoint_SetLinearDampingRatio(b2JointId jointId, float dampingRatio);
B2_API float b2WeldJoint_GetLinearDampingRatio(b2JointId jointId);
B2_API void b2WeldJoint_SetAngularHertz(b2JointId jointId, float hertz);
B2_API float b2WeldJoint_GetAngularHertz(b2JointId jointId);
B2_API void b2WeldJoint_SetAngularDampingRatio(b2JointId jointId, float dampingRatio);
B2_API float b2WeldJoint_GetAngularDampingRatio(b2JointId jointId);

B2_API b2JointId b2CreateWheelJoint(b2WorldId worldId, const b2WheelJointDef* def);
B2_API void b2WheelJoint_EnableSpring(b2JointId jointId, bool enableSpring);
B2_API bool b2WheelJoint_IsSpringEnabled(b2JointId jointId);
B2_API void b2WheelJoint_SetSpringHertz(b2JointId jointId, float hertz);
B2_API float b2WheelJoint_GetSpringHertz(b2JointId jointId);
B2_API void b2WheelJoint_SetSpringDampingRatio(b2JointId jointId, float dampingRatio);
B2_API float b2WheelJoint_GetSpringDampingRatio(b2JointId jointId);
B2_API void b2WheelJoint_EnableLimit(b2JointId jointId, bool enableLimit);
B2_API bool b2WheelJoint_IsLimitEnabled(b2JointId jointId);
B2_API float b2WheelJoint_GetLowerLimit(b2JointId jointId);
B2_API float b2WheelJoint_GetUpperLimit(b2JointId jointId);
B2_API void b2WheelJoint_SetLimits(b2JointId jointId, float lower, float upper);
B2_API void b2WheelJoint_EnableMotor(b2JointId jointId, bool enableMotor);
B2_API bool b2WheelJoint_IsMotorEnabled(b2JointId jointId);
B2_API void b2WheelJoint_SetMotorSpeed(b2JointId jointId, float motorSpeed);
B2_API float b2WheelJoint_GetMotorSpeed(b2JointId jointId);
B2_API void b2WheelJoint_SetMaxMotorTorque(b2JointId jointId, float torque);
B2_API float b2WheelJoint_GetMaxMotorTorque(b2JointId jointId);
B2_API float b2WheelJoint_GetMotorTorque(b2JointId jointId);
