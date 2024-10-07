#include "networking/server.h"
#include "khg_tcp/tcp.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

Client clients[MAX_CLIENTS];
int client_count = 0;
int client_id_counter = 16777215;

// Helper to generate a unique client ID in hex format
void get_client_id(char *client_id) {
    snprintf(client_id, 7, "%06X", client_id_counter);
    client_id_counter--;
}

// Function to handle incoming data from the client
void handle_client_data(Client *client, const char *data) {
    if (strncmp(data, "POST /send", 10) == 0) {
        const char *body = strstr(data, "\r\n\r\n") + 4;
        // Parse the message (assuming valid JSON in the body)
        char receiver[10], message[BUFFER_SIZE];
        if (sscanf(body, "{\"message\":\"%[^:]:%[^\"]\"}", receiver, message) == 2) {
            // Find the target client by their ID and send the message
            for (int i = 0; i < client_count; i++) {
                if (strcmp(clients[i].id, receiver) == 0) {
                    char response[BUFFER_SIZE];
                    snprintf(response, BUFFER_SIZE, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n%s", message);
                    tcp_send(clients[i].sock, response, strlen(response), 1000);
                }
            }
        } else {
            // Malformed JSON, respond with 400 error
            const char *error_response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nInvalid JSON";
            tcp_send(client->sock, error_response, strlen(error_response), 1000);
        }
    } else if (strncmp(data, "GET /receive", 12) == 0) {
        // Send the stored message back to the client
        const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nNo message available";
        tcp_send(client->sock, response, strlen(response), 1000);
    } else {
        // Unknown request, respond with 404
        const char *response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nNot Found";
        tcp_send(client->sock, response, strlen(response), 1000);
    }
}

int server_start() {
    tcp_init();

    /*tcp_server *server = tcp_open_server("165.22.176.143", "3000", MAX_CLIENTS);*/
    tcp_server *server = tcp_open_server("localhost", "3000", MAX_CLIENTS);
    printf("HTTP server started on port 3000\n");

    while (1) {
        tcp_channel *new_client = tcp_accept(server, 1000);
        if (new_client) {
            // Assign a new ID to the connected client
            Client client;
            get_client_id(client.id);
            client.sock = new_client;

            printf("Client %s connected\n", client.id);
            clients[client_count++] = client;

            char buffer[BUFFER_SIZE];
            int received = tcp_receive(client.sock, buffer, BUFFER_SIZE, 1000);
            if (received > 0) {
                buffer[received] = '\0'; // Null-terminate the buffer
                handle_client_data(&client, buffer);
            }

            tcp_close_channel(client.sock);
            client_count--;
            client_id_counter++; // Increment ID counter back when client disconnects
            printf("Client %s disconnected\n", client.id);
        }
    }

    tcp_close_server(server);
    tcp_term();
    return 0;
}
