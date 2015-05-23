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
#define GRAPH_COLLECT_NO 3
int desNum = 0;
int active_count = 0;
int send_count = 0;
int collect_count = 0;
int collected_graph_count = 2;
int gsize = 80;
/*
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
void *(*start_routine) (void *), void *arg);

Compile and link with -pthread.

The pthread_create() function starts a new thread in the calling
process.  The new thread starts execution by invoking
start_routine(); arg is passed as the sole argument of
start_routine().
*/

struct broadcast* broadcast_list[100];

void broadcast_graph();
void *broadcast_handler();

int main(int argc, char* argv[]) {
	
	char dir_name[250];
	int p, err = 0;
	bool start_flag = true;
	// initialize server
	memset(&broadcast_list, '0', sizeof(broadcast_list));
	for (p = 0; p < 100; ++ p)
		memset(&broadcast_list[p], '0', sizeof(broadcast_list[p]));
	set_port();
	// create a listener thread
	pthread_t sock_recv_thread_id;
	err = pthread_create(&sock_recv_thread_id, NULL, server_listen_to_clients_handler, NULL);
	if (err != 0) perror("Could not create server_listen_to_clients_handler thread!");


	// create regular check broadcast thread
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
		if (collect_count == GRAPH_COLLECT_NO || start_flag) {
			if(start_flag) start_flag = false;
			else collected_graph_count = collect_count;
			sprintf(dir_name, "../../file/server/CE_%d",  ++ gsize);
			mkdir(dir_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			collect_count = 0;
			broadcast_graph();
		}
		else {
			sleep(5);
			printf("current active computers: %d\n", active_count);
			printf("current received graphs: %d\n", collect_count);
			printf("currently waiting for %d graphs\n", GRAPH_COLLECT_NO - collect_count);
			printf("number of last ce %d\n", collected_graph_count);
			printf("current maximum counterexample %d\n", gsize - 1);
		}
	}

	return 0;
}

void broadcast_graph() {
	int i, err;
	for (i = 0; i < desNum; i ++) {
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
		sleep(10);
	}
	pthread_exit(0);
}
