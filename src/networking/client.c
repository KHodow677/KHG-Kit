#include "networking/client.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define SERVER_ADDRESS "165.22.176.143"
#define SERVER_PORT "80"
#define TIMEOUT_MS 5000
#define BUFFER_SIZE 2048

// Function to send an HTTP POST request
bool send_post_request(tcp_channel *channel, const char *receiver_id, const char *message) {
    char buffer[BUFFER_SIZE];
    char post_body[BUFFER_SIZE];

    // Prepare POST body
    snprintf(post_body, sizeof(post_body), "{\"message\": \"%s:%s\"}", receiver_id, message);

    // Prepare POST request
    snprintf(buffer, sizeof(buffer),
             "POST /send HTTP/1.1\r\n"
             "Host: %s:%s\r\n"
             "Content-Type: application/json\r\n"
             "Content-Length: %zu\r\n\r\n%s",
             SERVER_ADDRESS, SERVER_PORT, strlen(post_body), post_body);

    // Send POST request
    if (!tcp_send(channel, buffer, strlen(buffer), TIMEOUT_MS)) {
        fprintf(stderr, "Failed to send POST request\n");
        return false;
    }
    return true;
}

// Function to send an HTTP GET request
bool send_get_request(tcp_channel *channel) {
    char buffer[BUFFER_SIZE];

    // Prepare GET request
    snprintf(buffer, sizeof(buffer),
             "GET /receive HTTP/1.1\r\n"
             "Host: %s:%s\r\n\r\n",
             SERVER_ADDRESS, SERVER_PORT);

    // Send GET request
    if (!tcp_send(channel, buffer, strlen(buffer), TIMEOUT_MS)) {
        fprintf(stderr, "Failed to send GET request\n");
        return false;
    }
    return true;
}

// Function to receive and print server response
void receive_response(tcp_channel *channel) {
    char buffer[BUFFER_SIZE];
    int received_length = tcp_receive(channel, buffer, BUFFER_SIZE - 1, TIMEOUT_MS);

    if (received_length > 0) {
        buffer[received_length] = '\0';  // Null-terminate the response
        printf("Server Response:\n%s\n", buffer);
    } else {
        fprintf(stderr, "Failed to receive response or timed out\n");
    }
}

int client_start() {
    // Initialize the TCP library
    if (!tcp_init()) {
        fprintf(stderr, "Failed to initialize TCP\n");
        return 1;
    }

    // Connect to the server
    tcp_channel *channel = tcp_connect(SERVER_ADDRESS, SERVER_PORT);
    if (!channel) {
        fprintf(stderr, "Failed to connect to server\n");
        tcp_term();
        return 1;
    }

    // Send POST request to a receiver with ID "FF0000" and a message
    if (send_post_request(channel, "FF0000", "Hello, Receiver!")) {
        receive_response(channel);  // Receive the server's response
    }

    // Send GET request to receive any message available
    if (send_get_request(channel)) {
        receive_response(channel);  // Receive the server's response
    }

    // Close the connection and terminate TCP
    tcp_close_channel(channel);
    tcp_term();

    return 0;
}
