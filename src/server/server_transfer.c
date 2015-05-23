#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include <pthread.h>
#include "server_transfer.h"

#define BUFFER_SIZE 1024
#define PORT 8000

static int SERVER_LISTEN_PORT = -1;
static int CLIENT_LISTEN_PORT = -1;

void construct_broadcast(Broadcast* bc, const char* ip_addr, int act) {
	strcpy(bc->ipAddr, ip_addr);
	bc->active = act;
}

void send_file(int connected_socket) {
        char buffer[BUFFER_SIZE], filename[250];
		int file_block_length = 0;
        memset(buffer, '0', sizeof(buffer));
		printf("sending graph %d\n", send_count % collected_graph_count);
		sprintf(filename, "../../file/server/CE_%d/%d", gsize - 1, (send_count ++) % collected_graph_count) ;
		printf("reading file %s\n", filename);
		FILE * fp = fopen(filename, "r");
		if (fp == NULL) {
			printf("Could not open to read!\n");
			return;
		}
        else {
        	file_block_length = 0;
                //printf("Entering the read while block!\n");
			while (1) {
				file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp);
				if (file_block_length <= 0) {
					perror("fread error: ");
					break;
				}
				printf("file_block_length = %d\n", file_block_length);
				if (send(connected_socket, buffer, file_block_length, 0) < 0) {
					perror("Sending file failed! error: ");
					break;
				}
				memset(buffer, '0', sizeof(buffer));
			}
		}
		fclose(fp);
		printf("File transmitted!\n\n");
}

void send_check(int connected_socket) {
	char buffer[BUFFER_SIZE];
	memset(buffer, '0', sizeof(buffer));
	buffer[0] = 'c';
	if (send(connected_socket, buffer, 1, 0) < 0)
		printf("Sending check signal failed!\n");
	printf("check signal transmitted!\n\n");
}

int receive_file(int connected_socket) {
	char buffer[BUFFER_SIZE], filename[250];
	int written_length;
	memset(buffer, '0', sizeof(buffer));
	
	int length = recv(connected_socket, buffer, BUFFER_SIZE, 0);
	printf("buffer %s", buffer);
	if (length < 0) {
		printf("Receiving data failed!\n");
		return -1;
	}
	else if (buffer[0] == 'c') {
		return 1;
	}
	else {
		sprintf(filename, "../../file/server/CE_%d/%d", gsize, collect_count ++);
		FILE * fp = fopen(filename, "w");
		if (fp == NULL) {
			printf("Could not open to write!\n");
			return -1;
		}
		written_length = fwrite(buffer, sizeof(char), length, fp);
		if (written_length < length) printf("File writing failed!\n");
		memset(buffer, '0', BUFFER_SIZE);
		while ((length = recv(connected_socket, buffer, BUFFER_SIZE, 0)) != 0) {
			if (length < 0) {
				printf("Receiving data failed!\n");
				break;
			}

			written_length = fwrite(buffer, sizeof(char), length, fp);
			if (written_length < length) printf("File writing failed!\n");
			memset(buffer, '0', BUFFER_SIZE);
		}
		fclose(fp);
		printf("File receiveing finished!\n\n");
		return 0;
	}
}

void set_port() {
	srand(time(NULL) * 1000);
	SERVER_LISTEN_PORT = PORT;
	CLIENT_LISTEN_PORT = PORT;
}

void *send_to_one_des(void* _des) {
	struct broadcast* des = (struct broadcast*)_des;

	char* ip_addr = des->ipAddr;

	//printf("des->ipAddr: %s\n", ip_addr);
	//printf("des->fileName: %s\n", file_name);

	int iResult = 0;

	struct sockaddr_in client_addr;
	memset(&client_addr, '0', sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(0);
	client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int client_socket = 0;
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0) {
		printf("Could not create send_to_one_des socket!\n");
		exit(1);
	}
	//printf("send_to_one_des socket created!\n");

	iResult = bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr));
	if (iResult != 0) {
		perror("Could not bind send_to_one_des socket!\n");
		exit(1);
	}
	//printf("send_to_one_des socket bounded!\n");

	struct sockaddr_in server_addr;
	memset(&server_addr, '0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_LISTEN_PORT);

	if (inet_aton(ip_addr, &server_addr.sin_addr) <= 0) {
		printf("Input IP is not correct!\n");
		exit(1);
	}

	socklen_t server_addr_length = sizeof(server_addr);

	iResult = connect(client_socket, (struct sockaddr*)&server_addr, server_addr_length);
	if (iResult != 0) {
		printf("send_to_one_des could not connect!\n");
		exit(1);
	}
	//printf("send_to_one_des connected!\n");

	send_file(client_socket);

	close(client_socket);
	pthread_exit(0);
}

void *send_to_des(void* _des) {
	struct broadcast** des_list = (struct broadcast **)_des;

	int i;

	active_count = 0;

	for (i = 0; i < desNum; ++i) {
		int is_active = (*(des_list + i))->active;
		if (is_active == -1) continue;

		char* ip_addr = (*(des_list + i))->ipAddr;

		printf("ip_addr: %s\n", ip_addr);
		
		int iResult = 0;

		struct sockaddr_in client_addr;
		memset(&client_addr, '0', sizeof(client_addr));
		client_addr.sin_family = AF_INET;
		client_addr.sin_port = htons(0);
		client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

		int client_socket = 0;
		client_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (client_socket < 0) {
			printf("Could not create send_to_des socket!\n");
			exit(1);
		}
		//printf("send_to_des socket created!\n");

		iResult = bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr));
		if (iResult != 0) {
			perror("Could not bind send_to_des socket!\n");
			exit(1);
		}
		//printf("send_to_des socket bounded!\n");

		struct sockaddr_in server_addr;
		memset(&server_addr, '0', sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(CLIENT_LISTEN_PORT);
		
		if (inet_aton(ip_addr, &server_addr.sin_addr) <= 0) {
			printf("IP is not correct!\n");
			exit(1);
		}
		
		socklen_t server_addr_length = sizeof(server_addr);

		//printf("send_to_des tries to connect!\n");
		iResult = connect(client_socket, (struct sockaddr*)&server_addr, server_addr_length);
		if (iResult != 0) {
			printf("\nsend_to_des could not connect! %s has retreated!!!\n", ip_addr);
			(*(des_list + i))->active = -1;
			close(client_socket);
			continue;
		}
		++active_count;
		printf("send_to_des connected!\n");
		
		send_check(client_socket);

		close(client_socket);
	}
	pthread_exit(0);
}

void *server_listen_to_clients_handler() {
	int err = pthread_detach(pthread_self());
	if (err != 0) {
		perror("Could not pthread_detach!");
	}
	
	int iResult = 0;
	
	struct sockaddr_in serv_addr;
	memset(&serv_addr, '0', sizeof(serv_addr));

	//Address family (must be AF_INET)
	serv_addr.sin_family = AF_INET;
	//IP port
	serv_addr.sin_port = htons(SERVER_LISTEN_PORT);
	//IP address
	/*struct sockaddr_in {
	short sin_family;
	unsigned short sin_port;
	struct in_addr sin_addr;
	char sin_zero[8];
	};
	*/
	//The in_addr structure represents an IPv4 Internet address.
	/*If an application does not care what local address is assigned,
	specify the constant value INADDR_ANY for an IPv4 local address.
	*/
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int serv_socket = 0;
	//If no error occurs, socket returns a descriptor referencing the
	//new socket. Otherwise, a value of INVALID_SOCKET is returned, and a specific error code can be retrieved by calling WSAGetLastError.
	serv_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_socket < 0) {
		printf("Could not create server_listen_to_clients_handler socket!\n");
		exit(1);
	}

	/*int bind(
	_In_  SOCKET s,
	_In_  const struct sockaddr *name,
	_In_  int namelen
	);
	*/
	//name [in]
	//A pointer to a sockaddr structure of the local address to assign to the bound socket.
	iResult = bind(serv_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if (iResult != 0) {
		perror("Could not bind server_listen_to_clients_handler socket!\n");
		exit(1);
	}

	printf("server_listen_to_clients_handler starts to listen!\n");
	//If no error occurs, listen returns zero.
	//TODO: Listen to 20? OK or not
	if (listen(serv_socket, 20)) {
		printf("server_listen_to_clients_handler listening failed!\n");
		exit(1);
	}
	
	struct sockaddr_in client_addr;
	memset(&client_addr, '0', sizeof(client_addr));
	socklen_t length = sizeof(client_addr);

	//printf("\nserver_listen_to_clients_handler starts to accept!\n");
	
	int connectedSocket;
	
	while ((connectedSocket = accept(serv_socket, (struct sockaddr*)&client_addr, &length)) != 0) {
		//printf("server_listen_to_clients_handler did accept!\n");
		char incoming_ip_addr[20];
		printf("\nThe connection from %s\n", inet_ntop(AF_INET, &(client_addr.sin_addr), incoming_ip_addr, 16));
		//printf("incoming_ip_addr = %s\n", incoming_ip_addr);
		
		int exist = 0, i;
		for (i = 0; i < desNum; ++i) {
			//if (broadcast_list[i]->active == -1) continue;
			char* existing_ip_addr = broadcast_list[i]->ipAddr;
			if (strcmp(incoming_ip_addr, existing_ip_addr) == 0) {
				printf("This IP is already in the list!\n");
				printf("broadcast_list[%d]->ipAddr = %s\n", i, incoming_ip_addr);
				printf("desNum = %d\n", desNum);
				if (broadcast_list[i]->active == -1)
					broadcast_list[i]->active = 1;
				exist = 1;
				break;
			}
		}

		if (exist == 0) {
			Broadcast* broadcast_target = (Broadcast*) malloc(sizeof(Broadcast));
			construct_broadcast(broadcast_target, incoming_ip_addr, 1);

			//printf("broadcast_target->ipAddr: %s\n", broadcast_target->ipAddr);
			//printf("broadcast_target->fileName: %s\n", broadcast_target->fileName);

			broadcast_list[desNum] = broadcast_target;

			printf("broadcast_list[%d] = broadcast_target", desNum);

			++desNum;
		}

		/*err = pthread_create(&thread_id, NULL, connection_handler, (void*)&connectedSocket) != 0;
		if (err != 0) {
			printf("Could not create thread! errno: %d \n", errno);
			perror("Could not create thread!");
			continue;
		}
		pthread_join(thread_id, NULL);*/
		int recv_result = receive_file(connectedSocket);
		if (recv_result == -1) printf("Return -1, receive error\n");
		else if (recv_result == 1) send_file(connectedSocket);
		close(connectedSocket);
	}
	close(serv_socket);
	pthread_exit(0);
}

/*void *connection_handler(void* connected_info) {
	struct sockandfilename sock = *(struct sockandfilename*)connected_info;
	int connected_socket = 
	int connected_sock = sock.connectedSocket;
	
	//printf("This is connection_handler thread. Starts to receive file.\n");
	receive_file(connected_sock);
	
	close(connected_sock);
	pthread_exit(0);
}*/

int create_connection(Broadcast* des) {
	struct sockaddr_in server_addr;
	memset(&server_addr, '0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_LISTEN_PORT);
	if (inet_aton(des->ipAddr, &server_addr.sin_addr) <= 0) {
		printf("Input IP is not correct!\n");
		exit(1);
	}

	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0) {
		printf("Could not create send_to_one_des socket!\n");
		exit(1);
	}

	socklen_t server_addr_length = sizeof(server_addr);

	if (connect(client_socket, (struct sockaddr*)&server_addr, server_addr_length) < 0) {
		printf("send_to_one_des could not connect!\n");
		exit(1);
	}
	printf("Connected to the server!\n");
	return client_socket;
}
