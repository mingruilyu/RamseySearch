#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <pthread.h>

#define BUFFER_SIZE 1024
#define PORT 8000

static int SERVER_LISTEN_PORT = -1;
static int CLIENT_LISTEN_PORT = -1;

static int desNum = 0;
struct broadcast* broadcast_list[20];

void *connection_handler(void *);

struct broadcast {
	char *ipAddr;
	char *fileName;
};

struct sockandfilename {
	int connectedSocket;
	char *fileName;
};

void send_file(int connected_socket, char *filename) {
        char buffer[BUFFER_SIZE];
        memset(buffer, '0', sizeof(buffer));

        FILE *fp;
        fp = fopen(filename, "r");
        if (fp == NULL) {
        	printf("Could not open to read!\n");
	}
        else {
        	int file_block_length = 0;
                printf("Entering the read while block!\n");
                while ((file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0) {
                	printf("file_block_length = %d\n", file_block_length);

                        if (send(connected_socket, buffer, file_block_length, 0) < 0) {
                        	printf("Sending file failed!\n");
                                break;
                        }

                        memset(buffer, '0', sizeof(buffer));
		}
			fclose(fp);
			printf("File transmitted!\n\n");
	}
}

void receive_file(int connected_socket, char *filename) {
	char buffer[BUFFER_SIZE];
	memset(buffer, '0', sizeof(buffer));

	FILE *fp;
	fp = fopen(filename, "w");
	if (fp == NULL) {
		printf("Could not open to write!\n");
	}
	else {
		printf("Starting to receive!\n");
		int length = 0;
		while (length = recv(connected_socket, buffer, BUFFER_SIZE, 0)) {
			if (length < 0) {
				printf("Receiving data failed!\n");
				break;
			}

			int written_length = fwrite(buffer, sizeof(char), length, fp);
			if (written_length < length) {
				printf("File writing failed!\n");
				break;
			}
			memset(buffer, '0', BUFFER_SIZE);
		}
		fclose(fp);

		printf("File receiveing finished!\n\n");
	}
}

void set_port() {
	srand(time(NULL) * 1000);
	SERVER_LISTEN_PORT = PORT;
	CLIENT_LISTEN_PORT = PORT + 1;
}

void *send_to_one_des(void* _des) {
	struct broadcast des = *(struct broadcast*)_des;
	
	char *ip_addr = des.ipAddr;
	char *file_name = des.fileName;
	
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
	printf("send_to_one_des socket created!\n");

	iResult = bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr));
	if (iResult != 0) {
		perror("Could not bind send_to_one_des socket!\n");
		exit(1);
	}
	printf("send_to_one_des socket bounded!\n");

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
	printf("send_to_one_des connected!\n");

	send_file(client_socket, file_name);
	
	close(client_socket);
	pthread_exit(0);
}

void *send_to_des(void* _des) {
	struct broadcast** des_list = (struct broadcast **)_des;

	int i;

	for (i = 0; i < desNum; ++i) {
		char *ip_addr = (*(des_list + i))->ipAddr;
		char *file_name = (*(des_list + i))->fileName;
		
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
		printf("send_to_des socket created!\n");

		iResult = bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr));
		if (iResult != 0) {
			perror("Could not bind send_to_des socket!\n");
			exit(1);
		}
		printf("send_to_des socket bounded!\n");

		struct sockaddr_in server_addr;
		memset(&server_addr, '0', sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(CLIENT_LISTEN_PORT);
		
		if (inet_aton(ip_addr, &server_addr.sin_addr) <= 0) {
			printf("No.%d input IP is not correct!\n", i);
			exit(1);
		}

		socklen_t server_addr_length = sizeof(server_addr);

		iResult = connect(client_socket, (struct sockaddr*)&server_addr, server_addr_length);
		if (iResult != 0) {
			printf("send_to_des could not connect!\n");
			exit(1);
		}
		printf("send_to_des connected!\n");
		
		send_file(client_socket, file_name);

		close(client_socket);
	}
	pthread_exit(0);
}

void *server_listen_to_clients_handler(void* _file_name) {
	int err = pthread_detach(pthread_self());
	if (err != 0) {
		perror("Could not pthread_detach!");
	}
	
	char* file_name = (char*)_file_name;

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
	if (listen(serv_socket, 20)) {
		printf("server_listen_to_clients_handler listening failed!\n");
		exit(1);
	}
	
	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);

	printf("\nserver_listen_to_clients_handler starts to accept!\n");

	pthread_t thread_id;
	
	struct sockandfilename sf;
	sf.fileName = file_name;
	
	while (sf.connectedSocket = accept(serv_socket, (struct sockaddr*)&client_addr, &length)) {
		printf("server_listen_to_clients_handler did accept!\n");
		char incoming_ip_addr[20];
		printf("The connection from %s\n", inet_ntop(AF_INET, &(client_addr.sin_addr), incoming_ip_addr, 16));
		printf("incoming_ip_addr = %s\n", incoming_ip_addr);

		err = pthread_create(&thread_id, NULL, connection_handler, (void*)&sf) != 0;
		if (err != 0) {
			printf("Could not create thread! errno: %d \n", errno);
			perror("Could not create thread!");
			continue;
		}
		pthread_join(thread_id, NULL);
	}
	close(serv_socket);
	pthread_exit(0);
}

void *client_always_listen_to_one_handler(void* _file_name) {
	int err = pthread_detach(pthread_self());
	if (err != 0) {
		perror("Could not pthread_detach!");
	}
	
	char* file_name = (char*)_file_name;

	int iResult = 0;

	struct sockaddr_in serv_addr;
	memset(&serv_addr, '0', sizeof(serv_addr));

	//Address family (must be AF_INET)
	serv_addr.sin_family = AF_INET;
	//IP port
	serv_addr.sin_port = htons(CLIENT_LISTEN_PORT);
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
		printf("Could not create client_always_listen_to_one_handler socket!\n");
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
		perror("Could not bind client_always_listen_to_one_handler socket!\n");
		exit(1);
	}

	printf("client_always_listen_to_one_handler starts to listen!\n");
	//If no error occurs, listen returns zero.
	if (listen(serv_socket, 1)) {
		printf("client_always_listen_to_one_handler listening failed!\n");
		exit(1);
	}

	while (1) {
		struct sockaddr_in client_addr;
		socklen_t length = sizeof(client_addr);

		int connected_socket;
		printf("\nStarts to accept!\n");
		if ((connected_socket = accept(serv_socket, (struct sockaddr*)&client_addr, &length)) == -1) {
			printf("Accepting failed!\n");
			exit(1);
		}
		
		char incoming_ip_addr[20];
		printf("Got connection from %s\n", inet_ntop(AF_INET, &(client_addr.sin_addr), incoming_ip_addr, 16));
		
		receive_file(connected_socket, file_name);
		
		close(connected_socket);
	}
	close(serv_socket);
	pthread_exit(0);
}

void *connection_handler(void* connected_info) {
	struct sockandfilename sock = *(struct sockandfilename*)connected_info;
	
	int connected_sock = sock.connectedSocket;
	char* file_name = sock.fileName;
	
	printf("This is connection_handler thread. Starts to receive file.\n");
	receive_file(connected_sock, file_name);
	
	close(connected_sock);
	pthread_exit(0);
}
