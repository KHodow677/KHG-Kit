#pragma once

#include "khg_kin/namespace.h"

kin_engine kin_engine_create(void);
void kin_engine_deallocate(kin_engine *e);

void kin_engine_step(kin_engine e, float dt);
void kin_engine_body_add(kin_engine e, kin_body b);
void kin_engine_body_remove(kin_engine e, kin_body b);

void kin_engine_print(kin_engine e);
