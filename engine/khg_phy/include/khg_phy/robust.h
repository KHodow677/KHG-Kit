#pragma once

#include "khg_phy/phy_types.h"

// This is a private header for functions (currently just one) that need strict floating point results.
// It was easier to put this in it's own file than to fiddle with 4 different compiler specific pragmas or attributes.
// "Fast math" should be disabled here.

// Check if c is to the left of segment (a, b).
bool cpCheckPointGreater(const phy_vect a, const phy_vect b, const phy_vect c);

// Check if p is behind one of v0 or v1 on axis n.
bool cpCheckAxis(phy_vect v0, phy_vect v1, phy_vect p, phy_vect n);
