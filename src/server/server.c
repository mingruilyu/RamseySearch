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
int recv_count = 0;
int send_count = 0;
int collect_count = 0;
int gsize = 80;
int clique_count; // the clique count of the seeds we are currently waiting for
struct broadcast* broadcast_list[100];

void broadcast_graph();

int main(int argc, char* argv[]) {
	
	char dir_name[250];
	int p, err = 0;
	bool start_flag = true;
	clique_count = RECURSION_THRESHOLD - 1;
	// initialize server
	memset(&broadcast_list, '0', sizeof(broadcast_list));
	for (p = 0; p < 100; ++ p)
		memset(&broadcast_list[p], '0', sizeof(broadcast_list[p]));
	set_port();
	// create a listener thread
	pthread_t sock_recv_thread_id;
	err = pthread_create(&sock_recv_thread_id, NULL, server_listen_to_clients_handler, NULL);
	if (err != 0) perror("Could not create server_listen_to_clients_handler thread!");

	while (1) {
		if (collect_count >= GRAPH_COLLECT_NO || start_flag) {
			if(start_flag) start_flag = false;
			sprintf(dir_name, "../../file/server/CE_%d",  ++ gsize);
			mkdir(dir_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			broadcast_graph();
		}
		else {
			sleep(5);
			printf("current received graphs: %d\n", collect_count);
			printf("currently waiting for %d graphs\n", GRAPH_COLLECT_NO - collect_count);
			printf("current maximum counterexample %d\n", gsize - 1);
		}
	}
	return 0;
}
