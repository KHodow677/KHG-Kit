#pragma once

#include "khg_ecs/ecs.h"
#include "khg_utl/vector.h"

typedef struct {
  bool selected;
  bool just_selected;
  int tex_id;
  int linked_tex_id;
  int selected_tex_id;
  int selected_linked_tex_id;
} selector_info;

typedef struct {
  ecs_id id;
} comp_selector;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_selector;

extern ecs_id SELECTOR_COMPONENT_SIGNATURE;
extern selector_info NO_SELECTOR;
extern utl_vector *SELECTOR_INFO;

void comp_selector_register(comp_selector *cs);

void sys_selector_register(sys_selector *ss);
void sys_selector_add(ecs_id *eid, selector_info *info);

