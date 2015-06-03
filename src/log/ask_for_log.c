#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define SERVER_LISTEN_PORT 8000
#define BUFFER_SIZE        1024

int create_connection(char*);

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("You should input server IP address only!\n");
		exit(1);
	}
	
	int socket, length, written_length;
	char buffer[BUFFER_SIZE], *ip_addr;
	char log_name[18] = "server_status.log";
	ip_addr = argv[1];
	memset(buffer, 0, sizeof(buffer));

	socket = create_connection(ip_addr);
	buffer[0] = 'l';
	if (send(socket, buffer, BUFFER_SIZE, 0) < 0)
		perror("Sending asking log signal failed!");
	printf("Asking log signal transmitted!\n");
	memset(buffer, 0, sizeof(buffer));

	FILE* fp = fopen(log_name, "w");
	if (fp == NULL) {
		perror("Could not open to write!\n");
		return -1;
	}
	
	while (true) {
		length = recv(socket, buffer, BUFFER_SIZE, 0);
		if (length < 0) {
			perror("Receiving data failed!");
			break;
		}
		if (length == 0) break;
		written_length = fwrite(buffer, sizeof(char), length, fp);
		if (written_length < length)
			perror("File writing failed!");
		memset(buffer, 0, BUFFER_SIZE);
	}
	fclose(fp);
	return 0;
}

int create_connection(char* ip_addr) {
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_LISTEN_PORT);
	if (inet_aton(ip_addr, &server_addr.sin_addr) <= 0) {
		perror("Input IP is not correct!");
		exit(1);
	}

	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0) {
		perror("Could not create send_to_one_des socket!");
		exit(1);
	}

	socklen_t server_addr_length = sizeof(server_addr);
	if (connect(client_socket, (struct sockaddr*)&server_addr, server_addr_length) < 0) {
		perror("Could not connect!\n");
		exit(1);
	}
	printf("Connected to the server!\n");
	return client_socket;
}
