#include "networking/client.h"
#include "khg_tcp/tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int TIMEOUT = 500;

void client_run() {
    game_client client = {0};
    tcp_init();
    client_connect(&client, "165.22.176.143", "http");
    client_send_message(&client, "Hello Server!");
    while (true) {
        client_receive_message(&client);
    }
    client_disconnect(&client);
    tcp_term();
}

void client_connect(game_client *client, const char *ip, const char *port) {
    printf("Connecting to server at %s:%s\n", ip, port);
    client->ip = ip;
    client->port = port;
    client->server = tcp_connect(ip, port);
    if (!client->server) {
        fprintf(stderr, "Failed to connect to server at %s:%s\n", ip, port);
        exit(EXIT_FAILURE);
    }
}

void client_disconnect(game_client *client) {
    printf("Disconnecting from server\n");
    tcp_close_channel(client->server);
}

void client_send_message(game_client *client, const char *message) {
    const char *request_template = "POST /send HTTP/1.1\r\nHost: %s\r\nContent-Length: %zu\r\n\r\n%s";
    char formatted_request[1024];
    snprintf(formatted_request, sizeof(formatted_request), request_template, client->ip, strlen(message), message);
    
    if (tcp_send(client->server, formatted_request, strlen(formatted_request), TIMEOUT)) {
        printf("Message sent: %s\n", message);
    } else {
        fprintf(stderr, "Failed to send message\n");
    }
}

void client_receive_message(game_client *client) {
    int bytes_received = tcp_receive(client->server, client->buffer, sizeof(client->buffer), TIMEOUT);
    if (bytes_received > 0) {
        printf("Received: %.*s\n", bytes_received, client->buffer);
    } else {
        printf("No message received within timeout\n");
    }
}
