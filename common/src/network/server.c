#include "khg_net/http.h"
#include <stdio.h>

void handle_root(net_http_request *req, net_http_response *res) {
  (void)req;
  net_http_set_status(res, 200, "OK");
  net_http_set_body(res, "Hello, World!");
}

void handle_echo(net_http_request *req, net_http_response *res) {
  printf("Received body: %s\n", req->body);
  if (req->json_body) {
    printf("Debug: Valid JSON body detected\n");
    net_http_set_status(res, 200, "OK");
    net_http_set_json_body(res, utl_json_deep_copy(req->json_body));
  } 
  else {
    net_http_send_error(res, 400, "Bad Request: Expected JSON body");
  }
}

int server() {
  net_http_register_route("/", NET_HTTP_GET, handle_root);
  net_http_register_route("/echo", NET_HTTP_POST, handle_echo);
  printf("Starting HTTP server on port 8051...\n");
  net_http_start_server(8051);
  return 0;
}

