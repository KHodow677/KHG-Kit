#pragma once

typedef struct phy_material {
  float density;
  float restitution;
  float friction;
} phy_material;

static const phy_material phy_material_basic = { .density = 1.0f, .restitution = 0.1f, .friction = 0.4f };
static const phy_material phy_material_steel = { .density = 7.8f, .restitution = 0.43f, .friction = 0.45f };
static const phy_material phy_material_wood = { .density = 1.5f, .restitution = 0.37f, .friction = 0.52f };
static const phy_material phy_material_glass = { .density = 2.5f, .restitution = 0.55f, .friction = 0.19f };
static const phy_material phy_material_ice = { .density = 0.92f, .restitution = 0.05f, .friction = 0.02f };
static const phy_material phy_material_concrete = { .density = 3.6f, .restitution = 0.075f, .friction = 0.73f };
static const phy_material phy_material_rubber = { .density = 1.4f, .restitution = 0.89f, .friction = 0.92f };
static const phy_material phy_material_gold = { .density = 19.3f, .restitution = 0.4f, .friction = 0.35f };
static const phy_material phy_material_cardboard = { .density = 0.6f, .restitution = 0.02f, .friction = 0.2f };

