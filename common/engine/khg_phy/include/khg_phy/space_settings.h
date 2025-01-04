#pragma once

#include "khg_phy/constraints/constraint.h"
#include <stdbool.h>

typedef struct phy_space_settings {
  float baumgarte;
  float penetration_slop;
  phy_contact_position_correction contact_position_correction;
  unsigned int velocity_iterations;
  unsigned int position_iterations;
  unsigned int substeps;
  float linear_damping;
  float angular_damping;
  bool warmstarting;
  phy_coefficient_mix restitution_mix;
  phy_coefficient_mix friction_mix;
} phy_space_settings;

