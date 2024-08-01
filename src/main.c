#define LIGHTMODBUS_SLAVE_FULL
#define LIGHTMODBUS_DEBUG
#define LIGHTMODBUS_IMPL

#include "chat.h"
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

bool print_buffer(const char* buffer, int length, void* user_data)
{
	(void) user_data;
	return fwrite(buffer, sizeof(char), length, stdout) == (size_t) length;
}

int main(int argc, char *argv[]) {
  printf("Hi\n");
	stcp_channel* channel = NULL;
	stcp_set_error_callback(process_error, &channel);
	stcp_initialize();

	const char* request = "HEAD / HTTP/1.2\r\n\r\n";
	channel = stcp_connect("www.google.com", "http");
	stcp_send(channel, request, strlen(request), 500);
	stcp_stream_receive(channel, print_buffer, NULL, 500);

	stcp_close_channel(channel);
	stcp_terminate();
	return 0;
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <server|client>\n", argv[0]);
    return -1;
  }
  if (strcmp(argv[1], "server") == 0) {
    printf("Starting chat server...\n");
    return server_test();
  } 
  else if (strcmp(argv[1], "client") == 0) {
    printf("Starting chat client...\n");
    return client_test();
  } 
  else {
    fprintf(stderr, "Unknown argument '%s'. Use 'server' or 'client'.\n", argv[1]);
    return -1;
  }
  return 0;
}
