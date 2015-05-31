#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include <pthread.h>
#include "server_transfer.h"
#include "search.h"
#include "graph.h"
#include "clique_count.h"

#define IP_RANGE 25

int send_count = 0; // mutex
int collect_count = 0; // mutex
int gsize = 107;
int clique_count;//mutex // the clique count of the seeds we are currently waiting for

pthread_mutex_t send_count_mutex;
pthread_mutex_t collect_count_mutex;
pthread_mutex_t clique_count_mutex;
pthread_mutex_t gsize_mutex;

struct broadcast* broadcast_list[100];

void broadcast_graph();
void *listener_handler(void*);
void *server_print_handler();
void check_ip_list(char*);

int main(int argc, char* argv[]) {
	int p, err = 0;
	
	clique_count = RECURSION_THRESHOLD - 1;
	// upon initialization, put all files into seed_0

	// initialize server
	memset(&broadcast_list, 0, sizeof(broadcast_list));
	for (p = 0; p < 100; ++p) {
		broadcast_list[p] = NULL;
	}
	srand(time(NULL) * 1000);

	pthread_t server_print_thread_id;
	err = pthread_create(&server_print_thread_id, NULL, server_print_handler, NULL);
	if (err != 0) perror("Could not create server_print_thread_id thread!");
	
	for (p = 0; p < 4; ++p) {
		pthread_t listener_thread;
		err = pthread_create(&listener_thread, NULL, listener_handler, (void*)&p);
		if (err != 0) perror("Could not create listener_thread thread!");
	}
	while (1) {
		if (collect_count >= GRAPH_COLLECT_NO) {
			if (clique_count == 0){
				pthread_mutex_lock(clique_count_mutex);
				clique_count = RECURSION_THRESHOLD - 1;
				pthread_mutex_unlock(clique_count_mutex);
				pthread_mutex_lock(gsize_mutex);
				pthread_mutex_lock(gsize_mutex);
				sprintf(dir_name, "../../file/server/ce_%d", ++gsize);
				pthread_mutex_unlock(gsize_mutex);
				mkdir(dir_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			}
			else {
				pthread_mutex_lock(clique_count_mutex);
				clique_count--;
				pthread_mutex_unlock(clique_count_mutex);
			}
			// clean seed file
			pthread_mutex_lock(clique_count_mutex);
			sprintf(dir_name, "../../file/server/seed_%d", clique_count % 2);
			pthread_mutex_unlock(clique_count_mutex);
			printf("cleaning %s\n", dir_name);
			delete_graph(dir_name);
			broadcast_graph();
		}
		sleep(5);
	}
	return 0;
}

void *listener_handler(void* thread_id) {
	int connectedSocket, count, i,
		serv_socket = 0, recv_result;
	int *g;

	char incoming_ip_addr[20];
	char des_file[250];
	char dir_name[250];
	
	int id = *(int*)thread_id, desNum = 0;

	int server_listener_port = set_port(id);

	int iResult = 0;
	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(server_listener_port);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	serv_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_socket < 0) {
		perror("Could not create server_listen_to_clients_handler socket!");
		exit(1);
	}

	//A pointer to a sockaddr structure of the local address to assign to the bound socket.
	iResult = bind(serv_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if (iResult != 0) {
		perror("Could not bind server_listen_to_clients_handler socket!");
		exit(1);
	}

	printf("server_listen_to_clients_handler starts to listen!\n");
	//If no error occurs, listen returns zero.
	if (listen(serv_socket, 20)) {
		perror("server_listen_to_clients_handler listening failed!");
		exit(1);
	}

	struct sockaddr_in client_addr;
	memset(&client_addr, '0', sizeof(client_addr));
	socklen_t length = sizeof(client_addr);

	while (1) {
		printf("Going to waiting\n");
		connectedSocket = accept(serv_socket, (struct sockaddr*)&client_addr, &length);
		if (connectedSocket == 0) {
			perror("accept:");
			break;
		}
		printf("connectedSocket: %d\n", connectedSocket);

		//printf("server_listen_to_clients_handler did accept!\n");
		printf("\nThe connection from %s\n", inet_ntop(AF_INET, &(client_addr.sin_addr), incoming_ip_addr, 16));
		//printf("incoming_ip_addr = %s\n", incoming_ip_addr);
		
		check_ip_list(incoming_ip_addr, id, &desNum);

		printf("Trying to recv!\n");
		recv_result = receive_file(connectedSocket);
		pthread_mutex_unlock(send_count_mutex);
		if (recv_result == RECV_RETURN_ERROR)
			printf("Return error, receive error\n");
		else if (recv_result == RECV_RETURN_SEND_GRAPH_DEPTH_FIRST) {
			if (collect_count == 0)
				send_file(connectedSocket, SEARCH_MODE_DEPTH_FIRST, BROADCAST_RANDOM_RESTART);
			//broadcast_graph(SEARCH_MODE_DEPTH_FIRST, BROADCAST_RANDOM_RESTART);
			else
				send_file(connectedSocket, SEARCH_MODE_DEPTH_FIRST, BROADCAST_RANDOM_CONTINUE);
			//	broadcast_graph(SEARCH_MODE_DEPTH_FIRST, BROADCAST_RANDOM_CONTINUE);
		}
		else if (recv_result == RECV_RETURN_SEND_GRAPH_BREADTH_FIRST)
			send_file(connectedSocket, SEARCH_MODE_BREADTH_FIRST, BROADCAST_ORDER);
		else {`
			pthread_mutex_lock(gsize_mutex);
		if (ReadGraph("../../file/server/temp/temp", &g, &gsize)){
			count = CliqueCount(g, gsize);
			if (count == 0) {
				sprintf(des_file, "../../file/server/ce_%d/%d", gsize, collect_count);
				copy("../../file/server/temp/temp", des_file);
				pthread_mutex_lock(collect_count_mutex);
				sprintf(des_file, "../../file/server/seed_%d/%d", clique_count % 2, collect_count++);
				pthread_mutex_lock(clique_count_mutex);
				pthread_mutex_unlock(collect_count_mutex);
				copy("../../file/server/temp/temp", des_file);
			}
			else if (count <= clique_count){
				pthread_mutex_lock(collect_count_mutex);
				sprintf(des_file, "../../file/server/seed_%d/%d", clique_count % 2, collect_count++);
				pthread_mutex_unlock(collect_count_mutex);
				copy("../../file/server/temp/temp", des_file);
			} // else leave the received file there because it is obsolete
		}


			pthread_mutex_unlock(gsize_mutex);
		}
		printf("finish receivning\n");
		close(connectedSocket);
	}
	printf("Out of loop\n");
	close(serv_socket);
	pthread_exit(0);
}

void *server_print_handler() {
	while (1) {
		printf("send_count: %d\n", send_count);
		printf("clique_count: %d\n", clique_count);
		printf("currently waiting for %d graphs\n", GRAPH_COLLECT_NO - collect_count);
		printf("current maximum counterexample %d\n", gsize);
		sleep(5);
	}
	pthread_exit(0);
}

void check_ip_list(char* incoming_ip_addr, int id, int *desNum) {
	int exist = 0, i;
	for (i = id * IP_RANGE; i < (id * IP_RANGE + *desNum); ++i) {
		if (strcmp(incoming_ip_addr, broadcast_list[i]->ipAddr) == 0) {
			printf("This IP is already in the list!\n");
			printf("broadcast_list[%d]->ipAddr = %s\n", i, incoming_ip_addr);
			printf("desNum = %d\n", *desNum);
			exist = 1;
			break;
		}
	}

	if (exist == 0) {
		Broadcast* broadcast_target = (Broadcast*)malloc(sizeof(Broadcast));
		construct_broadcast(broadcast_target, incoming_ip_addr, 1);

		broadcast_list[id * IP_RANGE + *desNum] = broadcast_target;

		printf("broadcast_list[%d] = broadcast_target", *desNum);

		++ (*desNum);
	}
}
