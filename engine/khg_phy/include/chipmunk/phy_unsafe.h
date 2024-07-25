#pragma once

#include "chipmunk/phy_types.h"

/// Set the radius of a circle shape.
CP_EXPORT void cpCircleShapeSetRadius(cpShape *shape, cpFloat radius);
/// Set the offset of a circle shape.
CP_EXPORT void cpCircleShapeSetOffset(cpShape *shape, cpVect offset);

/// Set the endpoints of a segment shape.
CP_EXPORT void cpSegmentShapeSetEndpoints(cpShape *shape, cpVect a, cpVect b);
/// Set the radius of a segment shape.
CP_EXPORT void cpSegmentShapeSetRadius(cpShape *shape, cpFloat radius);

/// Set the vertexes of a poly shape.
CP_EXPORT void cpPolyShapeSetVerts(cpShape *shape, int count, cpVect *verts, cpTransform transform);
CP_EXPORT void cpPolyShapeSetVertsRaw(cpShape *shape, int count, cpVect *verts);
/// Set the radius of a poly shape.
CP_EXPORT void cpPolyShapeSetRadius(cpShape *shape, cpFloat radius);

