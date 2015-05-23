#include <stdbool.h>
#ifndef CLIENT_TRANSFER_H
#define CLIENT_TRANSFER_H

#define BUFFER_SIZE 1024
#define PORT 8000

typedef struct broadcast {
	char ipAddr[250];
	int active;
} Broadcast;

void construct_broadcast(Broadcast* bc, const char* ip_addr, int act);

void send_file(int connected_socket);

void send_check(int connected_socket);

void receive_file(int connected_socket);

void set_port();

void *client_always_listen_to_one_handler();

int create_connection(Broadcast*);

extern bool recv_flag;
extern bool first_connection;
extern int new_graph_count;
extern int one_more_flag;

#endif
