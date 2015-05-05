#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "list.h"
#include "fifo.h"	/* for taboo list */
#include "time.h"
#include "clique_count.h"
#define MAXSIZE (541)
#define TABOOSIZE (100000)
#define BIGCOUNT (1000)

/*
 * example of very simple search for R(7, 7) counter 
 * examples starts with a small randomized graph and 
 * works its way up to successively larger graphs one 
 * at a time
 * uses a taboo list of size #TABOOSIZE# to hold and 
 * encoding of and edge (i, j) + clique_count
 */

int main(int argc, char *argv[])
{
	typedef enum {false, true} bool;
	int *g, *new_g;
	int gsize, count = 50, i, j, rand_no, 
			best_count, best_i, best_j, try_count = 0,
			last_flip_i = -1, last_flip_j = -1, 
			last_flip_i_forward = -2,
			last_flip_j_forward = -2,
			mutation_col;
	void *taboo_list;
	// make a fifo to use as the taboo list
	taboo_list = FIFOInitEdge(TABOOSIZE);
	for(i = 0; i < 20; i ++) {
		for(j = 0; j < 20; j ++) {
			for(count = 0; count < 20; count ++)
				FIFOInsertEdgeCount(taboo_list, i, j, count);
		}
	}
	for(i = 0; i < 20; i ++) {
		for(j = 0; j < 20; j ++) {
			for(count = 0; count < 20; count ++) {
				if(!FIFOFindEdgeCount(taboo_list, i, j, count))
					printf("not found\n");
			}
		}
	}
	FIFODelete(taboo_list);
	return(0);
}

















