#include <stdbool.h>
#ifndef SERVER_TRANSFER_H
#define SERVER_TRANSFER_H

#define RECV_RETURN_BROADCASTNEW 0
#define RECV_RETURN_SEND_GRAPH_DEPTH_FIRST 1
#define RECV_RETURN_WRITE_GRAPH_BREADTH_FIRST 2
#define RECV_RETURN_ERROR 3

#define GRAPH_COLLECT_NO 1000

typedef struct broadcast {
	char ipAddr[250];
	int active;
} Broadcast;

void construct_broadcast(Broadcast* bc, const char* ip_addr, int act);

void send_file(int connected_socket);

int receive_file(int connected_socket);

void set_port();

void *send_to_one_des(void* _des);

void *send_to_des(void* _des);

void *server_listen_to_clients_handler();

int create_connection(Broadcast*);

void broadcast_graph();



extern struct broadcast* broadcast_list[100];
extern int desNum;
extern int recv_count;
extern int send_count;
extern int collect_count;
extern int gsize;
extern int clique_count;
#endif
