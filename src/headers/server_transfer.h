#ifndef SERVER_TRANSFER_H
#define SERVER_TRANSFER_H

typedef struct broadcast {
	char ipAddr[250];
	char fileName[250];
	int active;
} Broadcast;

struct sockandfilename {
	int connectedSocket;
	char *fileName;
};

void construct_broadcast(Broadcast* bc, const char* ip_addr, const char* file_name, int act);

void send_file(int connected_socket, char *filename);

void receive_file(int connected_socket, char *filename);

void set_port();

void *send_to_des(void* _des);

void *server_listen_to_clients_handler(void* _file_name);



#endif