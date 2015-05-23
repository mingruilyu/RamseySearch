#include <stdbool.h>
#ifndef SERVER_TRANSFER_H
#define SERVER_TRANSFER_H

typedef struct broadcast {
	char ipAddr[250];
	int active;
} Broadcast;

/*struct sockandfilename {
	int connectedSocket;
	char *fileName;
};*/

void construct_broadcast(Broadcast* bc, const char* ip_addr, int act);

void send_file(int connected_socket);

void send_check(int connected_socket);

int receive_file(int connected_socket);

void set_port();

void *send_to_one_des(void* _des);

void *send_to_des(void* _des);

void *server_listen_to_clients_handler();

extern struct broadcast* broadcast_list[100];
extern int active_count;
extern int collected_graph_count;
extern int send_count;
extern int collect_count;
extern int gsize;
extern int desNum;
#endif
