#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "client_transfer.h"
#include "search.h"
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

bool recv_flag = false;
int new_graph_count = 0;
char *ip_addr;
int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("You should input server IP address only!\n");
		exit(1);
	}

	set_port();

	int err = 0, gsize;
	int *g;
	ip_addr = argv[1];

	printf("IP_ADDR: %s\n", ip_addr);
	// create a listener thread
	pthread_t sock_recv_thread_id;
	err = pthread_create(&sock_recv_thread_id, NULL, client_always_listen_to_one_handler, NULL);
	if (err != 0) perror("Could not create client_always_listen_to_one_handler thread!");
	// init a connection with server
	send_check(ip_addr);
	while (1) {
		if (recv_flag) {
			recv_flag = false;
			g = load_graph(&gsize);
			search(g, gsize);
			send_request();
		}
		else sleep(10);
	}

	return 0;
}
