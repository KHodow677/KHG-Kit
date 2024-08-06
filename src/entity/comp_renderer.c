#include "comp_renderer.h"
#include "ecs_setup.h"
#include "khg_ecs/ecs.h"

ecs_id RENDERER_COMPONENT;

void comp_renderer_setup(comp_renderer *cr) {

}

void comp_renderer_free(comp_renderer *cr) {

}

void comp_renderer_register(comp_renderer *cr, ecs_ecs *ecs) {

}

void sys_renderer_register(sys_renderer *sr, comp_renderer *cr, ecs_ecs *ecs) {

}

ecs_ret sys_renderer_update(ecs_ecs *ecs, ecs_id *entities, int entity_count, ecs_dt dt, void *udata) {
  return 0;
}

