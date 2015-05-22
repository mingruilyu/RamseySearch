#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "server_transfer.h"
#include "search.h"
extern int desNum;
extern int active_count;
int send_count;
int collect_max = 10;
/*
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
void *(*start_routine) (void *), void *arg);

Compile and link with -pthread.

The pthread_create() function starts a new thread in the calling
process.  The new thread starts execution by invoking
start_routine(); arg is passed as the sole argument of
start_routine().
*/
#include <pthread.h>

extern struct broadcast* broadcast_list[100];

int main(int argc, char *argv[]) {
	// initialize server
	memset(&broadcast_list, '0', sizeof(broadcast_list));
	int p;
	for (p = 0; p < 100; ++ p) 
		memset(&broadcast_list[p], '0', sizeof(broadcast_list[p]));

	set_port();

	int err = 0;
	int i;
	int graph_count = 0;
	int max_graph_no = 40;
	char file_name[250];
	char* file_from_client = "DataFromClient.txt";
	pthread_t sock_recv_thread_id;
	err = pthread_create(&sock_recv_thread_id, NULL, server_listen_to_clients_handler, (void*)file_from_client);
	if (err != 0) perror("Could not create server_listen_to_clients_handler thread!");

	pthread_t broadcast_thread_id;
	err = pthread_create(&broadcast_thread_id, NULL, broadcast_handler, NULL);
	if (err != 0) perror("Could not create broadcast_handler thread!");

	while (1) {
	/*	pthread_t sock_send_thread_id;
		err = pthread_create(&sock_send_thread_id, NULL, send_to_des, (void*)broadcast_list);
		if (err != 0) perror("Could not create send_to_des thread!");

		err = pthread_join(sock_send_thread_id, NULL);
		if (err != 0) {
			printf("sock_thread goes wrong! %s \n", strerror(err));
			perror("sock_thread goes wrong!");
		}*/
		if (collect_count >= collect_max) {
			broadcast_graph();
			collect_count = 0;
			collect_max = 
		}
		max_graph_no = active_count;
	}

	return 0;
}

void broadcast_graph() {
	int i, err;
	for (i = 0; i < desNum; i ++) {
		int is_active = (*(broadcast_list + i))->active;
		if (is_active == -1) continue;

		char* ip_addr = (*(broadcast_list + i))->ipAddr;
		//char* file_name = (*(des_list + i))->fileName;
		sprintf(file_name, "../../file/server/CE_%d/%d", gsize, send_count % collect_max);
		send_count++;

		Broadcast* tmp = (Broadcast*)malloc(sizeof(Broadcast));
		construct_broadcast(tmp, ip_addr, file_name, 1);

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

void *broadcast_handler() {
	int err = pthread_detach(pthread_self());
	if (err != 0) {
		perror("Could not pthread_detach!");
	}
	while (1) {
		pthread_t sock_send_thread_id;
		err = pthread_create(&sock_send_thread_id, NULL, send_to_des, (void*)broadcast_list);
		if (err != 0) perror("Could not create send_to_des thread!");

		err = pthread_join(sock_send_thread_id, NULL);
		if (err != 0) {
			printf("sock_thread goes wrong! %s \n", strerror(err));
			perror("sock_thread goes wrong!");
		}
	}
	pthread_exit(0);
}
