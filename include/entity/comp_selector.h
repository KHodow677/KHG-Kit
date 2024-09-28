#pragma once

#include "khg_ecs/ecs.h"

typedef struct {
  ecs_id id;
  bool selected;
  bool just_selected;
  bool should_deselect;
  int tex_id;
  int linked_tex_id;
  int selected_tex_id;
  int selected_linked_tex_id;
} comp_selector;

typedef struct {
  ecs_id id;
  ecs_ecs ecs;
} sys_selector;

extern ecs_id SELECTOR_COMPONENT_SIGNATURE;

void comp_selector_register(void);

void sys_selector_register(sys_selector *ss);
comp_selector *sys_selector_add(ecs_id eid);

