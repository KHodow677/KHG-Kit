#define LIGHTMODBUS_SLAVE_FULL
#define LIGHTMODBUS_DEBUG
#define LIGHTMODBUS_IMPL

#include "khg_ecs/ecs.h"
#include "khg_gfx/texture.h"
#include "khg_gfx/ui.h"
#include "khg_gfx/elements.h"
#include "khg_tcp/stcp.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

float ang = 0.0f;
gfx_texture body, top;

typedef struct {
  float x, y;
} pos_t;

typedef struct {
  float vx, vy;
} vel_t;

typedef struct {
  int x, y, w, h;
} rect_t;

ecs_id_t PosComp;
ecs_id_t VelComp;
ecs_id_t RectComp;

ecs_id_t System1;
ecs_id_t System2;
ecs_id_t System3;


void register_components(ecs_t *ecs) {
  PosComp = ecs_register_component(ecs, sizeof(pos_t), NULL, NULL);
  VelComp = ecs_register_component(ecs, sizeof(vel_t), NULL, NULL);
  RectComp = ecs_register_component(ecs, sizeof(rect_t), NULL, NULL);
}

ecs_ret_t system_update(ecs_t *ecs, ecs_id_t *entities, int entity_count, ecs_dt_t dt, void *udata) {
  (void)ecs;
  (void)dt;
  (void)udata;
  for (int id = 0; id < entity_count; id++) {
    printf("%u ", entities[id]);
  }
  printf("\n");
  return 0;
}

void register_systems(ecs_t *ecs) {
  System1 = ecs_register_system(ecs, system_update, NULL, NULL, NULL);
  System2 = ecs_register_system(ecs, system_update, NULL, NULL, NULL);
  System3 = ecs_register_system(ecs, system_update, NULL, NULL, NULL);

  ecs_require_component(ecs, System1, PosComp);
  ecs_require_component(ecs, System2, PosComp);
  ecs_require_component(ecs, System2, VelComp);

  ecs_require_component(ecs, System3, PosComp);
  ecs_require_component(ecs, System3, VelComp);
  ecs_require_component(ecs, System3, RectComp);
}


int ecs_test() {
  ecs_t *ecs = ecs_new(1024, NULL);

  register_components(ecs);
  register_systems(ecs);

  ecs_id_t e1 = ecs_create(ecs);
  ecs_id_t e2 = ecs_create(ecs);
  ecs_id_t e3 = ecs_create(ecs);

  printf("---------------------------------------------------------------\n");
  printf("Created entities: %u, %u, %u\n", e1, e2, e3);
  printf("---------------------------------------------------------------\n");

  printf("PosComp added to: %u\n", e1);
  ecs_add(ecs, e1, PosComp, NULL);

  printf("---------------------------------------------------------------\n");
  printf("PosComp added to: %u\n", e2);
  printf("VeloComp added to: %u\n", e2);

  ecs_add(ecs, e2, PosComp, NULL);
  ecs_add(ecs, e2, VelComp, NULL);

  printf("---------------------------------------------------------------\n");
  printf("PosComp added to: %u\n", e3);
  printf("VeloComp added to: %u\n", e3);
  printf("RectComp added to: %u\n", e3);

  ecs_add(ecs, e3, PosComp, NULL);
  ecs_add(ecs, e3, VelComp, NULL);
  ecs_add(ecs, e3, RectComp, NULL);

  printf("---------------------------------------------------------------\n");

  printf("Executing system 1\n");
  ecs_update_system(ecs, System1, 0.0f);

  printf("Executing system 2\n");
  ecs_update_system(ecs, System2, 0.0f);

  printf("Executing system 3\n");
  ecs_update_system(ecs, System3, 0.0f);

  printf("---------------------------------------------------------------\n");

  ecs_free(ecs);

  return 0;
}

int gfx_start() {
  if (!glfwInit()) {
    return -1;
  }
  GLFWwindow *window = glfwCreateWindow(800, 600, "Hello", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  gfx_init_glfw(800, 600, window);
  body = gfx_load_texture_asset("Tank-Body-Blue", "png");
  top = gfx_load_texture_asset("Tank-Top-Blue", "png");
  return gfx_loop_manager(window);
}

void gfx_loop() {
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  gfx_begin();
  ang += 0.05f;
  body.angle = ang;
  gfx_rect_no_block(300.0f, 250.0f, 145, 184, gfx_white, 0.0f, ang);
  gfx_image_no_block(200.0f, 150.0f, body);
  gfx_text("Hello!");
  //gfx_image(top);
}

void process_error(stcp_error e, void* user_data)
{
	stcp_channel** channel = (stcp_channel**) user_data;

	perror(stcp_error_to_string(e));
	stcp_close_channel(*channel);
	stcp_terminate();
	exit(-1);
}

bool print_buffer(const char* buffer, int length, void* user_data) {
	(void) user_data;
	return fwrite(buffer, sizeof(char), length, stdout) == (size_t) length;
}

int tcp_test() {
	stcp_channel *channel = NULL;
	stcp_set_error_callback(process_error, &channel);
	stcp_initialize();
	const char *request = "HEAD / HTTP/1.2\r\n\r\n";
	channel = stcp_connect("www.google.com", "http");
	stcp_send(channel, request, strlen(request), 500);
	stcp_stream_receive(channel, print_buffer, NULL, 500);
	stcp_close_channel(channel);
	stcp_terminate();
	return 0;
}

int main(int argc, char *argv[]) {
  return ecs_test();
  return 0;
}
