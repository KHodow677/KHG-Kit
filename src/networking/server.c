#include "networking/server.h"
#include "khg_tcp/tcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

client_t clients[MAX_CLIENTS];
int client_count = 0;
unsigned int client_id_counter = 0xFFFFFF;
char clientData[BUFFER_SIZE] = {0};

void assign_client_id(client_t *client) {
    snprintf(client->id, sizeof(client->id), "%06X", client_id_counter--);
}

client_t *find_client_by_sock(tcp_channel *sock) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].sock == sock) {
            return &clients[i];
        }
    }
    return NULL;
}

client_t *find_client_by_id(const char *id) {
    for (int i = 0; i < client_count; i++) {
        if (strcmp(clients[i].id, id) == 0) {
            return &clients[i];
        }
    }
    return NULL;
}

void handle_post_request(tcp_channel *client_sock, const char *body) {
    char receiver_id[7] = {0}; // 6 characters + null terminator
    char message[BUFFER_SIZE] = {0};

    // Extract receiver ID and message from the body
    sscanf(body, "{\"message\":\"%6[^:]:%[^\"]\"}", receiver_id, message);

    // Find receiver and send the message
    client_t *receiver = find_client_by_id(receiver_id);
    if (receiver) {
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n%s", message);
        tcp_send(receiver->sock, response, strlen(response), 500);
    }

    // Store the message on the server and send a response to the sender
    strncpy(clientData, message, sizeof(clientData) - 1);
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n%s", message);
    tcp_send(client_sock, response, strlen(response), 500);
}

void handle_get_request(tcp_channel *client_sock) {
    const char *response_message = clientData[0] ? clientData : "No message available";
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n%s", response_message);
    tcp_send(client_sock, response, strlen(response), 500);
}

void handle_client_request(tcp_channel *client_sock, const char *request) {
    if (strncmp(request, "POST /send", 10) == 0) {
        // Extract the body of the POST request
        const char *body = strstr(request, "\r\n\r\n") + 4;
        handle_post_request(client_sock, body);
    } else if (strncmp(request, "GET /receive", 12) == 0) {
        handle_get_request(client_sock);
    } else {
        const char *response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nServer not found";
        tcp_send(client_sock, response, strlen(response), 500);
    }
}

void handle_new_client(tcp_channel *client_sock) {
    if (client_count >= MAX_CLIENTS) {
        printf("Max clients reached!\n");
        return;
    }

    // Assign a unique ID to the new client
    client_t new_client;
    new_client.sock = client_sock;
    assign_client_id(&new_client);
    clients[client_count++] = new_client;

    printf("Client %s connected\n", new_client.id);
}

void close_client(tcp_channel *client_sock) {
    client_t *client = find_client_by_sock(client_sock);
    if (client) {
        printf("Client %s disconnected\n", client->id);
        // Remove client from the list
        for (int i = 0; i < client_count; i++) {
            if (clients[i].sock == client_sock) {
                clients[i] = clients[--client_count]; // Move the last client into the current slot
                break;
            }
        }
        tcp_close_channel(client_sock);
    }
}

bool process_client_message(const char *buffer, int length, void *user_data) {
    tcp_channel *client_sock = (tcp_channel *)user_data;
    handle_client_request(client_sock, buffer);
    return true;
}

int server_start() {
    tcp_init();

    tcp_server *server = tcp_open_server("0.0.0.0", "3000", 10);
    printf("HTTP server started on port 3000\n");

    while (true) {
        tcp_channel *client_sock = tcp_accept(server, 1000);
        if (client_sock) {
            handle_new_client(client_sock);

            // Process client requests in a loop (this can be made concurrent)
            while (tcp_stream_receive_no_timeout(client_sock, process_client_message, client_sock)) {
                // Continuously receive messages from the client
            }

            // Client disconnected
            close_client(client_sock);
        }
    }

    tcp_close_server(server);
    tcp_term();
    return 0;
}
