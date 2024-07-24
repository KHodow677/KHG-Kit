#pragma once

#include "cglm/types-struct.h"
#include "cglm/vec2.h"
#include "cglm/mat2.h"

#define PHYSACDEF extern        // Functions visible from other files

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define     PHYSAC_MAX_BODIES               64
#define     PHYSAC_MAX_MANIFOLDS            4096
#define     PHYSAC_MAX_VERTICES             24
#define     PHYSAC_CIRCLE_VERTICES          24

#define     PHYSAC_COLLISION_ITERATIONS     100
#define     PHYSAC_PENETRATION_ALLOWANCE    0.05f
#define     PHYSAC_PENETRATION_CORRECTION   0.4f

#define     PHYSAC_PI                       3.14159265358979323846
#define     PHYSAC_DEG2RAD                  (PHYSAC_PI/180.0f)

//----------------------------------------------------------------------------------
// Types and Structures Definition
// NOTE: Below types are required for PHYSAC_STANDALONE usage
//----------------------------------------------------------------------------------
#if defined(PHYSAC_STANDALONE)
    // Vector2 type
    typedef struct Vector2 {
        float x;
        float y;
    } Vector2;

    // Boolean type
    #if !defined(_STDBOOL_H)
        typedef enum { false, true } bool;
        #define _STDBOOL_H
    #endif
#endif

typedef enum PhysicsShapeType { PHYSICS_CIRCLE, PHYSICS_POLYGON } PhysicsShapeType;

// Previously defined to be used in PhysicsShape struct as circular dependencies
typedef struct PhysicsBodyData *PhysicsBody;

typedef struct PolygonData {
    unsigned int vertexCount;                   // Current used vertex and normals count
    vec2s positions[PHYSAC_MAX_VERTICES];     // Polygon vertex positions vectors
    vec2s normals[PHYSAC_MAX_VERTICES];       // Polygon vertex normals vectors
} PolygonData;

typedef struct PhysicsShape {
    PhysicsShapeType type;                      // Physics shape type (circle or polygon)
    PhysicsBody body;                           // Shape physics body reference
    float radius;                               // Circle shape radius (used for circle shapes)
    mat2s transform;                             // Vertices transform matrix 2x2
    PolygonData vertexData;                     // Polygon shape vertices position and normals data (just used for polygon shapes)
} PhysicsShape;

typedef struct PhysicsBodyData {
    unsigned int id;                            // Reference unique identifier
    bool enabled;                               // Enabled dynamics state (collisions are calculated anyway)
    vec2s position;                           // Physics body shape pivot
    vec2s velocity;                           // Current linear velocity applied to position
    vec2s force;                              // Current linear force (reset to 0 every step)
    float angularVelocity;                      // Current angular velocity applied to orient
    float torque;                               // Current angular force (reset to 0 every step)
    float orient;                               // Rotation in radians
    float inertia;                              // Moment of inertia
    float inverseInertia;                       // Inverse value of inertia
    float mass;                                 // Physics body mass
    float inverseMass;                          // Inverse value of mass
    float staticFriction;                       // Friction when the body has not movement (0 to 1)
    float dynamicFriction;                      // Friction when the body has movement (0 to 1)
    float restitution;                          // Restitution coefficient of the body (0 to 1)
    bool useGravity;                            // Apply gravity force to dynamics
    bool isGrounded;                            // Physics grounded on other body state
    bool freezeOrient;                          // Physics rotation constraint
    PhysicsShape shape;                         // Physics body shape information (type, radius, vertices, normals)
} PhysicsBodyData;

typedef struct PhysicsManifoldData {
    unsigned int id;                            // Reference unique identifier
    PhysicsBody bodyA;                          // Manifold first physics body reference
    PhysicsBody bodyB;                          // Manifold second physics body reference
    float penetration;                          // Depth of penetration from collision
    vec2s normal;                             // Normal direction vector from 'a' to 'b'
    vec2s contacts[2];                        // Points of contact during collision
    unsigned int contactsCount;                 // Current collision number of contacts
    float restitution;                          // Mixed restitution during collision
    float dynamicFriction;                      // Mixed dynamic friction during collision
    float staticFriction;                       // Mixed static friction during collision
} PhysicsManifoldData, *PhysicsManifold;

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
PHYSACDEF void InitPhysics(void);                                                                           // Initializes physics values, pointers and creates physics loop thread
PHYSACDEF void RunPhysicsStep(void);                                                                        // Run physics step, to be used if PHYSICS_NO_THREADS is set in your main loop
PHYSACDEF void SetPhysicsTimeStep(double delta);                                                            // Sets physics fixed time step in milliseconds. 1.666666 by default
PHYSACDEF bool IsPhysicsEnabled(void);                                                                      // Returns true if physics thread is currently enabled
PHYSACDEF void SetPhysicsGravity(float x, float y);                                                         // Sets physics global gravity force
PHYSACDEF PhysicsBody CreatePhysicsBodyCircle(vec2s pos, float radius, float density);                    // Creates a new circle physics body with generic parameters
PHYSACDEF PhysicsBody CreatePhysicsBodyRectangle(vec2s pos, float width, float height, float density);    // Creates a new rectangle physics body with generic parameters
PHYSACDEF PhysicsBody CreatePhysicsBodyPolygon(vec2s pos, float radius, int sides, float density);        // Creates a new polygon physics body with generic parameters
PHYSACDEF void PhysicsAddForce(PhysicsBody body, vec2s force);                                            // Adds a force to a physics body
PHYSACDEF void PhysicsAddTorque(PhysicsBody body, float amount);                                            // Adds an angular force to a physics body
PHYSACDEF void PhysicsShatter(PhysicsBody body, vec2s position, float force);                             // Shatters a polygon shape physics body to little physics bodies with explosion force
PHYSACDEF int GetPhysicsBodiesCount(void);                                                                  // Returns the current amount of created physics bodies
PHYSACDEF PhysicsBody GetPhysicsBody(int index);                                                            // Returns a physics body of the bodies pool at a specific index
PHYSACDEF int GetPhysicsShapeType(int index);                                                               // Returns the physics body shape type (PHYSICS_CIRCLE or PHYSICS_POLYGON)
PHYSACDEF int GetPhysicsShapeVerticesCount(int index);                                                      // Returns the amount of vertices of a physics body shape
PHYSACDEF vec2s GetPhysicsShapeVertex(PhysicsBody body, int vertex);                                      // Returns transformed position of a body shape (body position + vertex transformed position)
PHYSACDEF void SetPhysicsBodyRotation(PhysicsBody body, float radians);                                     // Sets physics body shape transform based on radians parameter
PHYSACDEF void DestroyPhysicsBody(PhysicsBody body);                                                        // Unitializes and destroy a physics body
PHYSACDEF void ClosePhysics(void);                                                                          // Unitializes physics pointers and closes physics loop thread

/***********************************************************************************
*
*   PHYSAC IMPLEMENTATION
*
************************************************************************************/

#include <pthread.h>            // Required for: pthread_t, pthread_create()
#include <stdlib.h>                 // Required for: malloc(), free(), srand(), rand()
#include <math.h>                   // Required for: cosf(), sinf(), fabs(), sqrtf()
#include <stdint.h>                 // Required for: uint64_t

// Time management functionality
#include <time.h>                   // Required for: time(), clock_gettime()
#if defined(_WIN32)
    int __stdcall QueryPerformanceCounter(unsigned long long int* lpPerformanceCount);
    int __stdcall QueryPerformanceFrequency(unsigned long long int* lpFrequency);
#elif defined(__linux__)
    #if _POSIX_C_SOURCE < 199309L
        #undef _POSIX_C_SOURCE
        #define _POSIX_C_SOURCE 199309L // Required for CLOCK_MONOTONIC if compiled with c99 without gnu ext.
    #endif
    #include <sys/time.h>           // Required for: timespec
#elif defined(__APPLE__)            // macOS also defines __MACH__
    #include <mach/mach_time.h>     // Required for: mach_absolute_time()
#elif defined(EMSCRIPTEN)           // Emscripten uses the browser's time functions
    #include <emscripten.h>         // Required for: emscripten_get_now()
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define     min(a,b)                    (((a)<(b))?(a):(b))
#define     max(a,b)                    (((a)>(b))?(a):(b))
#define     PHYSAC_FLT_MAX              3.402823466e+38f
#define     PHYSAC_EPSILON              0.000001f
#define     PHYSAC_K                    1.0f/3.0f
#define     PHYSAC_VECTOR_ZERO          (vec2s){ 0.0f, 0.0f }

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
#if !defined(PHYSAC_NO_THREADS)
static pthread_t physicsThreadId;                           // Physics thread id
#endif
static unsigned int usedMemory = 0;                         // Total allocated dynamic memory
static volatile bool physicsThreadEnabled = false;          // Physics thread enabled state
static double baseTime = 0.0;                               // Offset time for MONOTONIC clock
static double startTime = 0.0;                              // Start time in milliseconds
static double deltaTime = 1.0/60.0/10.0 * 1000;             // Delta time used for physics steps, in milliseconds
static double currentTime = 0.0;                            // Current time in milliseconds
static uint64_t frequency = 0;                              // Hi-res clock frequency

static double accumulator = 0.0;                            // Physics time step delta time accumulator
static unsigned int stepsCount = 0;                         // Total physics steps processed
static vec2s gravityForce = { 0.0f, 9.81f };              // Physics world gravity force
static PhysicsBody bodies[PHYSAC_MAX_BODIES];               // Physics bodies pointers array
static unsigned int physicsBodiesCount = 0;                 // Physics world current bodies counter
static PhysicsManifold contacts[PHYSAC_MAX_MANIFOLDS];      // Physics bodies pointers array
static unsigned int physicsManifoldsCount = 0;              // Physics world current manifolds counter

//----------------------------------------------------------------------------------
// Module Internal Functions Declaration
//----------------------------------------------------------------------------------
static int FindAvailableBodyIndex();                                                                        // Finds a valid index for a new physics body initialization
static PolygonData CreateRandomPolygon(float radius, int sides);                                            // Creates a random polygon shape with max vertex distance from polygon pivot
static PolygonData CreateRectanglePolygon(vec2s pos, vec2s size);                                       // Creates a rectangle polygon shape based on a min and max positions
static void *PhysicsLoop(void *arg);                                                                        // Physics loop thread function
static void PhysicsStep(void);                                                                              // Physics steps calculations (dynamics, collisions and position corrections)
static int FindAvailableManifoldIndex();                                                                    // Finds a valid index for a new manifold initialization
static PhysicsManifold CreatePhysicsManifold(PhysicsBody a, PhysicsBody b);                                 // Creates a new physics manifold to solve collision
static void DestroyPhysicsManifold(PhysicsManifold manifold);                                               // Unitializes and destroys a physics manifold
static void SolvePhysicsManifold(PhysicsManifold manifold);                                                 // Solves a created physics manifold between two physics bodies
static void SolveCircleToCircle(PhysicsManifold manifold);                                                  // Solves collision between two circle shape physics bodies
static void SolveCircleToPolygon(PhysicsManifold manifold);                                                 // Solves collision between a circle to a polygon shape physics bodies
static void SolvePolygonToCircle(PhysicsManifold manifold);                                                 // Solves collision between a polygon to a circle shape physics bodies
static void SolveDifferentShapes(PhysicsManifold manifold, PhysicsBody bodyA, PhysicsBody bodyB);           // Solve collision between two different types of shapes
static void SolvePolygonToPolygon(PhysicsManifold manifold);                                                // Solves collision between two polygons shape physics bodies
static void IntegratePhysicsForces(PhysicsBody body);                                                       // Integrates physics forces into velocity
static void InitializePhysicsManifolds(PhysicsManifold manifold);                                           // Initializes physics manifolds to solve collisions
static void IntegratePhysicsImpulses(PhysicsManifold manifold);                                             // Integrates physics collisions impulses to solve collisions
static void IntegratePhysicsVelocity(PhysicsBody body);                                                     // Integrates physics velocity into position and forces
static void CorrectPhysicsPositions(PhysicsManifold manifold);                                              // Corrects physics bodies positions based on manifolds collision information
static float FindAxisLeastPenetration(int *faceIndex, PhysicsShape shapeA, PhysicsShape shapeB);            // Finds polygon shapes axis least penetration
static void FindIncidentFace(vec2s *v0, vec2s *v1, PhysicsShape ref, PhysicsShape inc, int index);      // Finds two polygon shapes incident face
static int Clip(vec2s normal, float clip, vec2s *faceA, vec2s *faceB);                                // Calculates clipping based on a normal and two faces
static bool BiasGreaterThan(float valueA, float valueB);                                                    // Check if values are between bias range
static vec2s TriangleBarycenter(vec2s v1, vec2s v2, vec2s v3);                                      // Returns the barycenter of a triangle given by 3 points

static void InitTimer(void);                                                                                // Initializes hi-resolution MONOTONIC timer
static uint64_t GetTimeCount(void);                                                                         // Get hi-res MONOTONIC time measure in mseconds
static double GetCurrentTime(void);                                                                         // Get current time measure in milliseconds

static mat2s glm_mat2_radians(float radians);                                                                     // Creates a matrix 2x2 from a given radians value
static void glm_mat2_radians_set(mat2 *matrix, float radians);                                                           // Set values from radians to a created matrix 2x2
static float glm_vec2_len_sqr(vec2 v);
static float glm_vec2_dist_sqr(vec2 v1, vec2 v2);

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
// Initializes physics values, pointers and creates physics loop thread
PHYSACDEF void InitPhysics(void) {
  pthread_create(&physicsThreadId, NULL, &PhysicsLoop, NULL);
  InitTimer();
  accumulator = 0.0;
}

PHYSACDEF bool IsPhysicsEnabled(void) {
  return physicsThreadEnabled;
}

PHYSACDEF void SetPhysicsGravity(float x, float y) {
  gravityForce.x = x;
  gravityForce.y = y;
}

PHYSACDEF PhysicsBody CreatePhysicsBodyCircle(vec2s pos, float radius, float density) {
  PhysicsBody newBody = (PhysicsBody)malloc(sizeof(PhysicsBodyData));
  usedMemory += sizeof(PhysicsBodyData);
  int newId = FindAvailableBodyIndex();
  if (newId != -1) {
    newBody->id = newId;
    newBody->enabled = true;
    newBody->position = pos;
    newBody->velocity = PHYSAC_VECTOR_ZERO;
    newBody->force = PHYSAC_VECTOR_ZERO;
    newBody->angularVelocity = 0.0f;
    newBody->torque = 0.0f;
    newBody->orient = 0.0f;
    newBody->shape.type = PHYSICS_CIRCLE;
    newBody->shape.body = newBody;
    newBody->shape.radius = radius;
    newBody->shape.transform = glm_mat2_radians(0.0f);
    newBody->shape.vertexData = (PolygonData){ 0 };
    newBody->mass = PHYSAC_PI * radius * radius * density;
    newBody->inverseMass = ((newBody->mass != 0.0f) ? 1.0f / newBody->mass : 0.0f);
    newBody->inertia = newBody->mass*radius*radius;
    newBody->inverseInertia = ((newBody->inertia != 0.0f) ? 1.0f / newBody->inertia : 0.0f);
    newBody->staticFriction = 0.4f;
    newBody->dynamicFriction = 0.2f;
    newBody->restitution = 0.0f;
    newBody->useGravity = true;
    newBody->isGrounded = false;
    newBody->freezeOrient = false;
    bodies[physicsBodiesCount] = newBody;
    physicsBodiesCount++;
  }
  return newBody;
}

PHYSACDEF PhysicsBody CreatePhysicsBodyRectangle(vec2s pos, float width, float height, float density) {
  PhysicsBody newBody = (PhysicsBody)malloc(sizeof(PhysicsBodyData));
  usedMemory += sizeof(PhysicsBodyData);
  int newId = FindAvailableBodyIndex();
  if (newId != -1) {
    newBody->id = newId;
    newBody->enabled = true;
    newBody->position = pos;
    newBody->velocity = (vec2s){ 0.0f };
    newBody->force = (vec2s){ 0.0f };
    newBody->angularVelocity = 0.0f;
    newBody->torque = 0.0f;
    newBody->orient = 0.0f;
    newBody->shape.type = PHYSICS_POLYGON;
    newBody->shape.body = newBody;
    newBody->shape.radius = 0.0f;
    newBody->shape.transform = glm_mat2_radians(0.0f);
    newBody->shape.vertexData = CreateRectanglePolygon(pos, (vec2s){ width, height });
    vec2s center = { 0.0f, 0.0f };
    float area = 0.0f;
    float inertia = 0.0f;
    for (int i = 0; i < newBody->shape.vertexData.vertexCount; i++) {
      vec2s p1 = newBody->shape.vertexData.positions[i];
      int nextIndex = (((i + 1) < newBody->shape.vertexData.vertexCount) ? (i + 1) : 0);
      vec2s p2 = newBody->shape.vertexData.positions[nextIndex];
      float D = glm_vec2_cross(p1.raw, p2.raw);
      float triangleArea = D/2;
      area += triangleArea;
      center.x += triangleArea * PHYSAC_K * (p1.x + p2.x);
      center.y += triangleArea * PHYSAC_K * (p1.y + p2.y);
      float intx2 = p1.x * p1.x + p2.x * p1.x + p2.x * p2.x;
      float inty2 = p1.y * p1.y + p2.y * p1.y + p2.y * p2.y;
      inertia += (0.25f * PHYSAC_K * D) * (intx2 + inty2);
    }
    center.x *= 1.0f / area;
    center.y *= 1.0f / area;
    for (int i = 0; i < newBody->shape.vertexData.vertexCount; i++) {
      newBody->shape.vertexData.positions[i].x -= center.x;
      newBody->shape.vertexData.positions[i].y -= center.y;
    }
    newBody->mass = density*area;
    newBody->inverseMass = ((newBody->mass != 0.0f) ? 1.0f / newBody->mass : 0.0f);
    newBody->inertia = density*inertia;
    newBody->inverseInertia = ((newBody->inertia != 0.0f) ? 1.0f / newBody->inertia : 0.0f);
    newBody->staticFriction = 0.4f;
    newBody->dynamicFriction = 0.2f;
    newBody->restitution = 0.0f;
    newBody->useGravity = true;
    newBody->isGrounded = false;
    newBody->freezeOrient = false;
    bodies[physicsBodiesCount] = newBody;
    physicsBodiesCount++;
  }
  return newBody;
}

PHYSACDEF PhysicsBody CreatePhysicsBodyPolygon(vec2s pos, float radius, int sides, float density) {
  PhysicsBody newBody = (PhysicsBody)malloc(sizeof(PhysicsBodyData));
  usedMemory += sizeof(PhysicsBodyData);
  int newId = FindAvailableBodyIndex();
  if (newId != -1) {
    newBody->id = newId;
    newBody->enabled = true;
    newBody->position = pos;
    newBody->velocity = PHYSAC_VECTOR_ZERO;
    newBody->force = PHYSAC_VECTOR_ZERO;
    newBody->angularVelocity = 0.0f;
    newBody->torque = 0.0f;
    newBody->orient = 0.0f;
    newBody->shape.type = PHYSICS_POLYGON;
    newBody->shape.body = newBody;
    newBody->shape.transform = glm_mat2_radians(0.0f);
    newBody->shape.vertexData = CreateRandomPolygon(radius, sides);
    vec2s center = { 0.0f, 0.0f };
    float area = 0.0f;
    float inertia = 0.0f;
    for (int i = 0; i < newBody->shape.vertexData.vertexCount; i++) {
      vec2s position1 = newBody->shape.vertexData.positions[i];
      int nextIndex = (((i + 1) < newBody->shape.vertexData.vertexCount) ? (i + 1) : 0);
      vec2s position2 = newBody->shape.vertexData.positions[nextIndex];
      float cross = glm_vec2_cross(position1.raw, position2.raw);
      float triangleArea = cross/2;
      area += triangleArea;
      center.x += triangleArea * PHYSAC_K * (position1.x + position2.x);
      center.y += triangleArea * PHYSAC_K * (position1.y + position2.y);
      float intx2 = position1.x * position1.x + position2.x * position1.x + position2.x * position2.x;
      float inty2 = position1.y * position1.y + position2.y * position1.y + position2.y * position2.y;
      inertia += (0.25f * PHYSAC_K * cross) * (intx2 + inty2);
    }
    center.x *= 1.0f / area;
    center.y *= 1.0f / area;
    for (int i = 0; i < newBody->shape.vertexData.vertexCount; i++) {
      newBody->shape.vertexData.positions[i].x -= center.x;
      newBody->shape.vertexData.positions[i].y -= center.y;
    }
    newBody->mass = density * area;
    newBody->inverseMass = ((newBody->mass != 0.0f) ? 1.0f / newBody->mass : 0.0f);
    newBody->inertia = density * inertia;
    newBody->inverseInertia = ((newBody->inertia != 0.0f) ? 1.0f / newBody->inertia : 0.0f);
    newBody->staticFriction = 0.4f;
    newBody->dynamicFriction = 0.2f;
    newBody->restitution = 0.0f;
    newBody->useGravity = true;
    newBody->isGrounded = false;
    newBody->freezeOrient = false;
    bodies[physicsBodiesCount] = newBody;
    physicsBodiesCount++;
  }
  return newBody;
}

PHYSACDEF void PhysicsAddForce(PhysicsBody body, vec2s force) {
  if (body != NULL) {
    glm_vec2_add(body->force.raw, force.raw, body->force.raw);
  }
}

PHYSACDEF void PhysicsAddTorque(PhysicsBody body, float amount) {
  if (body != NULL) {
    body->torque += amount;
  }
}

PHYSACDEF void PhysicsShatter(PhysicsBody body, vec2s position, float force) {
  if (body != NULL) {
    if (body->shape.type == PHYSICS_POLYGON) {
      PolygonData vertexData = body->shape.vertexData;
      bool collision = false;
      for (int i = 0; i < vertexData.vertexCount; i++) {
        vec2s positionA = body->position, positionB, positionC, temp;
        glm_vec2_add(body->position.raw, vertexData.positions[i].raw, temp.raw);
        glm_mat2_mulv(body->shape.transform.raw, temp.raw, positionB.raw);
        int nextIndex = (((i + 1) < vertexData.vertexCount) ? (i + 1) : 0);
        glm_vec2_add(body->position.raw, vertexData.positions[nextIndex].raw, temp.raw);
        glm_mat2_mulv(body->shape.transform.raw, temp.raw, positionC.raw);
        float alpha = ((positionB.y - positionC.y) * (position.x - positionC.x) + (positionC.x - positionB.x) * (position.y - positionC.y)) / ((positionB.y - positionC.y) * (positionA.x - positionC.x) + (positionC.x - positionB.x) * (positionA.y - positionC.y));
        float beta = ((positionC.y - positionA.y) * (position.x - positionC.x) + (positionA.x - positionC.x) * (position.y - positionC.y)) / ((positionB.y - positionC.y) * (positionA.x - positionC.x) + (positionC.x - positionB.x) * (positionA.y - positionC.y));
        float gamma = 1.0f - alpha - beta;
        if ((alpha > 0.0f) && (beta > 0.0f) && (gamma > 0.0f)) {
          collision = true;
          break;
        }
      }
      if (collision) {
        int count = vertexData.vertexCount;
        vec2s bodyPos = body->position;
        vec2s *vertices = (vec2s*)malloc(sizeof(vec2s) * count);
        mat2s trans = body->shape.transform;
        for (int i = 0; i < count; i++) {
          vertices[i] = vertexData.positions[i];
        }
        DestroyPhysicsBody(body);
        for (int i = 0; i < count; i++) {
          int nextIndex = (((i + 1) < count) ? (i + 1) : 0);
          vec2s center = TriangleBarycenter(vertices[i], vertices[nextIndex], PHYSAC_VECTOR_ZERO), offset;
          glm_vec2_add(bodyPos.raw, center.raw, center.raw);
          glm_vec2_sub(center.raw, bodyPos.raw, offset.raw);
          PhysicsBody newBody = CreatePhysicsBodyPolygon(center, 10, 3, 10);
          PolygonData newData = { 0 };
          newData.vertexCount = 3;
          glm_vec2_sub(vertices[i].raw, offset.raw, newData.positions[0].raw);
          glm_vec2_sub(vertices[nextIndex].raw, offset.raw, newData.positions[1].raw);
          glm_vec2_sub(position.raw, center.raw, newData.positions[2].raw);
          newData.positions[0].x *= 0.95f;
          newData.positions[0].y *= 0.95f;
          newData.positions[1].x *= 0.95f;
          newData.positions[1].y *= 0.95f;
          newData.positions[2].x *= 0.95f;
          newData.positions[2].y *= 0.95f;
          for (int j = 0; j < newData.vertexCount; j++) {
            int nextVertex = (((j + 1) < newData.vertexCount) ? (j + 1) : 0);
            vec2s face;
            glm_vec2_sub(newData.positions[nextVertex].raw, newData.positions[j].raw, face.raw);
            newData.normals[j] = (vec2s){ face.y, -face.x };
            glm_vec2_norm2(newData.normals[j].raw);
          }
          newBody->shape.vertexData = newData;
          newBody->shape.transform = trans;
          center = PHYSAC_VECTOR_ZERO;
          float area = 0.0f;
          float inertia = 0.0f;
          for (int j = 0; j < newBody->shape.vertexData.vertexCount; j++) {
            vec2s p1 = newBody->shape.vertexData.positions[j];
            int nextVertex = (((j + 1) < newBody->shape.vertexData.vertexCount) ? (j + 1) : 0);
            vec2s p2 = newBody->shape.vertexData.positions[nextVertex];
            float D = glm_vec2_cross(p1.raw, p2.raw);
            float triangleArea = D/2;
            area += triangleArea;
            center.x += triangleArea * PHYSAC_K * (p1.x + p2.x);
            center.y += triangleArea * PHYSAC_K * (p1.y + p2.y);
            float intx2 = p1.x * p1.x + p2.x * p1.x + p2.x * p2.x;
            float inty2 = p1.y * p1.y + p2.y * p1.y + p2.y * p2.y;
            inertia += (0.25f * PHYSAC_K * D) * (intx2 + inty2);
          }
          center.x *= 1.0f/area;
          center.y *= 1.0f/area;
          newBody->mass = area;
          newBody->inverseMass = ((newBody->mass != 0.0f) ? 1.0f/newBody->mass : 0.0f);
          newBody->inertia = inertia;
          newBody->inverseInertia = ((newBody->inertia != 0.0f) ? 1.0f/newBody->inertia : 0.0f);
          vec2s pointA = newBody->position, pointB, forceDirection, temp, temp2;
          glm_vec2_sub(newData.positions[1].raw, newData.positions[0].raw, pointB.raw);
          pointB.x /= 2.0f;
          pointB.y /= 2.0f;
          glm_vec2_add(newData.positions[0].raw, pointB.raw, temp.raw);
          glm_vec2_add(pointA.raw, temp.raw, temp2.raw);
          glm_vec2_sub(temp2.raw, newBody->position.raw, forceDirection.raw);
          glm_vec2_norm2(forceDirection.raw);
          forceDirection.x *= force;
          forceDirection.y *= force;
          PhysicsAddForce(newBody, forceDirection);
        }
        free(vertices);
      }
    }
  }
}

PHYSACDEF int GetPhysicsBodiesCount(void) {
  return physicsBodiesCount;
}

PHYSACDEF PhysicsBody GetPhysicsBody(int index) {
  return bodies[index];
}

PHYSACDEF int GetPhysicsShapeType(int index) {
  int result = -1;
  if (index < physicsBodiesCount) {
    if (bodies[index] != NULL) {
      result = bodies[index]->shape.type;
    }
  }
  return result;
}

PHYSACDEF int GetPhysicsShapeVerticesCount(int index) {
  int result = 0;
  if (index < physicsBodiesCount) {
    if (bodies[index] != NULL) {
      switch (bodies[index]->shape.type) {
        case PHYSICS_CIRCLE: 
          result = PHYSAC_CIRCLE_VERTICES; 
          break;
        case PHYSICS_POLYGON: 
          result = bodies[index]->shape.vertexData.vertexCount; 
          break;
        default: 
          break;
      }
    }
  }
  return result;
}

PHYSACDEF vec2s GetPhysicsShapeVertex(PhysicsBody body, int vertex) {
  vec2s position = { 0.0f, 0.0f };
  if (body != NULL) {
    switch (body->shape.type) {
      case PHYSICS_CIRCLE: {
        position.x = body->position.x + cosf(360.0f/PHYSAC_CIRCLE_VERTICES*vertex*PHYSAC_DEG2RAD)*body->shape.radius;
        position.y = body->position.y + sinf(360.0f/PHYSAC_CIRCLE_VERTICES*vertex*PHYSAC_DEG2RAD)*body->shape.radius;
        break;
      }
      case PHYSICS_POLYGON: {
        PolygonData vertexData = body->shape.vertexData;
        vec2s temp;
        glm_mat2_mulv(body->shape.transform.raw, vertexData.positions[vertex].raw, temp.raw);
        glm_vec2_add(body->position.raw, temp.raw, position.raw);
        break;
      }
      default: 
        break;
    }
  }
  return position;
}

PHYSACDEF void SetPhysicsBodyRotation(PhysicsBody body, float radians) {
  if (body != NULL) {
    body->orient = radians;
    if (body->shape.type == PHYSICS_POLYGON) {
      body->shape.transform = glm_mat2_radians(radians);
    }
  }
}

PHYSACDEF void DestroyPhysicsBody(PhysicsBody body) {
  if (body != NULL) {
    int id = body->id;
    int index = -1;
    for (int i = 0; i < physicsBodiesCount; i++) {
      if (bodies[i]->id == id) {
        index = i;
        break;
      }
    }
    free(body);
    usedMemory -= sizeof(PhysicsBodyData);
    bodies[index] = NULL;
    for (int i = index; i < physicsBodiesCount; i++) {
      if ((i + 1) < physicsBodiesCount) {
        bodies[i] = bodies[i + 1];
      }
    }
    physicsBodiesCount--;
  }
}

PHYSACDEF void ClosePhysics(void) {
  physicsThreadEnabled = false;
  pthread_join(physicsThreadId, NULL);
  for (int i = physicsManifoldsCount - 1; i >= 0; i--) {
    DestroyPhysicsManifold(contacts[i]);
  }
  for (int i = physicsBodiesCount - 1; i >= 0; i--) {
    DestroyPhysicsBody(bodies[i]);
  }
}

static int FindAvailableBodyIndex() {
  int index = -1;
  for (int i = 0; i < PHYSAC_MAX_BODIES; i++) {
    int currentId = i;
    for (int k = 0; k < physicsBodiesCount; k++) {
      if (bodies[k]->id == currentId) {
        currentId++;
        break;
      }
    }
    if (currentId == i) {
      index = i;
      break;
    }
  }
  return index;
}

static PolygonData CreateRandomPolygon(float radius, int sides) {
  PolygonData data = { 0 };
  data.vertexCount = sides;
  for (int i = 0; i < data.vertexCount; i++) {
    data.positions[i].x = cosf(360.0f/sides * i * PHYSAC_DEG2RAD) * radius;
    data.positions[i].y = sinf(360.0f/sides * i * PHYSAC_DEG2RAD) * radius;
  }
  for (int i = 0; i < data.vertexCount; i++) {
    int nextIndex = (((i + 1) < sides) ? (i + 1) : 0);
    vec2s face;
    glm_vec2_sub(data.positions[nextIndex].raw, data.positions[i].raw, face.raw);
    data.normals[i] = (vec2s){ face.y, -face.x };
    glm_vec2_norm2(data.normals[i].raw);
  }
  return data;
}

static PolygonData CreateRectanglePolygon(vec2s pos, vec2s size) {
  PolygonData data = { 0 };
  data.vertexCount = 4;
  data.positions[0] = (vec2s){ pos.x + size.x/2, pos.y - size.y/2 };
  data.positions[1] = (vec2s){ pos.x + size.x/2, pos.y + size.y/2 };
  data.positions[2] = (vec2s){ pos.x - size.x/2, pos.y + size.y/2 };
  data.positions[3] = (vec2s){ pos.x - size.x/2, pos.y - size.y/2 };
  for (int i = 0; i < data.vertexCount; i++) {
    int nextIndex = (((i + 1) < data.vertexCount) ? (i + 1) : 0);
    vec2s face;
    glm_vec2_sub(data.positions[nextIndex].raw, data.positions[i].raw, face.raw);
    data.normals[i] = (vec2s){ face.y, -face.x };
    glm_vec2_norm2(data.normals[i].raw);
  }
  return data;
}

static void *PhysicsLoop(void *arg) {
  physicsThreadEnabled = true;
  while (physicsThreadEnabled) {
    RunPhysicsStep();
  }
  return NULL;
}

static void PhysicsStep(void) {
  stepsCount++;
  for (int i = physicsManifoldsCount - 1; i >= 0; i--) {
    PhysicsManifold manifold = contacts[i];
    if (manifold != NULL) {
      DestroyPhysicsManifold(manifold);
    }
  }
  for (int i = 0; i < physicsBodiesCount; i++) {
    PhysicsBody body = bodies[i];
    body->isGrounded = false;
  }
  for (int i = 0; i < physicsBodiesCount; i++) {
    PhysicsBody bodyA = bodies[i];
    if (bodyA != NULL) {
      for (int j = i + 1; j < physicsBodiesCount; j++) {
        PhysicsBody bodyB = bodies[j];
        if (bodyB != NULL) {
          if ((bodyA->inverseMass == 0) && (bodyB->inverseMass == 0)) {
            continue;
          }
          PhysicsManifold manifold = CreatePhysicsManifold(bodyA, bodyB);
          SolvePhysicsManifold(manifold);
          if (manifold->contactsCount > 0) {
            PhysicsManifold newManifold = CreatePhysicsManifold(bodyA, bodyB);
            newManifold->penetration = manifold->penetration;
            newManifold->normal = manifold->normal;
            newManifold->contacts[0] = manifold->contacts[0];
            newManifold->contacts[1] = manifold->contacts[1];
            newManifold->contactsCount = manifold->contactsCount;
            newManifold->restitution = manifold->restitution;
            newManifold->dynamicFriction = manifold->dynamicFriction;
            newManifold->staticFriction = manifold->staticFriction;
          }
        }
      }
    }
  }
  for (int i = 0; i < physicsBodiesCount; i++) {
    PhysicsBody body = bodies[i];
    if (body != NULL) {
      IntegratePhysicsForces(body);
    }
  }
  for (int i = 0; i < physicsManifoldsCount; i++) {
    PhysicsManifold manifold = contacts[i];
    if (manifold != NULL) {
      InitializePhysicsManifolds(manifold);
    }
  }
  for (int i = 0; i < PHYSAC_COLLISION_ITERATIONS; i++) {
    for (int j = 0; j < physicsManifoldsCount; j++) {
      PhysicsManifold manifold = contacts[i];
      if (manifold != NULL) {
        IntegratePhysicsImpulses(manifold);
      }
    }
  }
  for (int i = 0; i < physicsBodiesCount; i++) {
    PhysicsBody body = bodies[i];
    if (body != NULL) {
      IntegratePhysicsVelocity(body);
    }
  }
  for (int i = 0; i < physicsManifoldsCount; i++) {
    PhysicsManifold manifold = contacts[i];
    if (manifold != NULL) {
      CorrectPhysicsPositions(manifold);
    }
  }
  for (int i = 0; i < physicsBodiesCount; i++) {
    PhysicsBody body = bodies[i];
    if (body != NULL) {
      body->force = PHYSAC_VECTOR_ZERO;
      body->torque = 0.0f;
    }
  }
}

PHYSACDEF void RunPhysicsStep(void) {
  currentTime = GetCurrentTime();
  const double delta = currentTime - startTime;
  accumulator += delta;
  while (accumulator >= deltaTime) {
    PhysicsStep();
    accumulator -= deltaTime;
  }
  startTime = currentTime;
}

PHYSACDEF void SetPhysicsTimeStep(double delta) {
  deltaTime = delta;
}

static int FindAvailableManifoldIndex() {
  int index = -1;
  for (int i = 0; i < PHYSAC_MAX_MANIFOLDS; i++) {
    int currentId = i;
    for (int k = 0; k < physicsManifoldsCount; k++) {
      if (contacts[k]->id == currentId) {
        currentId++;
        break;
      }
    }
    if (currentId == i) {
      index = i;
      break;
    }
  }
  return index;
}

static PhysicsManifold CreatePhysicsManifold(PhysicsBody a, PhysicsBody b) {
  PhysicsManifold newManifold = (PhysicsManifold)malloc(sizeof(PhysicsManifoldData));
  usedMemory += sizeof(PhysicsManifoldData);
  int newId = FindAvailableManifoldIndex();
  if (newId != -1) {
    newManifold->id = newId;
    newManifold->bodyA = a;
    newManifold->bodyB = b;
    newManifold->penetration = 0;
    newManifold->normal = PHYSAC_VECTOR_ZERO;
    newManifold->contacts[0] = PHYSAC_VECTOR_ZERO;
    newManifold->contacts[1] = PHYSAC_VECTOR_ZERO;
    newManifold->contactsCount = 0;
    newManifold->restitution = 0.0f;
    newManifold->dynamicFriction = 0.0f;
    newManifold->staticFriction = 0.0f;
    contacts[physicsManifoldsCount] = newManifold;
    physicsManifoldsCount++;
  }
  return newManifold;
}

static void DestroyPhysicsManifold(PhysicsManifold manifold) {
  if (manifold != NULL) {
    int id = manifold->id;
    int index = -1;
    for (int i = 0; i < physicsManifoldsCount; i++) {
      if (contacts[i]->id == id) {
        index = i;
        break;
      }
    }
    if (index == -1) {
      return;
    }      
    free(manifold);
    usedMemory -= sizeof(PhysicsManifoldData);
    contacts[index] = NULL;
    for (int i = index; i < physicsManifoldsCount; i++) {
      if ((i + 1) < physicsManifoldsCount) {
        contacts[i] = contacts[i + 1];
      }
    }
    physicsManifoldsCount--;
  }
}

static void SolvePhysicsManifold(PhysicsManifold manifold) {
  switch (manifold->bodyA->shape.type) {
    case PHYSICS_CIRCLE: {
      switch (manifold->bodyB->shape.type) {
        case PHYSICS_CIRCLE: 
          SolveCircleToCircle(manifold); 
          break;
        case PHYSICS_POLYGON: 
          SolveCircleToPolygon(manifold); 
          break;
        default: 
          break;
      }
      break;
    }
    case PHYSICS_POLYGON: {
      switch (manifold->bodyB->shape.type) {
        case PHYSICS_CIRCLE: 
          SolvePolygonToCircle(manifold); 
          break;
        case PHYSICS_POLYGON: 
          SolvePolygonToPolygon(manifold); 
          break;
        default: 
        break;
      }
      break;
    }
    default: 
      break;
  }
  if (!manifold->bodyB->isGrounded) {
    manifold->bodyB->isGrounded = (manifold->normal.y < 0);
  }
}

static void SolveCircleToCircle(PhysicsManifold manifold) {
  PhysicsBody bodyA = manifold->bodyA;
  PhysicsBody bodyB = manifold->bodyB;
  if ((bodyA == NULL) || (bodyB == NULL)) {
    return;
  }
  vec2s normal;
  glm_vec2_sub(bodyB->position.raw, bodyA->position.raw, normal.raw);
  float distSqr = glm_vec2_len_sqr(normal.raw);
  float radius = bodyA->shape.radius + bodyB->shape.radius;
  if (distSqr >= radius*radius) {
    manifold->contactsCount = 0;
    return;
  }
  float distance = sqrtf(distSqr);
  manifold->contactsCount = 1;
  if (distance == 0.0f) {
    manifold->penetration = bodyA->shape.radius;
    manifold->normal = (vec2s){ 1.0f, 0.0f };
    manifold->contacts[0] = bodyA->position;
  }
  else {
    manifold->penetration = radius - distance;
    manifold->normal = (vec2s){ normal.x/distance, normal.y/distance };
    manifold->contacts[0] = (vec2s){ manifold->normal.x*bodyA->shape.radius + bodyA->position.x, manifold->normal.y*bodyA->shape.radius + bodyA->position.y };
  }
  if (!bodyA->isGrounded) {
    bodyA->isGrounded = (manifold->normal.y < 0);
  }
}

static void SolveCircleToPolygon(PhysicsManifold manifold) {
  PhysicsBody bodyA = manifold->bodyA;
  PhysicsBody bodyB = manifold->bodyB;
  if ((bodyA == NULL) || (bodyB == NULL)) {
    return;
  }
  SolveDifferentShapes(manifold, bodyA, bodyB);
}

static void SolvePolygonToCircle(PhysicsManifold manifold) {
  PhysicsBody bodyA = manifold->bodyA;
  PhysicsBody bodyB = manifold->bodyB;
  if ((bodyA == NULL) || (bodyB == NULL)) {
    return;
  }
  SolveDifferentShapes(manifold, bodyB, bodyA);
  manifold->normal.x *= -1.0f;
  manifold->normal.y *= -1.0f;
}

static void SolveDifferentShapes(PhysicsManifold manifold, PhysicsBody bodyA, PhysicsBody bodyB) {
  manifold->contactsCount = 0;
  vec2s center = bodyA->position, temp;
  mat2s temp2;
  glm_mat2_transpose_to(bodyB->shape.transform.raw, temp2.raw);
  glm_vec2_sub(center.raw, bodyB->position.raw, temp.raw);
  glm_mat2_mulv(temp2.raw, temp.raw, center.raw);
  float separation = -PHYSAC_FLT_MAX;
  int faceNormal = 0;
  PolygonData vertexData = bodyB->shape.vertexData;
  for (int i = 0; i < vertexData.vertexCount; i++) {
    vec2s temp3;
    glm_vec2_sub(center.raw, vertexData.positions[i].raw, temp3.raw);
    float currentSeparation = glm_vec2_dot(vertexData.normals[i].raw, temp3.raw);
    if (currentSeparation > bodyA->shape.radius) {
      return;
    }
    if (currentSeparation > separation) {
      separation = currentSeparation;
      faceNormal = i;
    }
  }
  vec2s v1 = vertexData.positions[faceNormal];
  int nextIndex = (((faceNormal + 1) < vertexData.vertexCount) ? (faceNormal + 1) : 0);
  vec2s v2 = vertexData.positions[nextIndex];
  if (separation < PHYSAC_EPSILON) {
    manifold->contactsCount = 1;
    vec2s normal;
    glm_mat2_mulv(bodyB->shape.transform.raw, vertexData.normals[faceNormal].raw, normal.raw);
    manifold->normal = (vec2s){ -normal.x, -normal.y };
    manifold->contacts[0] = (vec2s){ manifold->normal.x * bodyA->shape.radius + bodyA->position.x, manifold->normal.y * bodyA->shape.radius + bodyA->position.y };
    manifold->penetration = bodyA->shape.radius;
    return;
  }
  vec2s temp3, temp4;
  glm_vec2_sub(center.raw, v1.raw, temp3.raw);
  glm_vec2_sub(v2.raw, v1.raw, temp4.raw);
  float dot1 = glm_vec2_dot(temp3.raw, temp4.raw);
  glm_vec2_sub(center.raw, v2.raw, temp3.raw);
  glm_vec2_sub(v1.raw, v2.raw, temp4.raw);
  float dot2 = glm_vec2_dot(temp3.raw, temp4.raw);
  manifold->penetration = bodyA->shape.radius - separation;
  if (dot1 <= 0.0f) {
    if (glm_vec2_dist_sqr(center.raw, v1.raw) > bodyA->shape.radius*bodyA->shape.radius) {
      return;
    }
    manifold->contactsCount = 1;
    vec2s normal;
    glm_vec2_sub(v1.raw, center.raw, normal.raw);
    glm_mat2_mulv(bodyB->shape.transform.raw, normal.raw, normal.raw);
    glm_vec2_norm2(normal.raw);
    manifold->normal = normal;
    glm_mat2_mulv(bodyB->shape.transform.raw, v1.raw, v1.raw);
    glm_vec2_add(v1.raw, bodyB->position.raw, v1.raw);
    manifold->contacts[0] = v1;
  }
  else if (dot2 <= 0.0f) {
    if (glm_vec2_dist_sqr(center.raw, v2.raw) > bodyA->shape.radius*bodyA->shape.radius) {
      return;
    }
    manifold->contactsCount = 1;
    vec2s normal;
    glm_vec2_sub(v2.raw, center.raw, normal.raw);
    glm_mat2_mulv(bodyB->shape.transform.raw, v2.raw, v2.raw);
    glm_vec2_add(v2.raw, bodyB->position.raw, v2.raw);
    manifold->contacts[0] = v2;
    glm_mat2_mulv(bodyB->shape.transform.raw, normal.raw, normal.raw);
    glm_vec2_norm2(normal.raw);
    manifold->normal = normal;
  }
  else {
    vec2s normal = vertexData.normals[faceNormal];
    vec2s temp5;
    glm_vec2_sub(center.raw, v1.raw, temp4.raw);
    if (glm_vec2_dot(temp5.raw, normal.raw) > bodyA->shape.radius) {
      return;
    }
    glm_mat2_mulv(bodyB->shape.transform.raw, normal.raw, normal.raw);
    manifold->normal = (vec2s){ -normal.x, -normal.y };
    manifold->contacts[0] = (vec2s){ manifold->normal.x * bodyA->shape.radius + bodyA->position.x, manifold->normal.y * bodyA->shape.radius + bodyA->position.y };
    manifold->contactsCount = 1;
  }
}

static void SolvePolygonToPolygon(PhysicsManifold manifold) {
  if ((manifold->bodyA == NULL) || (manifold->bodyB == NULL)) {
    return;
  }
  PhysicsShape bodyA = manifold->bodyA->shape;
  PhysicsShape bodyB = manifold->bodyB->shape;
  manifold->contactsCount = 0;
  int faceA = 0;
  float penetrationA = FindAxisLeastPenetration(&faceA, bodyA, bodyB);
  if (penetrationA >= 0.0f) {
    return;
  }
  int faceB = 0;
  float penetrationB = FindAxisLeastPenetration(&faceB, bodyB, bodyA);
  if (penetrationB >= 0.0f) {
    return;
  }
  int referenceIndex = 0;
  bool flip = false;
  PhysicsShape refPoly;
  PhysicsShape incPoly;
  if (BiasGreaterThan(penetrationA, penetrationB)) {
    refPoly = bodyA;
    incPoly = bodyB;
    referenceIndex = faceA;
  }
  else {
    refPoly = bodyB;
    incPoly = bodyA;
    referenceIndex = faceB;
    flip = true;
  }
  vec2s incidentFace[2];
  FindIncidentFace(&incidentFace[0], &incidentFace[1], refPoly, incPoly, referenceIndex);
  PolygonData refData = refPoly.vertexData;
  vec2s v1 = refData.positions[referenceIndex];
  referenceIndex = (((referenceIndex + 1) < refData.vertexCount) ? (referenceIndex + 1) : 0);
  vec2s v2 = refData.positions[referenceIndex];
  glm_mat2_mulv(refPoly.transform.raw, v1.raw, v1.raw);
  glm_vec2_add(v1.raw, refPoly.body->position.raw, v1.raw);
  glm_mat2_mulv(refPoly.transform.raw, v2.raw, v2.raw);
  glm_vec2_add(v2.raw, refPoly.body->position.raw, v2.raw);
  vec2s sidePlaneNormal;
  glm_vec2_sub(v2.raw, v1.raw, sidePlaneNormal.raw);
  glm_vec2_norm2(sidePlaneNormal.raw);
  vec2s refFaceNormal = { sidePlaneNormal.y, -sidePlaneNormal.x };
  float refC = glm_vec2_dot(refFaceNormal.raw, v1.raw);
  float negSide = glm_vec2_dot(sidePlaneNormal.raw, v1.raw) * -1;
  float posSide = glm_vec2_dot(sidePlaneNormal.raw, v2.raw);
  if (Clip((vec2s){ -sidePlaneNormal.x, -sidePlaneNormal.y }, negSide, &incidentFace[0], &incidentFace[1]) < 2) {
    return;
  }
  if (Clip(sidePlaneNormal, posSide, &incidentFace[0], &incidentFace[1]) < 2) {
    return;
  }
  manifold->normal = (flip ? (vec2s){ -refFaceNormal.x, -refFaceNormal.y } : refFaceNormal);
  int currentPoint = 0;
  float separation = glm_vec2_dot(refFaceNormal.raw, incidentFace[0].raw) - refC;
  if (separation <= 0.0f) {
    manifold->contacts[currentPoint] = incidentFace[0];
    manifold->penetration = -separation;
    currentPoint++;
  }
  else {
    manifold->penetration = 0.0f;
  }
  separation = glm_vec2_dot(refFaceNormal.raw, incidentFace[1].raw) - refC;
  if (separation <= 0.0f) {
    manifold->contacts[currentPoint] = incidentFace[1];
    manifold->penetration += -separation;
    currentPoint++;
    manifold->penetration /= currentPoint;
  }
  manifold->contactsCount = currentPoint;
}

static void IntegratePhysicsForces(PhysicsBody body) {
  if ((body == NULL) || (body->inverseMass == 0.0f) || !body->enabled) {
    return;
  }
  body->velocity.x += (body->force.x*body->inverseMass)*(deltaTime/2.0);
  body->velocity.y += (body->force.y*body->inverseMass)*(deltaTime/2.0);
  if (body->useGravity) {
    body->velocity.x += gravityForce.x*(deltaTime/1000/2.0);
    body->velocity.y += gravityForce.y*(deltaTime/1000/2.0);
  }
  if (!body->freezeOrient) {
    body->angularVelocity += body->torque*body->inverseInertia*(deltaTime/2.0);
  }
}

// Initializes physics manifolds to solve collisions
static void InitializePhysicsManifolds(PhysicsManifold manifold)
{
    PhysicsBody bodyA = manifold->bodyA;
    PhysicsBody bodyB = manifold->bodyB;

    if ((bodyA == NULL) || (bodyB == NULL))
        return;

    // Calculate average restitution, static and dynamic friction
    manifold->restitution = sqrtf(bodyA->restitution*bodyB->restitution);
    manifold->staticFriction = sqrtf(bodyA->staticFriction*bodyB->staticFriction);
    manifold->dynamicFriction = sqrtf(bodyA->dynamicFriction*bodyB->dynamicFriction);

    for (int i = 0; i < manifold->contactsCount; i++)
    {
        // Caculate radius from center of mass to contact
        Vector2 radiusA = Vector2Subtract(manifold->contacts[i], bodyA->position);
        Vector2 radiusB = Vector2Subtract(manifold->contacts[i], bodyB->position);

        Vector2 crossA = MathCross(bodyA->angularVelocity, radiusA);
        Vector2 crossB = MathCross(bodyB->angularVelocity, radiusB);

        Vector2 radiusV = { 0.0f, 0.0f };
        radiusV.x = bodyB->velocity.x + crossB.x - bodyA->velocity.x - crossA.x;
        radiusV.y = bodyB->velocity.y + crossB.y - bodyA->velocity.y - crossA.y;

        // Determine if we should perform a resting collision or not;
        // The idea is if the only thing moving this object is gravity, then the collision should be performed without any restitution
        if (MathLenSqr(radiusV) < (MathLenSqr((Vector2){ gravityForce.x*deltaTime/1000, gravityForce.y*deltaTime/1000 }) + PHYSAC_EPSILON))
            manifold->restitution = 0;
    }
}

// Integrates physics collisions impulses to solve collisions
static void IntegratePhysicsImpulses(PhysicsManifold manifold)
{
    PhysicsBody bodyA = manifold->bodyA;
    PhysicsBody bodyB = manifold->bodyB;

    if ((bodyA == NULL) || (bodyB == NULL))
        return;

    // Early out and positional correct if both objects have infinite mass
    if (fabs(bodyA->inverseMass + bodyB->inverseMass) <= PHYSAC_EPSILON)
    {
        bodyA->velocity = PHYSAC_VECTOR_ZERO;
        bodyB->velocity = PHYSAC_VECTOR_ZERO;
        return;
    }

    for (int i = 0; i < manifold->contactsCount; i++)
    {
        // Calculate radius from center of mass to contact
        Vector2 radiusA = Vector2Subtract(manifold->contacts[i], bodyA->position);
        Vector2 radiusB = Vector2Subtract(manifold->contacts[i], bodyB->position);

        // Calculate relative velocity
        Vector2 radiusV = { 0.0f, 0.0f };
        radiusV.x = bodyB->velocity.x + MathCross(bodyB->angularVelocity, radiusB).x - bodyA->velocity.x - MathCross(bodyA->angularVelocity, radiusA).x;
        radiusV.y = bodyB->velocity.y + MathCross(bodyB->angularVelocity, radiusB).y - bodyA->velocity.y - MathCross(bodyA->angularVelocity, radiusA).y;

        // Relative velocity along the normal
        float contactVelocity = MathDot(radiusV, manifold->normal);

        // Do not resolve if velocities are separating
        if (contactVelocity > 0.0f)
            return;

        float raCrossN = MathCrossVector2(radiusA, manifold->normal);
        float rbCrossN = MathCrossVector2(radiusB, manifold->normal);

        float inverseMassSum = bodyA->inverseMass + bodyB->inverseMass + (raCrossN*raCrossN)*bodyA->inverseInertia + (rbCrossN*rbCrossN)*bodyB->inverseInertia;

        // Calculate impulse scalar value
        float impulse = -(1.0f + manifold->restitution)*contactVelocity;
        impulse /= inverseMassSum;
        impulse /= (float)manifold->contactsCount;

        // Apply impulse to each physics body
        Vector2 impulseV = { manifold->normal.x*impulse, manifold->normal.y*impulse };

        if (bodyA->enabled)
        {
            bodyA->velocity.x += bodyA->inverseMass*(-impulseV.x);
            bodyA->velocity.y += bodyA->inverseMass*(-impulseV.y);
            
            if (!bodyA->freezeOrient)
                bodyA->angularVelocity += bodyA->inverseInertia*MathCrossVector2(radiusA, (Vector2){ -impulseV.x, -impulseV.y });
        }

        if (bodyB->enabled)
        {
            bodyB->velocity.x += bodyB->inverseMass*(impulseV.x);
            bodyB->velocity.y += bodyB->inverseMass*(impulseV.y);
            
            if (!bodyB->freezeOrient)
                bodyB->angularVelocity += bodyB->inverseInertia*MathCrossVector2(radiusB, impulseV);
        }

        // Apply friction impulse to each physics body
        radiusV.x = bodyB->velocity.x + MathCross(bodyB->angularVelocity, radiusB).x - bodyA->velocity.x - MathCross(bodyA->angularVelocity, radiusA).x;
        radiusV.y = bodyB->velocity.y + MathCross(bodyB->angularVelocity, radiusB).y - bodyA->velocity.y - MathCross(bodyA->angularVelocity, radiusA).y;

        Vector2 tangent = { radiusV.x - (manifold->normal.x*MathDot(radiusV, manifold->normal)), radiusV.y - (manifold->normal.y*MathDot(radiusV, manifold->normal)) };
        MathNormalize(&tangent);

        // Calculate impulse tangent magnitude
        float impulseTangent = -MathDot(radiusV, tangent);
        impulseTangent /= inverseMassSum;
        impulseTangent /= (float)manifold->contactsCount;

        float absImpulseTangent = fabs(impulseTangent);

        // Don't apply tiny friction impulses
        if (absImpulseTangent <= PHYSAC_EPSILON)
            return;

        // Apply coulumb's law
        Vector2 tangentImpulse = { 0.0f, 0.0f };
        if (absImpulseTangent < impulse*manifold->staticFriction)
            tangentImpulse = (Vector2){ tangent.x*impulseTangent, tangent.y*impulseTangent };
        else
            tangentImpulse = (Vector2){ tangent.x*-impulse*manifold->dynamicFriction, tangent.y*-impulse*manifold->dynamicFriction };

        // Apply friction impulse
        if (bodyA->enabled)
        {
            bodyA->velocity.x += bodyA->inverseMass*(-tangentImpulse.x);
            bodyA->velocity.y += bodyA->inverseMass*(-tangentImpulse.y);

            if (!bodyA->freezeOrient)
                bodyA->angularVelocity += bodyA->inverseInertia*MathCrossVector2(radiusA, (Vector2){ -tangentImpulse.x, -tangentImpulse.y });
        }

        if (bodyB->enabled)
        {
            bodyB->velocity.x += bodyB->inverseMass*(tangentImpulse.x);
            bodyB->velocity.y += bodyB->inverseMass*(tangentImpulse.y);

            if (!bodyB->freezeOrient)
                bodyB->angularVelocity += bodyB->inverseInertia*MathCrossVector2(radiusB, tangentImpulse);
        }
    }
}

// Integrates physics velocity into position and forces
static void IntegratePhysicsVelocity(PhysicsBody body)
{
    if ((body == NULL) ||!body->enabled)
        return;

    body->position.x += body->velocity.x*deltaTime;
    body->position.y += body->velocity.y*deltaTime;

    if (!body->freezeOrient)
        body->orient += body->angularVelocity*deltaTime;

    Mat2Set(&body->shape.transform, body->orient);

    IntegratePhysicsForces(body);
}

// Corrects physics bodies positions based on manifolds collision information
static void CorrectPhysicsPositions(PhysicsManifold manifold)
{
    PhysicsBody bodyA = manifold->bodyA;
    PhysicsBody bodyB = manifold->bodyB;

    if ((bodyA == NULL) || (bodyB == NULL))
        return;

    Vector2 correction = { 0.0f, 0.0f };
    correction.x = (max(manifold->penetration - PHYSAC_PENETRATION_ALLOWANCE, 0.0f)/(bodyA->inverseMass + bodyB->inverseMass))*manifold->normal.x*PHYSAC_PENETRATION_CORRECTION;
    correction.y = (max(manifold->penetration - PHYSAC_PENETRATION_ALLOWANCE, 0.0f)/(bodyA->inverseMass + bodyB->inverseMass))*manifold->normal.y*PHYSAC_PENETRATION_CORRECTION;

    if (bodyA->enabled)
    {
        bodyA->position.x -= correction.x*bodyA->inverseMass;
        bodyA->position.y -= correction.y*bodyA->inverseMass;
    }

    if (bodyB->enabled)
    {
        bodyB->position.x += correction.x*bodyB->inverseMass;
        bodyB->position.y += correction.y*bodyB->inverseMass;
    }
}

// Returns the extreme point along a direction within a polygon
static Vector2 GetSupport(PhysicsShape shape, Vector2 dir)
{
    float bestProjection = -PHYSAC_FLT_MAX;
    Vector2 bestVertex = { 0.0f, 0.0f };
    PolygonData data = shape.vertexData;

    for (int i = 0; i < data.vertexCount; i++)
    {
        Vector2 vertex = data.positions[i];
        float projection = MathDot(vertex, dir);

        if (projection > bestProjection)
        {
            bestVertex = vertex;
            bestProjection = projection;
        }
    }

    return bestVertex;
}

// Finds polygon shapes axis least penetration
static float FindAxisLeastPenetration(int *faceIndex, PhysicsShape shapeA, PhysicsShape shapeB)
{
    float bestDistance = -PHYSAC_FLT_MAX;
    int bestIndex = 0;

    PolygonData dataA = shapeA.vertexData;

    for (int i = 0; i < dataA.vertexCount; i++)
    {
        // Retrieve a face normal from A shape
        Vector2 normal = dataA.normals[i];
        Vector2 transNormal = Mat2MultiplyVector2(shapeA.transform, normal);

        // Transform face normal into B shape's model space
        Mat2 buT = Mat2Transpose(shapeB.transform);
        normal = Mat2MultiplyVector2(buT, transNormal);

        // Retrieve support point from B shape along -n
        Vector2 support = GetSupport(shapeB, (Vector2){ -normal.x, -normal.y });

        // Retrieve vertex on face from A shape, transform into B shape's model space
        Vector2 vertex = dataA.positions[i];
        vertex = Mat2MultiplyVector2(shapeA.transform, vertex);
        vertex = Vector2Add(vertex, shapeA.body->position);
        vertex = Vector2Subtract(vertex, shapeB.body->position);
        vertex = Mat2MultiplyVector2(buT, vertex);

        // Compute penetration distance in B shape's model space
        float distance = MathDot(normal, Vector2Subtract(support, vertex));

        // Store greatest distance
        if (distance > bestDistance)
        {
            bestDistance = distance;
            bestIndex = i;
        }
    }

    *faceIndex = bestIndex;
    return bestDistance;
}

// Finds two polygon shapes incident face
static void FindIncidentFace(vec2s *v0, vec2s *v1, PhysicsShape ref, PhysicsShape inc, int index)
{
    PolygonData refData = ref.vertexData;
    PolygonData incData = inc.vertexData;

    vec2s referenceNormal = refData.normals[index];

    // Calculate normal in incident's frame of reference
    referenceNormal = Mat2MultiplyVector2(ref.transform, referenceNormal); // To world space
    referenceNormal = Mat2MultiplyVector2(Mat2Transpose(inc.transform), referenceNormal); // To incident's model space

    // Find most anti-normal face on polygon
    int incidentFace = 0;
    float minDot = PHYSAC_FLT_MAX;

    for (int i = 0; i < incData.vertexCount; i++)
    {
        float dot = MathDot(referenceNormal, incData.normals[i]);

        if (dot < minDot)
        {
            minDot = dot;
            incidentFace = i;
        }
    }

    // Assign face vertices for incident face
    *v0 = Mat2MultiplyVector2(inc.transform, incData.positions[incidentFace]);
    *v0 = Vector2Add(*v0, inc.body->position);
    incidentFace = (((incidentFace + 1) < incData.vertexCount) ? (incidentFace + 1) : 0);
    *v1 = Mat2MultiplyVector2(inc.transform, incData.positions[incidentFace]);
    *v1 = Vector2Add(*v1, inc.body->position);
}

static int Clip(vec2s normal, float clip, vec2s *faceA, vec2s *faceB) {
  int sp = 0;
  vec2s out[2] = { *faceA, *faceB };
  float distanceA = glm_vec2_dot(normal.raw, faceA->raw) - clip;
  float distanceB = glm_vec2_dot(normal.raw, faceB->raw) - clip;
  if (distanceA <= 0.0f) {
    out[sp++] = *faceA;
  }
  if (distanceB <= 0.0f) {
    out[sp++] = *faceB;
  }
  if ((distanceA*distanceB) < 0.0f) {
    float alpha = distanceA/(distanceA - distanceB);
    out[sp] = *faceA;
    vec2s delta;
    glm_vec2_sub(faceB->raw, faceA->raw, delta.raw);
    delta.x *= alpha;
    delta.y *= alpha;
    glm_vec2_add(out[sp].raw, delta.raw, out[sp].raw);
    sp++;
  }
  *faceA = out[0];
  *faceB = out[1];
  return sp;
}

// Check if values are between bias range
static bool BiasGreaterThan(float valueA, float valueB)
{
    return (valueA >= (valueB*0.95f + valueA*0.01f));
}

// Returns the barycenter of a triangle given by 3 points
static Vector2 TriangleBarycenter(Vector2 v1, Vector2 v2, Vector2 v3)
{
    Vector2 result = { 0.0f, 0.0f };

    result.x = (v1.x + v2.x + v3.x)/3;
    result.y = (v1.y + v2.y + v3.y)/3;

    return result;
}

// Initializes hi-resolution MONOTONIC timer
static void InitTimer(void)
{
    srand(time(NULL));              // Initialize random seed

    #if defined(_WIN32)
        QueryPerformanceFrequency((unsigned long long int *) &frequency);
    #endif

    #if defined(__linux__)
        struct timespec now;
        if (clock_gettime(CLOCK_MONOTONIC, &now) == 0)
            frequency = 1000000000;
    #endif

    #if defined(__APPLE__)
        mach_timebase_info_data_t timebase;
        mach_timebase_info(&timebase);
        frequency = (timebase.denom*1e9)/timebase.numer;
    #endif

    #if defined(EMSCRIPTEN)
      frequency = 1000;
    #endif

    baseTime = GetTimeCount();      // Get MONOTONIC clock time offset
    startTime = GetCurrentTime();   // Get current time
}

// Get hi-res MONOTONIC time measure in seconds
static uint64_t GetTimeCount(void)
{
    uint64_t value = 0;

    #if defined(_WIN32)
        QueryPerformanceCounter((unsigned long long int *) &value);
    #endif

    #if defined(__linux__)
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        value = (uint64_t)now.tv_sec*(uint64_t)1000000000 + (uint64_t)now.tv_nsec;
    #endif

    #if defined(__APPLE__)
        value = mach_absolute_time();
    #endif

    #if defined(EMSCRIPTEN)
      value = emscripten_get_now();
    #endif

    return value;
}

// Get current time in milliseconds
static double GetCurrentTime(void)
{
    return (double)(GetTimeCount() - baseTime)/frequency*1000;
}

static mat2s glm_mat2_radians(float radians) {
  float c = cosf(radians);
  float s = sinf(radians);
  mat2s mat;
  float raw[4] = { c, -s, s, c };
  glm_mat2_make(raw, mat.raw);
  return mat;
}

static void glm_mat2_radians_set(mat2 *matrix, float radians) {
  float cos = cosf(radians);
  float sin = sinf(radians);
  float raw[4] = { cos, -sin, sin, cos };
  glm_mat2_make(raw, *matrix);
}

static float glm_vec2_len_sqr(vec2 v) {
  return (v[0] * v[0] + v[1] * v[1]);
}

static float glm_vec2_dist_sqr(vec2 v1, vec2 v2) {
  vec2 dir;
  glm_vec2_sub(v1, v2, dir);
  return glm_vec2_dot(dir, dir);
}
