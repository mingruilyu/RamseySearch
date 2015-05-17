#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

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

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("You should input server IP address only!\n");
		exit(1);
	}

	set_port();

	int err = 0;
	
	Broadcast* server = (Broadcast*) malloc(sizeof(Broadcast));
	construct_broadcast(server, argv[1], "DataToServer.txt", 1);
	
	printf("argv[1]: %s\n", server->ipAddr);
	printf("fileName: %s\n", server->fileName);

	char* file_from_server = "DataFromServer.txt";

	pthread_t sock_recv_thread_id;
	err = pthread_create(&sock_recv_thread_id, NULL, client_always_listen_to_one_handler, (void*)file_from_server);
	if (err != 0) {
		perror("Could not create client_always_listen_to_one_handler thread!");
	}

	while (1) {
		pthread_t sock_send_thread_id;
		err = pthread_create(&sock_send_thread_id, NULL, send_to_one_des, (void*)server);
		if (err != 0) {
			perror("Could not create send_to_one_des thread!");
		}

		err = pthread_join(sock_send_thread_id, NULL);
		if (err != 0) {
			printf("sock_thread goes wrong! %s \n", strerror(err));
			perror("sock_thread goes wrong!");
		}
		sleep(10);
	}

	return 0;
}
