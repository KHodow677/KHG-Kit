#include "chat.h"
#include "chat_common.h"
#include "khg_net/net.h"
#include <stdbool.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif

#define MAX_CLIENTS 16

typedef struct {
	ENetHost *host;
	net_socket listen;
} ENetLANServer;

volatile sig_atomic_t stop = 0;
void sigint_handle(int signum);
bool start_server(ENetLANServer *server);
void listen_for_clients(ENetLANServer *server);
void server_send_string(ENetHost *host, char *s);
void stop_server(ENetLANServer *server);

int server_test() {
	signal(SIGINT, sigint_handle);
	ENetLANServer server;
	if (!start_server(&server)) {
		return 1;
	}
	int check;
	do {
		listen_for_clients(&server);
		ENetEvent event;
		check = enet_host_service(server.host, &event, 0);
		if (check > 0) {
			char buf[256];
			switch (event.type) {
				case ENET_EVENT_TYPE_CONNECT:
					sprintf(buf, "New client connected: id %d", event.peer->incomingPeerID);
					server_send_string(server.host, buf);
					printf("%s\n", buf);
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					sprintf(buf, "Client %d says: %s", event.peer->incomingPeerID, event.packet->data);
					server_send_string(server.host, buf);
					printf("%s\n", buf);
					break;
				case ENET_EVENT_TYPE_DISCONNECT:
					sprintf(buf, "Client %d disconnected", event.peer->incomingPeerID);
					server_send_string(server.host, buf);
					printf("%s\n", buf);
					break;
				default:
					break;
			}
		}
		else if (check < 0) {
			fprintf(stderr, "Error servicing host\n");
		}
		Sleep(1);
	} while (!stop && check >= 0);
	stop_server(&server);
	return 0;
}

void sigint_handle(int signum) {
	if (signum == SIGINT) {
		stop = 1;
	}
}

bool start_server(ENetLANServer *server) {
	if (enet_initialize() != 0) {
		fprintf(stderr, "An error occurred while initializing ENet\n");
		return false;
	}
	server->listen = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM);
	if (server->listen == NET_SOCKET_NULL) {
		fprintf(stderr, "Failed to create socket\n");
		return false;
	}
	if (enet_socket_set_option(server->listen, ENET_SOCKOPT_REUSEADDR, 1) != 0) {
		fprintf(stderr, "Failed to enable reuse address\n");
		return false;
	}
	ENetAddress listenaddr;
	listenaddr.host = ENET_HOST_ANY;
	listenaddr.port = LISTEN_PORT;
	if (enet_socket_bind(server->listen, &listenaddr) != 0) {
		fprintf(stderr, "Failed to bind listen socket\n");
		return false;
	}
	if (enet_socket_get_address(server->listen, &listenaddr) != 0) {
		fprintf(stderr, "Cannot get listen socket address\n");
		return false;
	}
	printf("Listening for scans on port %d\n", listenaddr.port);
	ENetAddress addr;
	addr.host = ENET_HOST_ANY;
	addr.port = ENET_PORT_ANY;
	server->host = enet_host_create(&addr, MAX_CLIENTS, 2, 0, 0);
	if (server->host == NULL) {
		fprintf(stderr, "Failed to open ENet host\n");
		return false;
	}
	printf("ENet host started on port %d (press ctrl-C to exit)\n", server->host->address.port);
	return true;
}

void listen_for_clients(ENetLANServer *server) {
	net_socket_set set;
	net_socketset_empty(set);
	net_socketset_add(set, server->listen);
	if (enet_socketset_select(server->listen, &set, NULL, 0) <= 0) {
		return;
	}
	ENetAddress recvaddr;
	char buf;
	net_buffer recvbuf;
	recvbuf.data = &buf;
	recvbuf.dataLength = 1;
	const int recvlen = enet_socket_receive(server->listen, &recvaddr, &recvbuf, 1);
	if (recvlen <= 0) {
		return;
	}
	char addrbuf[256];
	enet_address_get_host_ip(&recvaddr, addrbuf, sizeof addrbuf);
	printf("Listen port: received (%d) from %s:%d\n", *(char *)recvbuf.data, addrbuf, recvaddr.port);
	ServerInfo sinfo;
	if (enet_address_get_host(&server->host->address, sinfo.hostname, sizeof sinfo.hostname) != 0) {
		fprintf(stderr, "Failed to get hostname\n");
		return;
	}
	sinfo.port = server->host->address.port;
	recvbuf.data = &sinfo;
	recvbuf.dataLength = sizeof sinfo;
	if (enet_socket_send(server->listen, &recvaddr, &recvbuf, 1) != (int)recvbuf.dataLength) {
		fprintf(stderr, "Failed to reply to scanner\n");
	}
}

void server_send_string(ENetHost *host, char *s) {
	ENetPacket *packet = enet_packet_create(s, strlen(s) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(host, 0, packet);
}

void stop_server(ENetLANServer *server) {
	printf("Server closing\n");
	if (enet_socket_shutdown(server->listen, ENET_SOCKET_SHUTDOWN_READ_WRITE) != 0) {
		fprintf(stderr, "Failed to shutdown listen socket\n");
	}
	enet_socket_destroy(server->listen);
	enet_host_destroy(server->host);
	enet_deinitialize();
}
