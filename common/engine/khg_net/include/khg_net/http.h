#pragma once

#include "khg_net/tcp.h"
#include "khg_utl/json.h"
#include <stdint.h>

#define NET_MAX_HEADERS 100
#define NET_MAX_QUERY_PARAMS 50
#define NET_MAX_ROUTES 100

typedef enum net_http_method {
  NET_HTTP_GET,
  NET_HTTP_POST,
  NET_HTTP_PUT,
  NET_HTTP_DELETE,
  NET_HTTP_OPTIONS,
  NET_HTTP_HEAD,
  NET_HTTP_PATCH,
  NET_HTTP_UNKNOWN
} net_http_method;

typedef struct net_http_header {
  char *name;
  char *value;
} net_http_header;

typedef struct net_http_query_param {
  char *name;
  char *value;
} net_http_query_param;

typedef struct net_http_request {
  net_http_method method;
  char *path;
  char *body;
  utl_json_element *json_body;
  net_http_header headers[NET_MAX_HEADERS];
  net_http_query_param query_params[NET_MAX_QUERY_PARAMS];
  size_t header_count;
  size_t query_param_count;
  int32_t id;
} net_http_request;

typedef struct net_http_response {
  uint16_t status_code;
  char *status_message;
  net_http_header headers[NET_MAX_HEADERS];
  utl_json_element *json_body;
  char *body;
  size_t header_count;
} net_http_response;

typedef void (*net_http_handler)(net_http_request *req, net_http_response *res);

typedef struct net_http_route {
  char *path;
  net_http_method method;
  net_http_handler handler;
} net_http_route;

net_http_method net_http_parse_method(const char *request);
net_http_request *net_http_parse_request(const char *request);
void net_http_free_request(net_http_request *request);

void net_http_set_status(net_http_response *response, uint16_t code, const char *message);
void net_http_set_json_body(net_http_response *response, utl_json_element *json);
void net_http_set_body(net_http_response *response, const char *body);
void net_http_add_header(net_http_response *response, const char *header, const char *value);
void net_http_free_response(net_http_response *response);

void net_http_register_route(const char *path, net_http_method method, net_http_handler handler);
void net_http_start_server(uint16_t port);
void net_http_stop_server(void);
void net_http_handle_request(net_tcp_socket client_socket);

const char *net_http_get_header(net_http_request *req, const char *name);
const char *net_http_get_query_param(net_http_request *req, const char *name);
void net_http_send_error(net_http_response *res, uint16_t code, const char *message);

char *net_http_serialize_response(net_http_response *response);

