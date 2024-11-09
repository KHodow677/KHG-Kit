/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#ifndef NOVAPHYSICS_H
#define NOVAPHYSICS_H


/**
 * @file novaphysics.h
 * 
 * @brief Main Nova Physics API.
 * 
 * Included STL headers:
 * - stdlib.h
 * - stdio.h (For sprintf in core/error)
 * - stdint.h (For nv_uint and nv_int types)
 * - math.h
 * - float.h (For NV_FLOAT_EPSILON)
 * - string.h (For memory functions)
 */


// Stringify macro
#define _NV_STRINGIFY(x) #x
#define NV_STRINGIFY(x) _NV_STRINGIFY(x)

// Version in MAJOR.MINOR.PATCH format
#define NV_VERSION_MAJOR 1
#define NV_VERSION_MINOR 0
#define NV_VERSION_PATCH 0
// Version string
#define NV_VERSION_STRING                  \
        NV_STRINGIFY(NV_VERSION_MAJOR) "." \
        NV_STRINGIFY(NV_VERSION_MINOR) "." \
        NV_STRINGIFY(NV_VERSION_PATCH)


#include "khg_phy/core/error.h"
#include "khg_phy/vector.h"
#include "khg_phy/math.h"
#include "khg_phy/aabb.h"
#include "khg_phy/constants.h"
#include "khg_phy/material.h"
#include "khg_phy/broadphase.h"
#include "khg_phy/space.h"
#include "khg_phy/body.h"
#include "khg_phy/collision.h"
#include "khg_phy/contact.h"
#include "khg_phy/constraints/constraint.h"
#include "khg_phy/constraints/contact_constraint.h"
#include "khg_phy/constraints/distance_constraint.h"
#include "khg_phy/constraints/hinge_constraint.h"
#include "khg_phy/constraints/spline_constraint.h"


#endif
