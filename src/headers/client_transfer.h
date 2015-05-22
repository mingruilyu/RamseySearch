#ifndef CLIENT_TRANSFER_H
#define CLIENT_TRANSFER_H

#define BUFFER_SIZE 1024
#define PORT 8000

typedef struct broadcast {
	char ipAddr[250];
	char fileName[250];
	int active;
} Broadcast;
typedef enum { false, true } bool;
void construct_broadcast(Broadcast* bc, const char* ip_addr, const char* file_name, int act);

void send_file(int connected_socket, char *filename);

void receive_file(int connected_socket, char *filename);

void set_port();

void *send_to_one_des(void* _des);

void *client_always_listen_to_one_handler(void* _file_name);

extern bool recv_flag;
extern bool first_connection;

#endif