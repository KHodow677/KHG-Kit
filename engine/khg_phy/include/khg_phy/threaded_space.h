#pragma once

#include "khg_phy/phy_types.h"

struct phy_hasty_space;
typedef struct phy_hasty_space phy_hasty_space;

phy_space *phy_threaded_space_new(void);
void phy_threaded_space_free(phy_space *space);

void phy_threaded_space_set_threads(phy_space *space, unsigned long threads);
unsigned long phy_threaded_space_get_threads(phy_space *space);

void phy_threaded_space_step(phy_space *space, float dt);

