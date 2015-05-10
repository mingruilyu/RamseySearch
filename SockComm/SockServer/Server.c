#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "Transfer.h"
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

struct broadcast* broadcast_list[20];

int main(int argc, char *argv[]) {
	struct broadcast** broadcast_list = (struct broadcast**)malloc(sizeof(struct broadcast*) * 20);

	char* file_from_client = "DataFromClient.txt";
	pthread_t sock_recv_thread_id;
	pthread_create(&sock_recv_thread_id, NULL, server_listen_to_clients_handler, (void*)file_from_client);
	
	while (1) {
		pthread_t sock_send_thread_id;
		pthread_create(&sock_send_thread_id, NULL, send_to_des, (void*)broadcast_list);
	}

	return 0;
}
