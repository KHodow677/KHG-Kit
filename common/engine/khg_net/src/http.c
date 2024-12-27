#include <stdlib.h>
#include <string.h>
#include "khg_net/http.h"
#include "khg_utl/error_func.h"
#include "khg_utl/string.h"

static net_http_route ROUTES[NET_MAX_ROUTES];
static size_t ROUTE_COUNT = 0;
static bool SERVER_RUNNING = true;

static bool net_match_route(const char *route, const char *path, int32_t *id_out) {
  const char* route_ptr = route;
  const char* path_ptr = path;
  while (*route_ptr && *path_ptr) {
    if (*route_ptr == '{') {
      while (*route_ptr && *route_ptr != '}') {
        route_ptr++;
      }
      route_ptr++;
      *id_out = atoi(path_ptr);
      while (*path_ptr && *path_ptr != '/') {
        path_ptr++;
      }
    } 
    else {
      if (*route_ptr != *path_ptr) {
        return false;
      }
      route_ptr++;
      path_ptr++;
    }
  }
  bool match = *route_ptr == '\0' && *path_ptr == '\0';
  return match;
}

static void net_handle_request(net_http_request *req, net_http_response *res) {
  for (size_t i = 0; i < ROUTE_COUNT; i++) {
    int32_t id = -1;
    if (net_match_route(ROUTES[i].path, req->path, &id) && ROUTES[i].method == req->method) {
      req->id = id;
      ROUTES[i].handler(req, res);
      return;
    }
  }
  net_http_send_error(res, 404, "Route not found");
}

net_http_method net_http_parse_method(const char *request) {
  if (strncmp(request, "GET", 3) == 0) { 
    return NET_HTTP_GET;
  }
  else if (strncmp(request, "POST", 4) == 0) { 
    return NET_HTTP_POST;
  }
  else if (strncmp(request, "PUT", 3) == 0) {
    return NET_HTTP_PUT;
  }
  else if (strncmp(request, "DELETE", 6) == 0) { 
    return NET_HTTP_DELETE;
  }
  else if (strncmp(request, "OPTIONS", 7) == 0) { 
    return NET_HTTP_OPTIONS;
  }
  else if (strncmp(request, "HEAD", 4) == 0) { 
    return NET_HTTP_HEAD;
  }
  else if (strncmp(request, "PATCH", 5) == 0) { 
    return NET_HTTP_PATCH;
  }
  return NET_HTTP_UNKNOWN;
}

net_http_request *net_http_parse_request(const char *request) {
  net_http_request *req = (net_http_request *)malloc(sizeof(net_http_request));
  memset(req, 0, sizeof(net_http_request));
  req->method = net_http_parse_method(request);
  const char *path_start = strchr(request, ' ') + 1;
  const char *path_end = strchr(path_start, ' ');
  req->path = utl_string_strndup(path_start, path_end - path_start);
  char *query_start = strchr(req->path, '?');
  if (query_start) {
    *query_start = '\0'; 
    char *query_string = utl_string_strdup(query_start + 1);
    char *token = strtok(query_string, "&");
    while (token) {
      char *equal_sign = strchr(token, '=');
      if (equal_sign) {
        *equal_sign = '\0';
        req->query_params[req->query_param_count].name = utl_string_strdup(token);
        req->query_params[req->query_param_count].value = utl_string_strdup(equal_sign + 1);
        req->query_param_count++;
      }
      token = strtok(NULL, "&");
    }
    free(query_string);
  }
  const char *header_start = strstr(request, "\r\n") + 2;
  while (header_start && *header_start != '\r') {
    const char *header_end = strstr(header_start, "\r\n");
    if (!header_end) {
      break;
    }
    const char *colon = strchr(header_start, ':');
    if (colon && colon < header_end) {
      req->headers[req->header_count].name = utl_string_strndup(header_start, colon - header_start);
      req->headers[req->header_count].value = utl_string_strndup(colon + 2, header_end - colon - 2);
      req->header_count++;
    }
    header_start = header_end + 2;
  }
  const char *body_start = strstr(request, "\r\n\r\n");
  if (body_start) {
    body_start += 4;
    req->body = utl_string_strdup(body_start);
    req->json_body = utl_json_parse(req->body);
    if (!req->json_body) {
      utl_error_func("Failed to parse JSON body", utl_user_defined_data);
    }
  }
  return req;
}

void net_http_free_request(net_http_request *request) {
  if (!request) { 
    return;
  }
  if (request->path) { 
    free(request->path);
  }
  if (request->body) { 
    free(request->body);
  }
  if (request->json_body) {
    utl_json_deallocate(request->json_body);
  }
  for (size_t i = 0; i < request->header_count; i++) {
    free(request->headers[i].name);
    free(request->headers[i].value);
  }
  for (size_t i = 0; i < request->query_param_count; i++) {
    free(request->query_params[i].name);
    free(request->query_params[i].value);
  }
  free(request);
}

void net_http_set_status(net_http_response *response, uint16_t code, const char *message) {
  response->status_code = code;
  response->status_message = utl_string_strdup(message);
}

void net_http_set_json_body(net_http_response *response, utl_json_element *json) {
  if (json) {
    if (response->json_body) {
      utl_json_deallocate(response->json_body);
    }
    response->json_body = json;
    if (response->body) {
      free(response->body);
    }
    response->body = utl_json_serialize(json);
    net_http_add_header(response, "Content-Type", "application/json");
  } 
  else {
    utl_error_func("Null JSON body received", utl_user_defined_data);
    net_http_set_status(response, 500, "Internal Server Error: Null JSON Body");
    net_http_set_body(response, "Failed to set JSON body: NULL pointer received");
  }
}

void net_http_set_body(net_http_response *response, const char *body) {
  response->body = utl_string_strdup(body);
  net_http_add_header(response, "Content-Type", "text/plain");
}

void net_http_add_header(net_http_response *response, const char *header, const char *value) {
  response->headers[response->header_count].name = utl_string_strdup(header);
  response->headers[response->header_count].value = utl_string_strdup(value);
  response->header_count++;
}

char *net_http_serialize_response(net_http_response *response) {
  size_t estimated_size = 1024 + (response->json_body ? 1024 : 0) + (response->body ? strlen(response->body) : 0);
  char *buffer = (char *)malloc(estimated_size);
  snprintf(buffer, estimated_size, "HTTP/1.1 %d %s\r\n", response->status_code, response->status_message);
  for (size_t i = 0; i < response->header_count; i++) {
    strcat(buffer, response->headers[i].name);
    strcat(buffer, ": ");
    strcat(buffer, response->headers[i].value);
    strcat(buffer, "\r\n");
  }
  strcat(buffer, "\r\n");
  if (response->json_body) {
    char* json_str = utl_json_serialize(response->json_body);
    strcat(buffer, json_str);
    free(json_str);
  } 
  else if (response->body) {
    strcat(buffer, response->body);
  }
  return buffer;
}


void net_http_free_response(net_http_response *response) {
  if (response->status_message) {
    free(response->status_message);
  }
  if (response->json_body) { 
    utl_json_deallocate(response->json_body);
  }
  if (response->body) { 
    free(response->body);
  }
  for (size_t i = 0; i < response->header_count; i++) {
    free(response->headers[i].name);
    free(response->headers[i].value);
  }
}

void net_http_register_route(const char *path, net_http_method method, net_http_handler handler) {
  if (ROUTE_COUNT < NET_MAX_ROUTES) {
    ROUTES[ROUTE_COUNT].path = utl_string_strdup(path);
    ROUTES[ROUTE_COUNT].method = method;
    ROUTES[ROUTE_COUNT].handler = handler;
    ROUTE_COUNT++;
  }
}

void net_http_start_server(uint16_t port) {
  net_tcp_socket server_socket;
  net_tcp_status status;
  status = net_tcp_init();
  if (status != NET_TCP_SUCCESS) {
    utl_error_func("Failed to initialize network", utl_user_defined_data);
    return;
  }
  status = net_tcp_socket_create(&server_socket);
  if (status != NET_TCP_SUCCESS) {
    utl_error_func("Failed to create server socket", utl_user_defined_data);
    return;
  }
  status = net_tcp_bind(server_socket, "0.0.0.0", port);
  if (status != NET_TCP_SUCCESS) {
    utl_error_func("Failed to bind server socket", utl_user_defined_data);
    return;
  }
  status = net_tcp_listen(server_socket, 10);
  if (status != NET_TCP_SUCCESS) {
    utl_error_func("Failed to listen on server socket", utl_user_defined_data);
    return;
  }
  while (SERVER_RUNNING) {
    net_tcp_socket client_socket;
    status = net_tcp_accept(server_socket, &client_socket);
    if (status == NET_TCP_SUCCESS) {
      net_http_handle_request(client_socket);
      net_tcp_close(client_socket);
    }
  }
  net_tcp_close(server_socket);
  net_tcp_cleanup();
}

void net_http_stop_server(void) {
  SERVER_RUNNING = false;
}

void net_http_handle_request(net_tcp_socket client_socket) {
  char buffer[4096];
  size_t received;
  net_tcp_status status = net_tcp_recv(client_socket, buffer, sizeof(buffer), &received);
  if (status == NET_TCP_SUCCESS && received > 0) {
    buffer[received] = '\0';
    net_http_request *req = net_http_parse_request(buffer);
    if (!req) {
      utl_error_func("Failed to parse HTTP request", utl_user_defined_data);
      net_http_response res = {0};
      net_http_send_error(&res, 400, "Bad Request: Invalid HTTP Request");
      char *response_str = net_http_serialize_response(&res);
      net_tcp_send(client_socket, response_str, strlen(response_str), &received);
      free(response_str);
      return;
    }
    net_http_response res = {0};
    res.status_code = 200;
    res.status_message = utl_string_strdup("OK");
    net_handle_request(req, &res);
    char *response_str = net_http_serialize_response(&res);
    if (response_str) {
      size_t sent;
      net_tcp_send(client_socket, response_str, strlen(response_str), &sent);
      free(response_str);
    }
    net_http_free_request(req);
    net_http_free_response(&res);
  } 
  else {
    utl_error_func("Failed to receive data from socket or no data received", utl_user_defined_data);
  }
}

const char *net_http_get_header(net_http_request *req, const char *name) {
  for (size_t i = 0; i < req->header_count; i++) {
    if (strcmp(req->headers[i].name, name) == 0) {
      return req->headers[i].value;
    }
  }
  return NULL;
}

const char *net_http_get_query_param(net_http_request *req, const char *name) {
  for (size_t i = 0; i < req->query_param_count; i++) {
    if (strcmp(req->query_params[i].name, name) == 0) {
      return req->query_params[i].value;
    }
  }
  return NULL;
}

void net_http_send_error(net_http_response *res, uint16_t code, const char *message) {
  net_http_set_status(res, code, message);
  net_http_set_body(res, message);
}

