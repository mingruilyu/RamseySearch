#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "list.h"
#include "fifo.h"	/* for taboo list */
#include "time.h"
#include "clique_count.h"
#include "search.h"
#include "graph.h"
#define TABOOSIZE (500)
#define BIGCOUNT (1000)
#define THRESHOLD (10)
/*
 * example of very simple search for R(7, 7) counter 
 * examples starts with a small randomized graph and 
 * works its way up to successively larger graphs one 
 * at a time
 * uses a taboo list of size #TABOOSIZE# to hold and 
 * encoding of and edge (i, j) + clique_count
 */

int search(int *g, int gsize, int new_graph_count, bool *recv_flag) {
	int count, i, j, best_count = BIGCOUNT, best_i, best_j;
	void *taboo_list = FIFOInitEdge(TABOOSIZE);
	while(!*recv_flag) {
		CliqueCountCreateCache(g, gsize);
		// see how many clique 7 we have left. If there are still 
		// a lot, we will need to clean it up.
		if(cache_7.length == 0) {
			printf("Eureka! Counter-example found!\n");
			FIFODelete(taboo_list);
			PrintGraphNew(g, gsize, new_graph_count);
			free(g);
			return (0);
		}
		// do the second round of clean up by breaking the ties
		// since best_count is initialized to BIGCOUNT, we won't
		// have 
		for(i = 0; i < gsize; i ++) {
			for(j = i + 1; j < gsize; j ++) {
        g[i * gsize + j] = 1 - g[i * gsize + j];
				count = CliqueCountUseCache(g, gsize, i, j, best_count);
				if(count != -1 && !FIFOFindEdgeCount(taboo_list, i, j, count)) {
  	      if(count < best_count) {
    	      best_count = count;
            best_i = i;
            best_j = j;
					}
				}
        g[i * gsize + j] = 1 - g[i * gsize + j];
			}
		} 
							
		printf("BACKTRACKING size: %d, best_6_count: %d, best_count: %d, best edge: (%d, %d), new color: %d\n",
					 gsize, cache_6.length, best_count, best_i, best_j, 
					 g[best_i * gsize + best_j]); 
		// keep the best flip we saw. 
		g[best_i * gsize + best_j] = 1 - g[best_i * gsize + best_j];
		FIFOInsertEdgeCount(taboo_list, best_i, best_j, best_count);
		best_count = BIGCOUNT;
		PrintGraph(g, gsize);
	}
	return (-1);
}

int *load_graph(int* gsize) {
	int* new_g, *g, i;
	ReadGraph("../../file/client/old_graph", &g, gsize);
	new_g = (int *) malloc((*gsize + 1) * (*gsize + 1) * sizeof(int));
	// copy the old graph into the new graph leaving 
	// the last row and last column alone
	CopyGraph(g, *gsize, new_g, *gsize + 1);
	// throw away the old graph and make new one 
	free(g);
	g = new_g;
		// randomly assigned value for last column
	for(i = 0; i < *gsize + 1; i ++) {
		if(rand() % 100 > 50) {
			new_g[i * (*gsize + 1) + *gsize] = 1; // last column
			new_g[*gsize * (*gsize + 1) + i] = 1;	
		}
		else {
			new_g[i * (*gsize + 1) + *gsize] = 0; // last column
			new_g[*gsize * (*gsize + 1) + i] = 0;	
		}
	}
	*gsize = *gsize + 1;
	// reset the taboo list for the new graph
	return g;
}
