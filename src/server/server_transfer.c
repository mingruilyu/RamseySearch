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
#include "clique_count.h"
#include "graph.h"
#define BUFFER_SIZE 1024
#define PORT 8000

static int SERVER_LISTEN_PORT = -1;
static int CLIENT_LISTEN_PORT = -1;

void copy(char* source_file, char* target_file);
void construct_broadcast(Broadcast* bc, const char* ip_addr, int act) {
	strcpy(bc->ipAddr, ip_addr);
	bc->active = act;
}

void send_file(int connected_socket) {
		TODO: send twice
        char buffer[BUFFER_SIZE], filename[250];
		int file_block_length = 0;
        memset(buffer, '0', sizeof(buffer));
		printf("sending graph %d\n", send_count % GRAPH_COLLECT_NO);
		sprintf(filename, "../../file/server/seed_%d/%d", (clique_count + 1) % 2, send_count ++);
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

int receive_file(int connected_socket) {
	char buffer[BUFFER_SIZE], filename[250];
	int written_length;
	memset(buffer, '0', sizeof(buffer));
	
	int length = recv(connected_socket, buffer, BUFFER_SIZE, 0);
	printf("buffer %s", buffer);
	if (length < 0) {
		printf("Receiving data failed!\n");
		return RECV_RETURN_ERROR;
	}
	else if (buffer[0] == 'c') {
		return RECV_RETURN_SEND_GRAPH;
	}
	else if (buffer[0] == 'r') {
		recv_count++;
		if (send_count == max_graph_count && recv_count == max_graph_count) {
			max_graph_no = collect_count;
			return RECV_RETURN_BROADCASTNEW;
		}
		else
			return RECV_RETURN_SEND_GRAPH;
	}
	else {
		sprintf(filename, "../../file/server/temp/temp");
		printf("receiving filename: %s \n", filename);
		FILE * fp = fopen(filename, "w");
		if (fp == NULL) {
			perror("Could not open to write! fopen error: ");
			return RECV_RETURN_ERROR;
		}
		written_length = fwrite(buffer, sizeof(char), length, fp);
		if (written_length < length) printf("File writing failed!\n");
		memset(buffer, '0', BUFFER_SIZE);
		while (1) {
			length = recv(connected_socket, buffer, BUFFER_SIZE, 0);
 			if(!length) break;
			else if (length < 0) {
				perror("Receiving data failed! recv error: ");
				return RECV_RETURN_ERROR;
			}
			
			printf("buffer: %s\n", buffer);
			written_length = fwrite(buffer, sizeof(char), length, fp);
			printf("written length: %d\n", written_length);
			if (written_length < length) perror("File writing failed! :");
			memset(buffer, '0', BUFFER_SIZE);
		}
		fclose(fp);
		printf("File receiveing finished!\n\n");
		return RECV_RETURN_WRITE_GRAPH;
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

	int iResult = 0;

	int client_socket = 0;
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0) {
		printf("Could not create send_to_one_des socket!\n");
		exit(1);
	}

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

	for (i = 0; i < desNum; ++i) {
		int is_active = (*(des_list + i))->active;
		if (is_active == -1) continue;

		char* ip_addr = (*(des_list + i))->ipAddr;

		printf("ip_addr: %s\n", ip_addr);
		
		int iResult = 0, client_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (client_socket < 0) {
			printf("Could not create send_to_des socket!\n");
			exit(1);
		}

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
		printf("send_to_des connected!\n");

		close(client_socket);
	}
	pthread_exit(0);
}

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

void broadcast_graph() {
	TODO: delete send directory
	int i, err;
	collect_count = 0;
	recv_count = 0;
	for (i = 0; i < desNum; i++) {
		int is_active = (*(broadcast_list + i))->active;
		if (is_active == -1) continue;

		char* ip_addr = (*(broadcast_list + i))->ipAddr;

		Broadcast* tmp = (Broadcast*)malloc(sizeof(Broadcast));
		construct_broadcast(tmp, ip_addr, 1);

		printf("ip_addr: %s\n", ip_addr);

		pthread_t sock_send_thread_id;
		err = pthread_create(&sock_send_thread_id, NULL, send_to_one_des, (void*)tmp);
		if (err != 0) perror("Could not create send_to_one_des thread!");

		err = pthread_join(sock_send_thread_id, NULL);
		if (err != 0) {
			printf("sock_thread goes wrong! %s \n", strerror(err));
			perror("sock_thread goes wrong!");
		}
		free(tmp);
	}
}

void copy(char* source_file, char* target_file) {
	char ch;
	FILE* source, *target;


	source = fopen(source_file, "r");

	if (source == NULL)
	{
		printf("Press any key to exit...\n");
		return;
	}

	target = fopen(target_file, "w");
	if (target == NULL) {
		perror("Could not open to write! fopen error: ");
		return;
	}

	while ((ch = fgetc(source)) != EOF)
		fputc(ch, target);

	printf("File copied successfully.\n");

	fclose(source);
	fclose(target);
}
