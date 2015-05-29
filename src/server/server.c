#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include <pthread.h>
#include "server_transfer.h"
#include "search.h"
int desNum = 0;
int max_graph_no;
int recv_count = 0;
int send_count = 0;
int collect_count = 0;
int gsize = 80;
int clique_count; // the clique count of the seeds we are currently waiting for
struct broadcast* broadcast_list[100];

void broadcast_graph();

int main(int argc, char* argv[]) {
	char dir_name[250];
	int *g;
	char des_file[250], incoming_ip_addr[20];
	char* existing_ip_addr;
	int p, err = 0;
	bool start_flag = true;
	struct sockaddr_in serv_addr;
	
	
	clique_count = RECURSION_THRESHOLD - 1;
	// upon initialization, put all files into seed_0

	
	
	// initialize server
	memset(&broadcast_list, '0', sizeof(broadcast_list));
	for (p = 0; p < 100; ++p)
		memset(&broadcast_list[p], '0', sizeof(broadcast_list[p]));
	set_port();


	int connectedSocket, gsize, count, i, exist,
		serv_socket = 0, recv_result;

	int iResult = 0;
	memset(&serv_addr, 0, sizeof(serv_addr));

	//Address family (must be AF_INET)
	serv_addr.sin_family = AF_INET;
	//IP port
	serv_addr.sin_port = htons(SERVER_LISTEN_PORT);
	//IP address
	//The in_addr structure represents an IPv4 Internet address.
	/*If an application does not care what local address is assigned,
	specify the constant value INADDR_ANY for an IPv4 local address.
	*/
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//If no error occurs, socket returns a descriptor referencing the
	//new socket. Otherwise, a value of INVALID_SOCKET is returned, and a specific error code can be retrieved by calling WSAGetLastError.
	serv_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_socket < 0) {
		printf("Could not create server_listen_to_clients_handler socket!\n");
		exit(1);
	}

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

	while (1) {
		printf("goint to waiting\n");
		connectedSocket = accept(serv_socket, (struct sockaddr*)&client_addr, &length);
		if (connectedSocket == 0) {
			perror("accept error :");
			break;
		}
		printf("connectedSocket:            %d\n", connectedSocket);

		//printf("server_listen_to_clients_handler did accept!\n");
		printf("\nThe connection from %s\n", inet_ntop(AF_INET, &(client_addr.sin_addr), incoming_ip_addr, 16));
		//printf("incoming_ip_addr = %s\n", incoming_ip_addr);

		for (i = 0; i < desNum; ++i) {
			existing_ip_addr = broadcast_list[i]->ipAddr;
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
			Broadcast* broadcast_target = (Broadcast*)malloc(sizeof(Broadcast));
			construct_broadcast(broadcast_target, incoming_ip_addr, 1);

			broadcast_list[desNum] = broadcast_target;

			printf("broadcast_list[%d] = broadcast_target", desNum);

			++desNum;
		}

		printf("Trying to recv!\n");
		recv_result = receive_file(connectedSocket);
		if (recv_result == RECV_RETURN_ERROR)
			printf("Return error, receive error\n");
		else if (recv_result == RECV_RETURN_SEND_GRAPH)
			send_file(connectedSocket);
		else if (recv_result == RECV_RETURN_BROADCASTNEW)
			broadcast_graph();
		else {
			ReadGraph("../../file/server/temp/temp", &g, &gsize);
			count = CliqueCount(g, gsize);
			if (count == 0) {
				sprintf(des_file, "../../file/server/ce_%d/%d", gsize, collect_count);
				copy("../../file/server/temp/temp", des_file);
				sprintf(des_file, "../../file/server/seed_%d/%d", clique_count % 2, collect_count++);
				copy("../../file/server/temp/temp", des_file);
			}
			else if (count <= clique_count){
				sprintf(des_file, "../../file/server/seed_%d/%d", clique_count % 2, collect_count++);
				copy("../../file/server/temp/temp", des_file);
			} // else leave the received file there because it is obsolete

			if (collect_count >= GRAPH_COLLECT_NO) {
				if (clique_count == 0){
					clique_count = RECURSION_THRESHOLD - 1;
					sprintf(dir_name, "../../file/server/ce_%d", ++gsize);
					mkdir(dir_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
					broadcast_graph();
				}
				else clique_count--;
				max_graph_no = GRAPH_COLLECT_NO;
			}
		}
		printf("finish receivning\n");
		close(connectedSocket);
	}
	close(serv_socket);
	return 0;
}

















			//printf("current received graphs: %d\n", collect_count);
			//printf("currently waiting for %d graphs\n", GRAPH_COLLECT_NO - collect_count);
			//printf("current maximum counterexample %d\n", gsize - 1);
