#pragma once

#include "khg_kin/namespace.h"

kin_engine engine_create(void);
void engine_deallocate(kin_engine *engine);

void engine_step(kin_engine engine, float dt);
void engine_body_add(kin_engine engine, kin_body body);
void engine_body_remove(kin_engine engine, kin_body body);

void engine_print(kin_engine engine);
