#include "networking/server.h"
#include "khg_tcp/tcp.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

client_t clients[MAX_CLIENTS];
int client_count = 0;
unsigned int client_id_counter = 16777215;

void handle_client(tcp_channel *client_sock) {
    char buffer[BUFFER_SIZE];
    int bytes_received;

    // Assign a unique client ID
    char client_id[7];
    snprintf(client_id, sizeof(client_id), "%06X", client_id_counter--);

    // Log connection
    printf("Client %s connected\n", client_id);

    while ((bytes_received = tcp_receive(client_sock, buffer, sizeof(buffer) - 1, 500)) > 0) {
        // Process the incoming request
        if (strstr(buffer, "POST /send") == buffer) {
            // Handle POST request
            // TODO: Add JSON parsing and message handling logic
            // Example: Responding with 200 OK
            tcp_send(client_sock, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nMessage received", 100, 500);
        } 
        else if (strstr(buffer, "GET /receive") == buffer) {
            // Handle GET request
            // TODO: Respond with the latest message
            tcp_send(client_sock, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nNo message available", 100, 500);
        } else {
            // Handle unknown request
            tcp_send(client_sock, "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nServer not found", 100, 500);
        }
    }

    // Handle disconnection
    printf("Client %s disconnected\n", client_id);
    tcp_close_channel(client_sock);
}

int server_start() {
    tcp_init();
    tcp_server *server = tcp_open_server("localhost", "3000", MAX_CLIENTS);

    printf("HTTP server started on port 3000\n");

    while (1) {
        tcp_channel *client_sock = tcp_accept(server, 500);
        if (client_sock) {
            handle_client(client_sock);
        }
    }

    tcp_close_server(server);
    tcp_term();
    return 0;
}
