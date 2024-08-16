#pragma once

#include "khg_phy/phy_types.h"

/// Set the radius of a circle shape.
void cpCircleShapeSetRadius(phy_shape *shape, float radius);
/// Set the offset of a circle shape.
void cpCircleShapeSetOffset(phy_shape *shape, phy_vect offset);

/// Set the endpoints of a segment shape.
void cpSegmentShapeSetEndpoints(phy_shape *shape, phy_vect a, phy_vect b);
/// Set the radius of a segment shape.
void cpSegmentShapeSetRadius(phy_shape *shape, float radius);

/// Set the vertexes of a poly shape.
void cpPolyShapeSetVerts(phy_shape *shape, int count, phy_vect *verts, phy_transform transform);
void cpPolyShapeSetVertsRaw(phy_shape *shape, int count, phy_vect *verts);
/// Set the radius of a poly shape.
void cpPolyShapeSetRadius(phy_shape *shape, float radius);

